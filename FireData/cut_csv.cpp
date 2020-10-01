


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
  // process a csv file to output line with from coln1 to coln2

  if(argc != 5 ){
       cout<<"Usage: run <file_name> row coln1 coln2"<<endl;
       return 1;
  }
  
  ifstream csv_file(argv[1], ios::in) ;
  int row = atoi(argv[2]);
  int col1 = atoi(argv[3]);
  int col2 = atoi(argv[4]);
  int idx = 0;
  char line[64*1024];
  while(idx < row ){
    csv_file.getline(line, 64*1024);
    idx++;
  }
  csv_file.getline(line, 64*1024);
  string str_line = string(line);
      
  vector<string> tokens;
  tokenize(str_line, tokens, ","); 
  
  for(int j = col1; j  < col2 ; ++j){
    cout << tokens[j]<<",";
  }
  cout <<endl;
  csv_file.close();
   return 0;
}

