/*
 * Author: Abdelrahman Kamel
 * Assignment: CSCI 350 project 1
 * Creation date: 10/3/2009
 * Description: Member functions implementation of Pdisk class
*/

#include "Pdisk.h"
#include <iostream>
#include <fstream>

using namespace std;

Pdisk::Pdisk(string disk){
	disk_name = disk;
	
	ifstream file((disk+".spc").c_str());
	
	if(file.is_open())
		file >> number_of_blocks >> block_size;
	else
		cout << "Error opening disk, perhaps disk does not exist." << endl; 

	file.close();
}

Pdisk::Pdisk(string new_disk_name, int new_num_of_blocks, int new_block_size){

	ifstream test((new_disk_name + ".spc").c_str());

	if(test.is_open()){
		cout << "File already exists. Constructor now exiting." << endl;
		disk_name = new_disk_name;
		test >> number_of_blocks >> block_size;
		test.close();
		return;
	}
	
	test.close();
	//adding the spec's of the disk
	ofstream file((new_disk_name + ".spc").c_str());
	
	file << new_num_of_blocks << " " << new_block_size;
	file.close();
	
	//adding empty byptes
	file.open((new_disk_name+".dat").c_str());
	for(int i = 1; i <= new_num_of_blocks*new_block_size; i++){
		file << "#";
	}
	file.close();

	disk_name = new_disk_name;		
	number_of_blocks = new_num_of_blocks;
	block_size = new_block_size;
	
}

int Pdisk::getblock(int blk_number, string &buffer){
	
	if(blk_number > number_of_blocks){
		cout << "Error in getblock(). Block number does not exist!" << endl;
		return 0;
	}
	
	fstream file((disk_name+".dat").c_str());
	file.seekp(blk_number*block_size-1);
	
	//clearing buffer before use!
	buffer.clear();
	
	char tmp;
	for(int i = blk_number*block_size; i <= ((blk_number*block_size)+block_size-1); i++){
		file.get(tmp);
		buffer.push_back(tmp);
	}
	
   if(!file.good()){
      cout << "Error opening (or writing to) file." << endl;
      return 0;
   }else
      return 1;
}

int Pdisk::putblock(int blk_num, const string &buffer){
	
	if(buffer.size() > block_size){
		cout << "Error in putblock(). Buffer size is too large!" << endl;
		return 0;
	}

	if(blk_num > number_of_blocks){
		cout << "Error in putblock(). Block number does not exist!" << endl;
		return 0;
	}

	fstream file((disk_name+".dat").c_str());
	file.seekp(blk_num*block_size-1);

	for(int i = blk_num*block_size, counter=0; i <= ((blk_num*block_size)+block_size-1); i++)
		file << buffer[counter++];
	file.close();

	if(!file.good()){
		cout << "Error opening (or writing to) file." << endl;
		return 0;
	}else
		return 1;
}
