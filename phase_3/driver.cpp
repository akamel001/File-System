/*
 * Author: Abdelrahman Kamel
 * Assignment: CSCI 350 project 1
 * Creation date: 10/3/2009
 * Description: A test driver for the simulation disk
*/

#include <iostream>
#include <fstream>

#include "table.h"

using namespace std;

int main()
{
  Pdisk disk("disk",500,128);
  fileSys fs(disk);
  
  Table tb(fs, "flatfile", "indxFl");
  //tb.build_table("data2009.txt");
  string x ;
  cout << "Enter the Date: ";
  cin >> x;
  cout << endl;
  tb.search(x);

  fs.fsClose();
}
