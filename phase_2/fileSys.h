#ifndef FILESYS_H
#define FILESYS_H

#include "Pdisk.h"
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstdlib>

class fileSys{

public:
	fileSys(Pdisk &);
	int fsClose();
	int fsSynch();
	int newFile(string);
	int rmFile(string);
	int getFirstBlock(string);
	int addBlock(string, string);
	int delBlock(string, int);
	int getBlock(string, int, string &);
	int putBlock(string, int, string);
	int nextBlock(string, int);
	
private:
	Pdisk disk;
	int rootSize;
	int fatSize;
	int dirty;
	vector<string> fileName;
	vector<int> firstBlock;
	vector<int> fat;
	
	//helper functions	
	vector<string> block(string, int);
	int check_block(int, string);
	void init();
	void make_fs();
	void load_fs();
	int check_n_pad(string &);
	
};

#endif
