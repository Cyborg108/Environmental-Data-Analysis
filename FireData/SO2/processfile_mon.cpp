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
//struct dirent {
//    ino_t d_ino; /* inode number */
//    off_t d_off; /* offset to the next dirent */
//    unsigned short d_reclen; /* length of this record */
//    unsigned char d_type; /* type of file */
//    char d_name[256]; /* filename */
//};
 
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

struct so2data {
  int iyear;
  int imon;
  vector<float> SO2_PBL;
  vector<float> SO2_TRL;
  vector<float> SO2_TRM;
};

//convert day to month
int daytomonth(int dayn) {
  int monthDay[12]= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int day = dayn;
  for(int i =1 ; i <=12; i++){
    day = day - monthDay[i-1];
    if(day < 0) 
      return i;
  }
  return 12;
    
}

void openfile(const char *file ) {

  ifstream data_file(file, ios::in);
  if(! data_file.is_open()) {
    cout <<"Can't open "<<file<<endl;
    return;
  }
  string namecity ; 
  string namecountry;

  data_file >> namecity;
  data_file >> namecountry;
  string longitude;
  string latitude;
  
  string temp;
  while ( !data_file.eof()) {
    data_file >> temp;
    if(temp.compare("Latitude:") == 0) {
	data_file >>latitude ;
    }
    if(temp.compare("Longitude:") == 0 ) {
	data_file >>longitude ;
    }
      
    if(longitude.length() > 0 && latitude.length() > 0)
      break;
  }
  string ofilename = "output_"+namecity.substr(0,namecity.size()-2) +"_"+namecountry+".txt";;
  
  ofstream out_file(ofilename, ios::out);
  out_file << file <<"\t"<<namecity<<"\t"<<namecountry <<"\t"<<latitude  <<"\t"<< longitude <<endl;
  
  temp = "";
  float minlat= 2000;
  float maxlat= -2000;
  float minlong= 2000;
  float maxlong= -2000;
  
  vector<so2data*> vso2;
  int curyr = -1;
  int curmon = -1;
  while ( !data_file.eof()) {
    std::getline(data_file,temp);
    if(temp.find("Datetime       MJD") !=std::string::npos)
      break;
  }

  while ( !data_file.eof()) {
    std::getline(data_file,temp);
    vector<string> tokens;
    tokenize(temp, tokens, " "); 
    if(tokens.size() < 12) break;
    //cout <<temp<<endl;
    //Datetime       MJD2000  Year  DOY  sec. (UT)   Orbit   CTP    Lat.    Lon.  Dist.     SZA    SO2_PBL    SO2_TRL    SO2_TRM    SO2_STL   OMTO3_O3    Cld. F.  OMTO3_Ter_Pre  OMTO3_Ref   OMTO3_AI
    int year = stoi(tokens[2]);
    int mon = daytomonth(stoi(tokens[3]));
    if(year != curyr || mon != curmon ){
      curyr = year;
      curmon = mon;
      so2data* sd = new so2data();
      sd->iyear = year;
      sd->imon = mon;
      vso2.push_back(sd);
    }
    //cout <<year <<" "<< mon <<" "<< vso2.size()<<endl;
    so2data* sd2 =vso2[ vso2.size()-1];
    float t = stof(tokens[11]);
    if(t > 100 || t < -100 ) t = 0;
    sd2->SO2_PBL.push_back(t);
    t = stof(tokens[12]);
    if(t > 100 || t < -100 ) t = 0;
    sd2->SO2_TRL.push_back(t);
    t = stof(tokens[13]);
    if(t > 100 || t < -100 ) t = 0;
    sd2->SO2_TRM.push_back(t);
    float lat = stof(tokens[7]);
    float lon = stof(tokens[8]);
    minlat = std::min(minlat, lat);
    minlong = std::min(minlong, lon);
    maxlat = std::max(maxlat, lat);
    maxlong = std::max(maxlong, lon);
  }

  data_file.close();
  out_file <<vso2.size()  <<"\t min/max lat"<<minlat<<"\t"<<maxlat <<"\tmin/max long "<<minlong <<"\t"<< maxlong <<endl;
  out_file <<"year_mon \t SO2_PBL \t SO2_TRL \t SO2_TRM\n";
  for(int i = 0 ; i < vso2.size(); i ++) {
    out_file <<vso2[i]->iyear<<"_"<<vso2[i]->imon<<" \t";
    float tot = 0.0f;
    for(int j =0 ; j < vso2[i]->SO2_PBL.size() ; j++) {
      tot += vso2[i]->SO2_PBL[j];
    }
    out_file <<tot/vso2[i]->SO2_PBL.size()<<" \t";
    tot = 0.0f;
    for(int j =0 ; j < vso2[i]->SO2_TRL.size() ; j++) {
      tot += vso2[i]->SO2_TRL[j];
    }
    out_file <<tot/vso2[i]->SO2_TRL.size()<<" \t";
    tot = 0.0f;
    for(int j =0 ; j < vso2[i]->SO2_TRM.size() ; j++) {
      tot += vso2[i]->SO2_TRM[j];
    }
    out_file <<tot/vso2[i]->SO2_TRM.size()<<" \t\n";
    
  }

  for(int i = 0 ; i < vso2.size(); i ++){
    delete vso2[i];
  }
  vso2.clear();
  out_file.close();
}

static void list_dir(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
      //printf("%s\n",entry->d_name);
      if(strlen(entry->d_name) > 10) 
	openfile(entry->d_name);      
    }

    closedir(dir);
}


int  main () {

  //list_dir("./");

  // read in file names that we want to process
  vector <string> filenames ;
  ifstream filens("filenames.txt", ios::in);
  if(! filens.is_open()) {
    cout <<"Can't open "<<"filenames.txt"<<endl;
    return 1;
  }
  string fname; 

  while(!filens.eof()){
    filens >> fname;
    filenames.push_back(fname);
    openfile(fname.c_str());
  }
  filens.close();

  return 0;
}
