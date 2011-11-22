#ifndef TABLE_H
#define TABLE_H

#include "fileSys.h"

class Table{
	public:
		Table(fileSys &, string, string);
		int build_table(string);
		int search(string);
	private:
		int index_search(string);
		string flatfile;
		string indexfile;
		fileSys *fs;
};

#endif 
