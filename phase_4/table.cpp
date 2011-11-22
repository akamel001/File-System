#include "table.h"
#include <algorithm>
Table::Table(fileSys &file_sys, string fltfile, string bindxfile){
	fs = &file_sys;
	flatfile = fltfile;
	bindexfile = bindxfile;
	fs -> newFile(flatfile);
	fs -> newFile(bindexfile);
	check_n_load();
}

string Table::date_trunc(string buffer){
	int x;
	x = buffer.find("*");
	buffer = buffer.substr(0,x);
	x = buffer.find(" ");
	if( x != string::npos )
		buffer = buffer.substr(0,x);
	
	return buffer;
}

int Table::check_n_load(){
	ifstream file("root");
	
	if(file.is_open()){
		cout << "A table already exists!" << endl;
		file >> root;
		return -1;
	}else
		return 1;	
}

void Table::save_root(){
	ofstream file("root");
	file << root;	
	file.close();
}

int Table::build_table(string in_file){
	if( check_n_load() == -1)
		return -1;

	string block, record, date;
	int blk_num;
	vector<string> blocks, keys;
	vector<int> links;
	stringstream ss;
	ifstream file(in_file.c_str());

	if(!file.good()){
		cout << "build_table(): Error could not open input file\n";
		return -1;	
	}
	
	getline(file, record);
	blk_num = fs -> addBlock(flatfile, record);
	
	date = date_trunc(record);
	irec r1(date, blk_num);
	
	getline(file, record);
	blk_num = fs -> addBlock(flatfile, record);
	
	date = date_trunc(record);
	irec r2(date, blk_num);
	
	build_root(r1, r2);
	save_root();

	getline(file, record);
	while(!file.eof())	
	{
		blk_num = fs -> addBlock(flatfile, record);
		date = date_trunc(record);
		irec rec(date, blk_num);
		add_index(rec);
		cout << "Index record " << date << " added.\n";
		getline(file, record);
	}
}

int Table::search(string value){
	string buffer, date, end, type, place, ref, desc;
	vector<string> record;

	int block_number = index_search(value), lastPos, pos;
	if(block_number == -1){
		cout << "Record was not found." << endl;
		return -1;
	}
	fs -> getBlock(flatfile,block_number,buffer);
	
	//slik way of tokanizing the buffer
	lastPos = buffer.find_first_not_of("*",0);
	pos = buffer.find_first_of("*", lastPos);

	while (pos != string::npos || lastPos != string::npos){
		record.push_back( buffer.substr(lastPos, pos-lastPos));
		lastPos = buffer.find_first_not_of("*", pos);
		pos = buffer.find_first_of("*", lastPos);
	}
	
	cout << "Record found: " << endl;
	cout << "\tDate: " << record.at(0) << endl;
	cout << "\tEnd: " << record.at(1) << endl;
	cout << "\tType: " << record.at(2) << endl;
	cout << "\tPlace: " << record.at(3) << endl;
	cout << "\tReference: " << record.at(4) << endl;
	cout << "\tDescription: " << record.at(5) << endl << endl;;
	return 0;
}

int Table::btree_search(int node, string key){
	
	Bnode thisNode( *fs, bindexfile, node);
	vector<string> keys = thisNode.getKeys();
	vector<int> links = thisNode.getLinks();
	
	if(links.at(0) == -1){//external node
		for(int i = 0; i < keys.size(); i++){
			cout << keys.at(i) << " ";
			if(keys.at(i) == key)
				return links.at(i);
		}
		cout << "\next node and returning your -1\n";
		return -1;
	}else{//recursion (internal node)
		int i = 1;
		
		for(int i = 0; i < keys.size(); i++)
			cout << keys.at(i) << " ";
		cout << endl;
		for(int i = 0; i < links.size(); i++)
			cout << links.at(i) << " ";
		cout << endl;
			
		while (i < keys.size() and  convert_to_int(key) <= convert_to_int(keys.at(i))) i++; 
		
		int next_block = links.at(i-1);
		cout << "using link " << next_block << endl;
		
		return btree_search(next_block, key);
	}
}

int Table::convert_to_int(string value){
	int x;
	stringstream ss;

	ss << value; 
	ss >> x;
	return x;  
}

void Table::build_root(irec r1, irec r2){
	
	if(convert_to_int(r1.getKey()) > convert_to_int(r2.getKey())){
		string tmp;
		tmp = r2.getKey();
		r2.setKey(r1.getKey());
		r1.setKey(tmp);
		
		int blk;
		blk = r2.getBlock();
		r2.setBlock(r1.getBlock());
		r1.setBlock(blk);	
	}

	vector<string> keys;
	vector<int> blocks;
	
	keys.push_back("uu");
	keys.push_back(r1.getKey());

	blocks.push_back(-1);
	blocks.push_back(r1.getBlock());

	Bnode lchild(*fs, bindexfile, keys, blocks);

	keys.clear();
	blocks.clear();

	keys.push_back("uu");
	keys.push_back(r2.getKey());

	blocks.push_back(-1);
	blocks.push_back(r2.getBlock());

	Bnode rchild(*fs, bindexfile, keys, blocks);

	keys.clear();
	blocks.clear();
	
	keys.push_back("uu");
	keys.push_back(r1.getKey());

	blocks.push_back(lchild.getBlock());
	blocks.push_back(rchild.getBlock());
	
	Bnode rooot(*fs, bindexfile, keys, blocks);
	this->root = rooot.getBlock();	
}

void Table::add_index(irec record){
	
	irec r1 = add_index_record(root, record);
	
	if(r1.getBlock() != -1){
		cout << "New Root!" << endl;
		vector<string> keys;
		vector<int> links;
		
		keys.push_back("uu");
		keys.push_back(r1.getKey());
		links.push_back(this->root);
		links.push_back(record.getBlock());
		Bnode newRoot(*fs, bindexfile, keys, links);
		this->root = newRoot.getBlock();
		save_root();
	}
}

irec Table::add_index_record(int node, irec record){
	Bnode thisnode(*fs, bindexfile, node);
	
	vector<int> links = thisnode.getLinks();
	vector<string> keys = thisnode.getKeys();

	if(links.at(0) == -1){//extrn node
		return add_ext_node(node, record);
	}else{
		int index=0;
		string key = record.getKey();

		for(int i = 1; i < keys.size() and convert_to_int(key) > convert_to_int(keys.at(i)); i++)
			index = i-1;
		
		irec r = add_index_record(links.at(index), record);
		
		if(r.getBlock() == -1) return r;
		
		return add_int_node(node, r);
	}
}

irec Table::add_int_node(int node, irec record){
	Bnode inode(*fs, bindexfile, node);
	vector<string> keys = inode.getKeys();
	vector<int> links = inode.getLinks();
	
	keys.push_back(record.getKey());
	links.push_back(record.getBlock());

	int i = keys.size()-2;
	
	//cannot compair keys.at(i+1) > keys.at(i)
	while( i> 0 and convert_to_int(keys.at(i)) < convert_to_int(keys.at(i+1))){
		swap(keys.at(i), keys.at(i+1));
		swap(links.at(i), links.at(i+1));
		i--;
	}
	
	if(keys.size() > 5)// overflow
		return isplit(node, keys, links);
	else{
		inode.update(keys, links);
		return irec("okay", -1);
	}
}

irec Table::isplit(int node, vector<string> keys, vector<int> links){
	Bnode lchild(*fs, bindexfile, node); 
	vector<string> left_keys, right_keys;
	vector<int> left_links, right_links;
	int med = keys.size()/2;
		
	for(int i = 0; i <= med; i++){
		left_keys.push_back(keys.at(i));
		left_links.push_back(links.at(i));
	}
	lchild.update(left_keys, left_links);

	right_keys.push_back("uu");
	right_links.push_back(links.at(med+1));//was med+1

	for(int i = med+2; i < keys.size(); i++){
		right_keys.push_back(keys.at(i));
		right_links.push_back(links.at(i));
	}

	Bnode rchild(*fs, bindexfile, right_keys, right_links);
	
	return irec(keys.at(med+1), rchild.getBlock());
}

irec Table::esplit(int node, vector<string> keys, vector<int> links){
	int med = keys.size()/2;
	vector<string> left_keys, right_keys;
	vector<int> left_links, right_links;
	
	for(int i = 0; i <= med; i++){
		left_keys.push_back(keys.at(i));
		left_links.push_back(links.at(i));
	}

	right_keys.push_back("uu");
	right_links.push_back(-1);

	for(int i = med+1; i < keys.size(); i++){
		right_keys.push_back(keys.at(i));
		right_links.push_back(links.at(i));
	}

	Bnode lchild(*fs, bindexfile, node); 
			lchild.update(left_keys, left_links);

	Bnode rchild(*fs, bindexfile, right_keys, right_links);
	
	return irec(left_keys.at(med), rchild.getBlock());
}

irec Table::add_ext_node(int node, irec record){
	Bnode enode(*fs, bindexfile, node);
	vector<string> keys = enode.getKeys();
	vector<int> links = enode.getLinks();
	
	keys.push_back(record.getKey());
	links.push_back(record.getBlock());

	int i = keys.size()-2;
	
	while( i> 0 and convert_to_int(keys.at(i)) < convert_to_int(keys.at(i+1))){
		swap(keys.at(i), keys.at(i+1));
		swap(links.at(i), links.at(i+1));
		i--;
	}
	
	if(keys.size() > 5)// overflow
		return esplit(node, keys, links);
	else{
		enode.update(keys, links);
		return irec("okay", -1);
	}
	
}

int Table::index_search(string value){
	return btree_search(root, value);
}
