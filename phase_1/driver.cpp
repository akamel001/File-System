/*
 * Author: Abdelrahman Kamel
 * Assignment: CSCI 350 project 1
 * Creation date: 10/3/2009
 * Description: A test driver for the simulation disk
*/

#include <iostream>
#include <fstream>
#include "Pdisk.h"
using namespace std;

int main()
{
  Pdisk disk1("test1",16,32);
  //Pdisk disk1("test1");
  string block1, block2, block3, block4;
  for (int i=1; i<=32; i++) block1=block1+"1";
  for (int i=1; i<=32; i++) block2=block2+"2";
  disk1.putblock(4,block1);
  disk1.getblock(4,block3);
  cout << "Should be 32 1s : ";
  cout << block3 << endl;
  disk1.putblock(8,block2);
  disk1.getblock(8,block4);
  cout << "Should be 32 2s : ";
  cout << block4 << endl;;
}
