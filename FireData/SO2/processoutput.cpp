#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>

// icpc -std=c++11 -o process_so2 processfile.cpp
using namespace std;

struct so2data {
  string cityname;
  string countryname;
  float lat;
  float longt;
  vector<int> years;
  vector<int> monthcnt;
  vector<float> pbls;
  vector<float> tbls;
  vector<float> tbms;
};

void openfile(const char *file, so2data* ss  ) {
  
  ifstream data_file(file, ios::in);
  if(! data_file.is_open()) {
    cout <<"Can't open "<<file<<endl;
    return;
  }
  //cout <<"Opening "<<file<<endl;
  string temp;
  data_file >>temp;
  data_file >>ss->cityname;
  data_file >>ss->countryname;
  //cout << ss->cityname;
  data_file >>ss->lat;
  data_file >>ss->longt;
  int cnt = 0 ;
  std::getline(data_file,temp);
  std::getline(data_file,temp);
  std::getline(data_file,temp);
  //cout <<temp<<endl;
  while (!data_file.eof()) {
    int y =0 ; 
    data_file >> y; 
    if(y == 0) break;
    ss->years.push_back(y);
    data_file >> y; // for the month count
    ss->monthcnt.push_back(y);
    float f;
    data_file>> f; 
    ss->pbls.push_back(f);
    data_file>> f; 
    ss->tbls.push_back(f);
    data_file>> f; 
    ss->tbms.push_back(f);
    cnt++;
  }
  //cout <<"  total cnt "<<cnt<<endl;
  data_file.close();
  
}

int  main () {

  // read in file names that we want to process
  vector <string> filenames ;
  ifstream filens("fileoutputname.txt", ios::in);
  if(! filens.is_open()) {
    cout <<"Can't open "<<"fileoutputname.txt"<<endl;
    return 1;
  }
  string fname; 
  vector<so2data*> vec;
  while(!filens.eof()){
    filens >> fname;
    if(fname.length() < 4) continue;
    filenames.push_back(fname);
    so2data* ss = new so2data();
    openfile(fname.c_str(), ss);
    vec.push_back(ss);
    fname.clear();
  }
  filens.close();
  //cout <<"total "<<vec.size()<<endl;
  for(int i =0; i< vec.size(); i++ ) {
    cout <<vec[i]->cityname<<"."<<vec[i]->countryname<<"."<<vec[i]->lat<<"."<<vec[i]->longt<<"\t";
  }
  cout <<"\n";
  
  for(int j =0; j < vec[0]->years.size(); j++ ) {
    cout << vec[0]->years[j]<<"\t"<<vec[0]->monthcnt[j]<<"\t";
      for(int i =0; i< vec.size(); i++ ) {
	cout <<vec[i]->pbls[j]<<"\t";
      }
      cout <<"\n";
    }
  cout <<"\n\n\n\n";
  for(int j =0; j < vec[0]->years.size(); j++ ) {
      cout << vec[0]->years[j]<<"\t";
      for(int i =0; i< vec.size(); i++ ) {
	cout <<vec[i]->tbls[j]<<"\t";
      }
      cout <<"\n"; 
    }
   cout <<"\n\n\n\n";
   for(int j =0; j < vec[0]->years.size(); j++ ) {
       cout << vec[0]->years[j]<<"\t";
    for(int i =0; i< vec.size(); i++ ) {
      cout <<vec[i]->tbms[j]<<"\t";
    }
      cout <<"\n"; 
    }
 
  return 0;
}
