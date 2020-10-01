#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <float.h>

using namespace std;

// a quick process of nc file outputs
// g++ -std=c++11 -o process_log process_log.cpp

int main (int argc, char *argv[]) {
  if(argc != 2 ){
    cout<<"Usage: process_log <file_name> "<<endl;
    return 1;
  }
  
  ifstream in_file;
  in_file.open(argv[1], ios::in);
  char tmp[1024];
  while (!in_file.eof() ) {
    string s;
    string dd;
    in_file>> s;
    while (!in_file.eof() && s.compare("Region") != 0) in_file>> s;
    if(in_file.eof()) return 0;
    // found region 0
    for(int i = 0; i < 19; ++i ){
      int jj ;
      in_file>> jj;
      if(jj != i) { cout << "Number mismatch "<< jj <<" "<< i <<endl; return 1;}
      in_file>>s>>dd;
      in_file.getline(tmp, 1024);
      cout << dd<<"\t";
      if(i <18) {
	while (!in_file.eof() && s.compare("Region") != 0) {
	  in_file>> s;
	}
      }
    }
    cout <<endl;
  }
  return 0;
}


    
    	  
    
