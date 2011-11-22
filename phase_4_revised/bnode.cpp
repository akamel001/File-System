#include "bnode.h"

void Bnode::write(){
	ostringstream out;
	
	out << keys.size() << " ";

	for(int i = 0; i < keys.size(); i++)
		out << keys[i] << " " ;
	
	for(int i = 0; i < links.size(); i++)
		out << links[i] << " " ;

	vector<string> s = fs->block( out.str(), fs->getBlockSize() );
	string buffer = s[0];

	fs -> putBlock(file, block, buffer);
}

int Bnode::getBlock(){
	return block;
}

vector<string> Bnode::getKeys(){
	return keys;
}

vector<int> Bnode::getLinks(){
	return links;
}

void Bnode::update(vector<string> keys, vector<int> links){
	this -> keys = keys;
	this -> links = links;
	this -> write();
}

Bnode::Bnode( fileSys &fileSystem, string fyle, vector<string> kys, vector<int> lnks){
	fs = &fileSystem;
	file = fyle;
	keys = kys;
	links = lnks;

	string buffer;
	buffer = "omgwtfbbq";

	block = fs -> addBlock(file, buffer);
	write();
}

Bnode::Bnode(fileSys &fileSystem, string fyle, int blk){
	string buffer, stmp;
	int size, itmp;
	istringstream in;

	fs = &fileSystem;
	file = fyle;
	block = blk;
	fs -> getBlock(file, block, buffer);
	in.str(buffer);
	in >> size;

	for (int i =0; i < size; i++){
		in >> stmp;
		keys.push_back(stmp);
	}

	for(int i = 0; i < size; i++){
		in >> itmp;
		links.push_back(itmp);
	}
}
