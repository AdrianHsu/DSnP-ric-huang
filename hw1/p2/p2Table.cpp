#include "p2Table.h"

using namespace std;

// Implement member functions of class Row and Table here

bool
Table::read(const string& csvFile)
{
  fstream fs;
  fs.open(csvFile, ios::in);
  
  if(!fs) {
   return false; 
  }

  int i = 0;
  
  string str;
  int num = 0;

  while( getline(fs, str, '\r') ) {
  
    if(str == "\n")
      break;
    else if(str == "") { // prevent case: 3^M^M
      // however, problem will occured when "^M at the end"
      Row row(0);
      _rows.push_back(row);
      i++;
      continue;
      
    }
    // you don’t need to handle syntax errors in reading .csv file 
    // (e.g. missing ^M at the end) or parsing commands.

    vector<int> vec;
    stringstream ss(str);
    
    while( getline(ss, str, ',') ) {

      if(str == "") { //prevent case: ^M,3^M
        num = -100;
      } else {
        num = atoi(str.c_str());
      }
      vec.push_back(num);
    }
    // prevent case: 3,^M ...etc
    if(str == "") {
      num = -100;
      vec.push_back(num);
    }
      
    Row row(vec.size());
    _rows.push_back(row);
    
    for(int j = 0; j < vec.size(); j++) {
      _rows[i][j] = vec[j];
      cout << "_rows " << i << "," << j << " = " << _rows[i][j] << endl;  
    }
    i++;
  }

  _rows.pop_back(); // solve "^M at the end", 
  // since "missing ^M at the end" special case will not happened
  
  // cout << _rows.size() << endl;
  
  return true; // TODO
}

