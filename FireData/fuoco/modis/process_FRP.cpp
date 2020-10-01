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

// icpc -std=c++11 -o process_frp process_FRP.cpp

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

  //string s = "run <dir_name> file_prefix";
  //cout << s <<endl;

  if(argc != 2 ){
       cout<<"Usage: run <dir_name> file_prefix"<<endl;
       return 1;
  }
  cout << argv[1] <<endl;
  ifstream region_file;
  vector<regions> vRegion;
  region_file.open("../../coords.txt", ios::in);
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
  region_file.close();

  sort(vRegion.begin(), vRegion.end());

  cout <<"Read in "<<vRegion.size()<<endl;
  for(int ii =0; ii < vRegion.size(); ++ii) {
    cout <<vRegion[ii].lat1 <<" "<<vRegion[ii].lat2<<" "
	 <<vRegion[ii].long1 <<" "<<vRegion[ii].long2<<" area "<<vRegion[ii].area<< endl;
  }
  
  const int startyear = 2000;
  const int endyear = 2016;
  const int startmonth = 11;
  const int endmonth = 7;

  string pathprefix =  string(argv[1]) + string(".");
  cout <<pathprefix <<endl;

  for(int i = startyear; i <= endyear; i++ ){
    vector<float> regionannual(vRegion.size(), 0.0f);
    int moncount = 0;
    for(int j = 1; j <= 12; j++) {
      if(i == endyear && j > endmonth) break;
      if(i == startyear && j < startmonth) j = startmonth;
      
      string sdate = to_string(i);
      if( j < 10) sdate += string("0");
      sdate += to_string(j) ;
      string filename = pathprefix + sdate + string(".005.01.asc");

      ifstream data_file(filename.c_str(), ios::in);
      if(! data_file.is_open()) {
	cout <<"Can't open "<<filename<<endl;
	continue;
      }
      
      for(int ii =0; ii < vRegion.size(); ++ii) {
	vRegion[ii].data.clear();
      }
      char line[64*1024];
      //read first line (yyyymmdd ...)
      data_file.getline(line, 64*1024);
      
      string sYYYYMMDD, sHHMM, ssat;
      float lati , longi;
      float ft21, ft31, fsample, ffrp, fconf;
      int lineidx = 1;
      float ambit = 0; 
      while (!data_file.eof() ) { 
	data_file >> sYYYYMMDD >> sHHMM >> ssat >> lati >> longi >> ft21 >> ft31 >> fsample >> ffrp >> fconf;
	
	for(int ii =0; ii < vRegion.size(); ++ii) {
	  if(lati > vRegion[ii].lat1-ambit && lati < vRegion[ii].lat2 +ambit && 
	     longi > vRegion[ii].long1 -ambit && longi < vRegion[ii].long2 + ambit) {	    
	    vRegion[ii].data.push_back(ffrp);
	  }
          //cout << " "<<ii <<" "<<vRegion[ii].data.size() <<endl;
	}
		
	lineidx ++;
      }
      //cout <<lineidx <<endl;
      data_file.close();
      
      // finish one file, output
      //cout << sdate ;
      for(int ii =0; ii < vRegion.size(); ++ii){
	float tmp = 0.0f;
	for(int kk = 0 ; kk <vRegion[ii].data.size(); ++kk)
	  tmp += vRegion[ii].data[kk];
	if(vRegion[ii].data.size() > 0 )
	  regionannual[ii] += tmp/vRegion[ii].data.size();
	else 
	  regionannual[ii] += 0.0;
	//cout <<"here " <<ii << "  "<< regionannual[ii]<<endl;
	//cout <<"\t"<< tmp/vRegion[ii].data.size()<<" "<<vRegion[ii].data.size()<<endl ;
	  //else 
	  //cout <<"\t";
	//cout <<"\t"<< tmp/vRegion[ii].area;
	//else 
	// cout <<"\t No_Data";
      }
      //cout <<endl;
      moncount ++;
    }
    cout << i <<"\t"<<moncount<<"\t" ;
    if(moncount > 0) {
      for(int ii =0; ii < regionannual.size(); ++ii)
      cout << regionannual[ii]/moncount<<"\t";
    }
    cout <<endl;

  }

   return 0;
}

