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
    
    if(str == "")
      break;

    vector<int> vec;
    cout << "str=" << str << endl;
    stringstream ss(str);
    
    while( getline(ss, str, ',') ) {

      if(str == "") {
        num = -100;
      } else {
        num = atoi(str.c_str());
      }
      cout << num << endl;
    }
    if(str == "") {
      num = -100;
      cout << num << endl;
    }
      
    // Row row(vec.size());
    // _rows.push_back(row);
    
    // for(int j = 0; j < vec.size(); j++) {
    //  _rows[i][j] = vec[j];
      //cout << "_rows " << i << "," << j << " = " << _rows[i][j] << endl;  
    // }
    i++;
  }
 
  cout << "done!" << endl;
  
  return true; // TODO
}

