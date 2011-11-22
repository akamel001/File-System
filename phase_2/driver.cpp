/*
 * Author: Abdelrahman Kamel
 * Assignment: CSCI 350 project 2
 * Description: A test driver for the simulation disk
*/

#include <iostream>
#include <fstream>

#include "fileSys.h"

using namespace std;

int main()
{
  Pdisk disk("disk",64,128);
  fileSys fs(disk);
  
  int error = fs.newFile("file1");
  		error = fs.newFile("file2");
  
  string block;
  
  for(int i=0; i < 128; i++) block += "7";
  
  error = fs.addBlock("file2", block);	
	 
  fs.fsClose();
}
