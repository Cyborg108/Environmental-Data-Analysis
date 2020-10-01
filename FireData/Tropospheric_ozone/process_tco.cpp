#include <iostream>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <algorithm>
#include <vector>

using namespace std;

// icpc -std=c++11 -o process_tco process_tco.cpp
// this is to process tco_oct04_to_dec13.csv data
// from http://acdb-ext.gsfc.nasa.gov/Data_services/cloud_slice/new_data.html
// Below are the webpage links for two CSV files of tropospheric ozone.
// One file is tropospheric column ozone (TCO, in Dobson Units) and the
// other is tropospheric ozone mean volume mixing ratio (VMR, in units
// parts per billion). The two files each have three columns: (1)
// first column is ozone value, (2) second column is longitude, and (3)
// third column is latitude.

// update in 2/6/2017 
// download from ftp://jwocky.gsfc.nasa.gov/pub/ccd/data_monthly/
// tco_oct04_to_dec16.csv

struct regions {
  float lat1, lat2;
  float long1, long2;
  //area = pi*R^2* | sin(lat1) - sin(lat2) | *| (long1 - long2)| / 180
  // lat and long in degree
  float area;
  vector < float> data;
};

bool operator< (const regions& r1, const regions& r2) { 
  if(r1.lat1 == r2.lat1) {
    if(r1.lat2 == r2.lat2)
      return (r1.long1 < r2.long2);
    else
      return (r1.lat1 < r2.lat2);
  }
  else
    return (r1.lat1 < r2.lat2);
}

int main (int argc, char *argv[])
{

  ifstream region_file;
  vector<regions> vRegion;
  region_file.open("../coords.txt", ios::in);
  while (!region_file.eof() ) { 
    float i1=0, i2=0, i3=0, i4=0;
    region_file>>i1 >> i2 >> i3 >> i4;
    if( i1 ==0 && i2 ==0 ) break;
    regions r1;
    r1.lat1 = i1;
    r1.lat2 = i2;
    r1.long1 = i3;
    r1.long2 = i4;
    //area = pi*R^2* | sin(lat1) - sin(lat2) | *| (long1 - long2)| / 180
    r1.area = 3.1415926f * 6371.0f*6371.0f * abs(sin( (float)r1.lat1/180.0f*3.1415926f) - sin((float)r1.lat2/180.0f*3.1415926f ) ) * abs(r1.long1 - r1.long2 ) / 180.0f;
    vRegion.push_back(r1);
  }
  //cout <<"hello "<< vRegion[2].lat1 <<" "<<vRegion[2].lat2 << " "<< vRegion[2].long1 <<" "<<vRegion[2].long2<<endl;
  region_file.close();

  sort(vRegion.begin(), vRegion.end());

  cout <<"Read in "<<vRegion.size()<<endl;
  for(int ii =0; ii < vRegion.size(); ++ii) {
    cout <<vRegion[ii].lat1 <<" "<<vRegion[ii].lat2<<" "
	 <<vRegion[ii].long1 <<" "<<vRegion[ii].long2<<" area "<<vRegion[ii].area<< endl;
  }

  const int startyear = 2004;
  const int endyear = 2016;
  const int startmonth = 10;
  const int endmonth = 12;

  string filename = "tco_oct04_to_dec16.csv";
  ifstream data_file(filename.c_str(), ios::in);
  if(! data_file.is_open()) {
    cout <<"Can't open "<<filename<<endl;
	return 1;
  }
      
  // there are total 111 month
  // each month has 2592 lines
  for(int i = startyear; i <= endyear; i++ ){
    vector<float> regionannual(vRegion.size(), 0.0f);
    int moncount = 0;
    int j = 1;
    if(i == startyear) j = startmonth;
    for( ; j <= 12; j++) {
      //if(i == endyear  && j > endmonth) break;
      
      string sdate = to_string(i)+ string("-");
      if( j < 10) sdate += string("0");
      sdate += to_string(j) ;
      
      for(int ii =0; ii < vRegion.size(); ++ii) {
	vRegion[ii].data.clear();
      }

      int lines = 0;
      while(lines < 2592) {
	float val, longi, lat;
	data_file >> val >>  longi >> lat;
	
	for(int ii =0; ii < vRegion.size(); ++ii) {
	  if( ( lat > vRegion[ii].lat1-2.5 && lat < vRegion[ii].lat2+2.5 ) &&
		( longi >vRegion[ii].long1-2.5 && longi < vRegion[ii].long2+2.5 ) ) {
	    if( abs( val + 999 ) > 0.01f  )
		vRegion[ii].data.push_back(val);
	  }
	  //  cout << " "<<ii <<" "<<vRegion[ii].data.size() <<endl;
	}
	lines ++;
      }
	
      //cout << sdate ;
      for(int ii =0; ii < vRegion.size(); ++ii){
	float tmp = 0.0f;
	for(int kk = 0 ; kk <vRegion[ii].data.size(); ++kk)
	  tmp += vRegion[ii].data[kk];
	if(vRegion[ii].data.size() > 0 )
	  regionannual[ii] += tmp/vRegion[ii].data.size();
	//cout <<"\t"<< tmp/vRegion[ii].data.size();
	//else 
	  //cout <<"\t0";
	//cout <<"\t"<< tmp/vRegion[ii].area;
      }
      moncount ++;
      //cout <<endl;
    }
    cout << i <<"\t"<<moncount<<"\t" ;
    if(moncount > 0) {
      for(int ii =0; ii < regionannual.size(); ++ii)
	cout << regionannual[ii]/moncount<<"\t";
    }
    cout <<endl;
  }
  data_file.close();
  return 0;
}

