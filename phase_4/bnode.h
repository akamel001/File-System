#ifndef BNODE_H
#define BNODE_H

#include "table.h"

class Bnode{
	public:
		Bnode(fileSys &, string, int);
		Bnode(fileSys &, string, vector<string>, vector<int>);
		int getBlock();
		vector<string> getKeys();
		vector<int> getLinks();
		void write();
		void update(vector<string>, vector<int>);
	private:
		fileSys * fs;
		string file;
		int block;
		vector<string> keys;
		vector<int> links;
};

#endif
