#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include"a.proto.txt.pb.h"
#include "md6/md6.h"




namespace fs = boost::filesystem;
using namespace std;

struct Info { 
string Path;
string Hash;
int size;
string Flag = "New";
};

std::vector<Info> compare_lists(std::vector<Info> newfl, std::vector<Info> oldfl) {
	for (std::vector<Info>::iterator itnew = newfl.begin(); itnew < newfl.end(); itnew++) {
		
		for (std::vector<Info>::iterator itold = oldfl.begin(); itold < oldfl.end(); itold++) {
			if ((itnew->path == itold->path) && (itnew->hash == itold->hash)) {
				itnew->flag = "Not changed";
				oldfl.erase(itold);
				break;
			}
			if ((itnew->path == itold->path) && (itnew->hash != itold->hash)) {
				itnew->flag = "Changed";
				oldfl.erase(itold);
				break;
			}
		}
	}
	for (std::vector<Info>::iterator itold = oldfl.begin(); itold < oldfl.end(); itold++) {
		itold->flag = "Deleted";
		newfl.push_back(*itold);
	}
	return newfl;
}


void savepbuf (string Filename, vector <Info> vec_finfo) {
	nsofdir:: ArrFilep flist;
	nsofdir:: Filep * file_entry;
	ofstream output (filename, ofstream::binary);

	
	for (Info it : vec_finfo) {	
		file_entry = fList.add_filep();
		file_entry -> set_filepath(it.path);
		file_entry -> set_size(it.size);
		file_entry -> set_mdsixhash(it.hash); 
	}

	flist.PrintDebugString(); 
	flist.SerializeToOStream(&output); 
	output.close();
}


void loadpbuf (string Filename, vector <Info> vec_finfo) {
	Info it;
	nsofdir:: ArrFilep flist; 
	nsofdir:: Filep * file_entry;
	ifstream input(filename, ofstream::binary); 
	flist.ParseFromIstream(&input);
	input.close();
	flist.PrintDebugString();
	for (int i = 0; i < flist.filep_size(); i++) {
		file_entry = flist.filep(i);
		it.path = file_entry.filepath();
		it.size = file_entry.size();
		it.hash = file_entry.mdsixhash();
		vec_finfo.push_back(it);
	}
}

void get_dir_list(fs::directory_iterator iterator, vector<Info> & vec_info Info & finfo ifstream & ifs) {
	for (; iterator != fs::directory_iterator(); ++iterator) 
	{
		if (fs::is_directory(iterator->status())) { 
			fs::is_directory_iterator sub_dir(iterator->path());
			get_dir_list(sub_dir, vec_finfo, finfo, ifs);
		}
		else 
		{ 
			finfo.path = iterator->path().string();
			replace(finfo.path.begin(), finfo.path.end(), '\\', '/'); 
			finfo.size = fs::file_size(iterator->path());
			ifs.open(finfo.path, std::ios_base::binary);
			string strifs((istreambuf_iterator<char>(ifs)),
				(istreambuf_iterator<char>()));
			finfo.hash = md6(strifs);
			ifs.close();
			vec_finfo.push_back(finfo);
		}
	}
}

void print_finfo_vec(vector<Info> vec) {
	for (Info element : vec) {
		cout << element.path << endl <<
			element.size << endl <<
			element.hash << endl <<
			element.flag << endl << "***************" << endl;
	}
}

int main() {
	ofstream myfile;
	string path, dirpath;
	Info finfo;
	ifstream ifs;
	string checkstatus;
	cout << "Save for save file, check for compare" << 
		endl << "(check/save)" << endl;
	
	cin >> checkstatus;
	cin.clear();
	fflush(stdin);
	cout << "Folder path: " << endl;
	getline (cin, path);
	vector <Info> vec_info; 
	vector <Info> vec_info_old; 
	try {
	fs:: directory_iterator home_dir(path); 
	get_dir_list(home_dir, &vec_finfo); 
	}

	catch (const boost::filesystem::filesystem_error& e) {
		std::cout << "Wrong path" << std::endl;
		checkstatus = "null";
	}
	if (checkstatus == "save") {
		savepbuf("filelist.pb", vec_finfo);
		print_finfo_vec(vec_finfo);
	}
	if (checkstatus == "check") {
		loadpbuf("filelist.pb", vec_finfo_old);
		print_finfo_vec(compare_lists(vec_finfo, vec_finfo_old));
	}
	if ((checkstatus != "save") && (checkstatus != "check")) {
		print_finfo_vec(vec_finfo);
	}

	savepbuf("FileList.pb", vec_finfo); 
	std:: cin.clear();
	std:: fflush(stdin);
	std:: cin.get();
	return 0;
}
