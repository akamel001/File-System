#include "table.h"

Table::Table(fileSys &file_sys, string fltfile, string indxfile){
	fs = &file_sys;
	flatfile = fltfile;
	indexfile = indxfile;
	fs -> newFile(flatfile);
	fs -> newFile(indexfile);
}

int Table::build_table(string in_file){
	string block, record, date;
	int x;
	vector<string> blocks;
	stringstream ss;
	ifstream file(in_file.c_str());

	if(!file.good()){
		cout << "build_table(): Error could not open input file\n";
		return -1;	
	}
	
		
	getline(file, record);
	while(!file.eof())	
		{

			x = record.find("*");
			date = record.substr(0,x);
			//remove white spaces
			x = date.find(" ");
			if(x != string::npos)
				date = date.substr(0,x);

			ss << date << " " ;

			int flatfile_block_num = fs -> addBlock(flatfile,record);
			ss << flatfile_block_num << " ";
			fs -> addBlock(indexfile, ss.str());
			block.clear();
			ss.str("");
			ss.clear();
			
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

int Table::index_search(string value){
	int current_block = fs -> getFirstBlock(indexfile), blk_num;
	string block, date;
	stringstream ss;

	while( current_block != 0){
		fs -> getBlock(indexfile, current_block, block);
		ss.str(block);
		ss >> date >> blk_num;
		
		if(date == value)
			return blk_num;

		current_block = fs -> nextBlock(indexfile, current_block); 
	}
	
	return -1;
}
