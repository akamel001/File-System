#ifndef TABLE_H
#define TABLE_H

#include "fileSys.h"
#include "irec.h"
#include "bnode.h"

class Table{
	public:
		Table(fileSys &, string, string);
		int build_table(string);
		void build_root(irec, irec);
		int search(string);
		void add_index(irec);
	private:
		int btree_search(int, string);
		int check_n_load();
		void save_root();
		int convert_to_int(string);
		irec add_index_record(int, irec);
		irec add_ext_node(int, irec);
		irec add_int_node(int, irec);
		irec esplit(int, vector<string>, vector<int>);
		irec isplit(int, vector<string>, vector<int>);
		string date_trunc(string);
		int root; 
		int index_search(string);
		string flatfile;
		string bindexfile;
		fileSys *fs;
};

#endif 
