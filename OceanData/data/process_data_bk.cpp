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

// g++ -std=c++11 -o process_data process_data.cpp
// This is to process .nc files.
// 1.30.2019 Fix the index calculation

struct regions {
  string name;
  float lat1, lat2;
  float lon1, lon2;
  //area = pi*R^2* | sin(lat1) - sin(lat2) | *| (long1 - long2)| / 180
  // lat and long in degree
  float area;
  
  vector<int> dataCnt;
  vector < float> data;
  vector < float> dataMax;
  vector < float> dataMin;
};

bool operator< (const regions& r1, const regions& r2) { 
  if(r1.lat1 == r2.lat1) {
    if(r1.lat2 == r2.lat2)
      return (r1.lon1 < r2.lon2);
    else
      return (r1.lat1 < r2.lat2);
  }
  else
    return (r1.lat1 < r2.lat2);
}
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

void tokenize(string const &input, vector<string> &tokens, 
              string const &delim) 
{
    size_t start = 0;
    size_t end   = 0;

    while (end != string::npos) {
      end = input.find_first_of(delim, start);
      
      // If at end use length=remainder, else length=end-start.
      const size_t length = 
        end == string::npos ? input.length() - start : end - start;
      
      if (length > 0) {
	string ss = string(input.data() + start, length);
	tokens.push_back( trim(ss) );
      }
      start = (end == string::npos ? string::npos : end + 1);
    }            
}


int main (int argc, char *argv[])
{


  if(argc != 2 ){
       cout<<"Usage: process_data <dir_name> "<<endl;
       return 1;
  }
  
  ifstream region_file;
  vector<regions> vRegion;
  region_file.open("coord.txt", ios::in);
  while (!region_file.eof() ) {
    string s;
    float i1=0, i2=0, i3=0, i4=0;
    region_file>>s>>i1 >> i2 >> i3 >> i4;
    if( i1 ==0 && i2 ==0 ) break;
    regions r1;
    r1.name = s;
    r1.lat1 = i1;
    r1.lat2 = i2;
    r1.lon1 = i3;
    r1.lon2 = i4;
    
    //area = pi*R^2* | sin(lat1) - sin(lat2) | *| (long1 - long2)| / 180
    r1.area = 3.1415926f * 6371.0f*6371.0f * abs(sin( (float)r1.lat1/180.0f*3.1415926f) - sin((float)r1.lat2/180.0f*3.1415926f ) ) * abs(r1.lon1 - r1.lon2 ) / 180.0f;
    vRegion.push_back(r1);
  }
  region_file.close();

  //  sort(vRegion.begin(), vRegion.end());

  cout <<"Read in "<<vRegion.size()<<" regions."<<endl;
  for(int ii =0; ii < vRegion.size(); ++ii) {
    cout <<vRegion[ii].name << " : "<<vRegion[ii].lat1 <<" "<<vRegion[ii].lat2<<" "
 <<vRegion[ii].lon1 <<" "<<vRegion[ii].lon2<<" area "<<vRegion[ii].area<< endl;
  }

  string pathprefix = string(argv[1]) + string("/urllist.txt");
  ifstream url_file;
  vector<string> filenames;
  url_file.open(pathprefix, ios::in);
  while (!url_file.eof() ) {
    string s;
    url_file>> s;
   
    std::size_t i1 = s.find("getfile/");
    if(i1 == string::npos) continue;
    
    string s1 = s.substr(i1+8, s.size()-i1-8);
    filenames.push_back(s1);
    // cout << s1<<endl;
  }
  cout <<"Total "<< filenames.size()<<" files."<< endl;
  url_file.close();

  string var = "chl_ocx";
  if(filenames[0].find("nflh") != string::npos ) var = "nflh";

  double totaldatacnt = 0;
  double totalInvaliddatacnt = 0;
  for(int i = 0; i < filenames.size(); i++ ){
    
    string filename = string(argv[1]) +string("/") + filenames[i];

    int ret = system("rm -f t.out");
    
    string cmd = string("../netcdf-c-4.6.2/ncdump/ncdump -t -v ") + var + string(",lat,lon ") + filename +string(" > t.out"); 
    ret = system (cmd.c_str());

    ifstream data_file("t.out", ios::in);
    //ofstream dataout_file("data.out", ios::out);
    if(! data_file.is_open()) {
      cout <<"Can't open t.out "<<endl;
      return 1 ;
    }
      
    for(int ii =0; ii < vRegion.size(); ++ii) {
      vRegion[ii].data.clear();
      vRegion[ii].dataMax.clear();
      vRegion[ii].dataMin.clear();
      vRegion[ii].dataCnt.clear();
    }
          
    char line[64*1024];
    vector<string> datas;
    
    // lat/lon count and min max are the same, already used .csh to checked all files.
    // see individual .info file 

    // so I ignore the lat, lon grid and check the indices of data.
    // expect to get data of size latCnt*lonCnt; they are in .info file.
    // Aqual_Chl , chl_ocx(lat, lon), lat = 4320 ; lon = 8640 ;
    // Aqua_nflh,  nflh(lat, lon)  lat = 4320 ; lon = 8640 ;
    // Terra_Chl, chl_ocx(lat, lon) lat = 4320 ; lon = 8640 ;
    // Terra_nflh, nflh(lat, lon) lat = 4320 ; lon = 8640 ;
    // 4320*8640 = 37,324,800

    datas.reserve(37400000);
    int linecnt = 0;
    while (!data_file.eof() ) {
      data_file.getline(line, 1024);
      string str_line = string(line);
      if(str_line.find("data:") == 0){ // found at first position

	data_file.getline(line, 1024);
	//dataout_file << line;
	data_file.getline(line, 1024);
	//dataout_file << line;
	// start to read in data, stop when no , is read
	while (!data_file.eof() ) {
	  data_file.getline(line, 1024);
	  linecnt ++;
	  str_line = string(line);
	  str_line = trim(str_line);
	  if(str_line.size() <=0 || str_line.find(",") == string::npos ) break;
	  //dataout_file << str_line<<endl;
	  vector<string> tokes;
	  tokenize(str_line, tokes, ",");
	  std::copy (tokes.begin(), tokes.end(), std::back_inserter(datas));

	}
	if(datas.size() > 0) break;
      }
    }
    //dataout_file.close();
    cout <<"Read in data " << datas.size()<<" in "<< linecnt<<" lines."<<endl;

    // this is to test lat and lon region index if reading from file
    vector<float> latsf;
    vector<float> lonsf;
    while (!data_file.eof() ) {
      data_file.getline(line, 1024);
      string str_line = string(line);
      if(str_line.find("lat =") == 1){
	str_line = string(line);
	str_line = trim(str_line);
	str_line = str_line.substr(6, str_line.size()-6);
	vector<string> tokes;
	tokenize(str_line, tokes, ",");
	//cout <<"tokes size "<< tokes.size();
	for(int z =0; z < tokes.size(); z++ ){
	  latsf.push_back(stof(tokes[z]));
	}
	while (!data_file.eof() ) {
	  data_file.getline(line, 1024);
	  str_line = string(line);
	  str_line = trim(str_line);
	  if(str_line.size() <=0 || str_line.find(",") == string::npos ) break;
	  tokes.clear();
	  tokenize(str_line, tokes, ",");
	  for(int z =0; z < tokes.size(); z++ ){
	    latsf.push_back(stof(tokes[z]));
	  }
	}
	// the trailing number -89.97918 ;
	if(str_line.size() > 3 ){
	  tokes.clear();
	  tokenize(str_line, tokes, ";");	  
	  latsf.push_back(stof(tokes[0]));
	}
	if(latsf.size() > 0) break;
      }
    }

    while (!data_file.eof() ) {
      data_file.getline(line, 1024);
      string str_line = string(line);
      if(str_line.find("lon =") == 1){
	str_line = string(line);
	str_line = trim(str_line);
	str_line = str_line.substr(6, str_line.size()-6);
	vector<string> tokes;
	tokenize(str_line, tokes, ",");
	for(int z =0; z < tokes.size(); z++ ){
	  lonsf.push_back(stof(tokes[z]));
	}
	while (!data_file.eof() ) {
	  data_file.getline(line, 1024);
	  str_line = string(line);
	  str_line = trim(str_line);
	  if(str_line.size() <=0 || str_line.find(",") == string::npos ) break;
	  tokes.clear();
	  tokenize(str_line, tokes, ",");
	  for(int z =0; z < tokes.size(); z++ ){
	    lonsf.push_back(stof(tokes[z]));
	  }
	}
	if(lonsf.size() > 0) break;
      }
    }

    cout <<"Read in lat " << latsf.size()<<" lon "<< lonsf.size()<<endl;
    cout <<"Read in lat 0 is "<< latsf[0]<<endl;
    cout <<"Read in lon 0 is "<< lonsf[0]<<endl;
    
    for(int ii =0; ii < vRegion.size(); ++ii) {
      float lat1 = vRegion[ii].lat1;
      float lat2 = vRegion[ii].lat2;
      float lon1 = vRegion[ii].lon1;
      float lon2 = vRegion[ii].lon2;

      int latinx1 = -1;
      int latinx2 = -1;
      int loninx1 = -1;
      int loninx2 = -1;
      for(int x = 0; x < latsf.size(); x++){
	if(latinx2 == -1 && latsf[x] < lat2) latinx2 = x;
	if(latinx1 == -1 && latsf[x] < lat1) latinx1 = x;
	if(latinx2 != -1 && latinx1 != -1) break;
      }

      if(lon1 < lon2){
	for(int x = 0; x < lonsf.size(); x++){
	  if(loninx1 == -1 && lonsf[x] > lon1) loninx1 = x;
	  if(loninx2 == -1 && lonsf[x] > lon2) loninx2 = x;
	  if(loninx1 != -1 && loninx2 != -1) break;
	}
      } else {
	for(int x = 0; x < lonsf.size(); x++){
	  if(loninx2 == -1 && lonsf[x] > lon2) loninx2 = x;
	  if(loninx1 == -1 && lonsf[x] > lon1) loninx1 = x;
	  if(loninx1 != -1 && loninx2 != -1) break;
	}

      }

      cout <<"Found lat indices "<< latinx1<<":"<<latinx2<<" lon "<< loninx1<<":"<<loninx2<<endl;
    }
    data_file.close();
    
    int lat_p = 4320/180; //24
    int lon_p = 8640/360; //24
    for(int ii =0; ii < vRegion.size(); ++ii) {
      float lat1 = vRegion[ii].lat1;
      float lat2 = vRegion[ii].lat2;
      float lon1 = vRegion[ii].lon1;
      float lon2 = vRegion[ii].lon2;

      if(lat1 >= lat2) {
	cout << "Lat 1 can't be greater than lat2 ."<< lat1 <<" "<< lat2<<endl;
	return 1;
      }
      
      int validdatacnt = 0;
      int invalidCnt = 0;
      double val = 0.0;
      int sidx = 0;
      float valMax = -999999; //FLT_MIN;
      float valMin =  999999; //FLT_MAX;

      // count the case that lon1 < lon2, assuming all lat1 < lat2
      // lat goes from 90 to -90, lon goes from -180 to 180
      cout <<"Calculated range lat " << (90-lat2)*lat_p<<":"<<(90-lat1)*lat_p<<" lon "<<(lon1+180)*lon_p<<":"<<(lon2+180)*lon_p<<endl;
      if(lon1 < lon2 ){	
	for (int j = (90-lat2)*lat_p; j < (90-lat1)*lat_p; j++){
	  for(int k = (lon1+180)*lon_p; k < (lon2+180)*lon_p; k++){
	    sidx = j *lon_p*360 + k ;
	    if(datas[sidx].length() <2 && datas[sidx].compare("_") ==0 ) invalidCnt ++;
	    else {
	      double d = stod(datas[sidx]);
	      val += d;
	      if(d > valMax) valMax = d; 
	      if(d < valMin) valMin = d;
	      //cout <<sidx<<" "<< datas[sidx] <<" "<< val<<endl;
	      validdatacnt ++;
	    }
	  }
	}
      } else { // when lon1 > lon2
	for (int j = (90-lat2)*lat_p; j < (90-lat1)*lat_p; j++){
    	  for(int k = 0; k < (lon2+180)*lon_p; k++){
	    sidx = j *lon_p*360 + k ;
	    if(datas[sidx].length() <2 && datas[sidx].compare("_") ==0 ) invalidCnt ++;
	    else {
	      double d = stod(datas[sidx]);
	      val += d;
	      if(d > valMax) valMax = d; 
	      if(d < valMin) valMin = d;
	      //cout <<sidx<<" "<< datas[sidx] <<" "<< val<<endl;
	      validdatacnt ++;
	    }
	  }

	  for(int k = (lon1+180)*lon_p; k < 360*lon_p; k++){
	    sidx = j *lon_p*360 + k ;
	    if(datas[sidx].length() <2 && datas[sidx].compare("_") ==0 ) invalidCnt ++;
	    else {
	      double d = stod(datas[sidx]);
	      val += d;
	      if(d > valMax) valMax = d; 
	      if(d < valMin) valMin = d;
	      //cout <<sidx<<" "<< datas[sidx] <<" "<< val<<endl;
	      validdatacnt ++;
	    }
	  }
	} 
      }  // else

      vRegion[ii].dataCnt.push_back(validdatacnt);
      vRegion[ii].data.push_back(val/validdatacnt);
      vRegion[ii].dataMax.push_back(valMax);
      vRegion[ii].dataMin.push_back(valMin);
      cout << "Region "<<ii<< " : " << val/validdatacnt<<" max: "<< valMax<<" min: "<<valMin<<" valid data count: "<< validdatacnt<<" invalid count : "<< invalidCnt << endl;
      totaldatacnt += validdatacnt;
      totalInvaliddatacnt += invalidCnt; 
    } // for region
    
  }

  cout <<"Total valid data points "<< totaldatacnt<<endl;
  cout <<"Total invalid data points "<< totalInvaliddatacnt<<endl;
   return 0;
}

