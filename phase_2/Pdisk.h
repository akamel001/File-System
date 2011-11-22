/*
 * Author: Abdelrahman Kamel
 * Assignment: CSCI 350 project 2
 * Description: Pdisk class definitions 
*/

#ifndef PDISK_H
#define PDISK_H


#include <string>
#include <iostream>

using namespace std;

class Pdisk
{
	public:
		Pdisk(string);
		Pdisk(string ,int , int );
		int getblock(int , string &);
		int putblock(int , const string &);
		int getBlockSize(){return block_size;}
		int getNumOfBlocks(){return number_of_blocks;}
		string getDiskName(){return disk_name;}
		friend class Filesys;
	private:
		string disk_name;
		int number_of_blocks;
		int block_size;
};

#endif
