/*
 * Author: Abdelrahman Kamel
 * Assignment: CSCI 350 project 1
 * Creation date: 10/3/2009
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
	private:
		string disk_name;
		int number_of_blocks;
		int block_size;
};

#endif
