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
      Row row(1);
      row[0] = -100;
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
      // cout << "_rows " << i << "," << j << " = " << _rows[i][j] << endl;  
    }
    i++;
  }

  _rows.pop_back(); // solve "^M at the end", 
  // since "missing ^M at the end" special case will not happened
  
  // cout << _rows.size() << endl;
  
  return true; // TODO
}

void
Table::print()
{
  for(int i = 0; i < _rows.size(); i++) {
  
    for(int j = 0; j < getColTotal(); j++) {

      if(_rows[i][j] != -100)
        cout << setw(4) << right << _rows[i][j];
      else
        cout << "    ";
    }
    cout << endl;
  }
}
int
Table::sum(int j)
{
  int sum = 0;
  for(int i = 0; i < _rows.size(); i++) {
 
    if(getColTotal() > j) {
      
      if(_rows[i][j] != -100)
        sum += _rows[i][j];
    }
  }
  
  return sum;
}
void
Table::ave(int j)
{
  //double num = (double)_rows.size();
  double num = 0;
  for(int i = 0; i < _rows.size(); i++) {
    if(_rows[i][j] != -100)
      num++;
  }
  
  double sum = (double)Table::sum(j);
  
  cout << "The average of data in column #" << j <<" is " << fixed << setprecision(1) << (sum/num)  << "." << endl;

}
void
Table::max(int j)
{

  int max = -99;
  for(int i = 0; i < _rows.size(); i++) {
  
    if(getColTotal() > j) {
    
      if(_rows[i][j] > max && _rows[i][j] != -100)
        max = _rows[i][j];
    }
  }

  cout << "The maximum of data in column #" << j << " is " << max << "." << endl;
}

void
Table::min(int j)
{

  int min = 100;
  for(int i = 0; i < _rows.size(); i++) {
  
    if(getColTotal() > j) {
    
      if(_rows[i][j] < min && _rows[i][j] != -100)
        min = _rows[i][j];
    }
  }

  cout << "The minimum of data in column #" << j << " is " << min << "." << endl;
}
void
Table::count(int j)
{
  int count = 0;
  int num[200] = {0};// -99 to 100
  // if num = -99, -99 - (-99) = 0
  // if num = 100, 100 - (-99) = 199

  for(int i = 0; i < _rows.size(); i++) {
  
    if(getColTotal() > j) {
    
      int val = _rows[i][j];
      if(val == -100)
        continue;
      if(num[val + 99]) {
        // do nothing
      } else {
        num[val + 99] = 1;
        count++;
      }
    }
  }
  cout << "The distinct count of data in column #" << j << " is " << count << "." << endl;
}
void
Table::add(int arr[])
{
  
  Row row( getColTotal() );
  for(int i = 0; i < getColTotal(); i++)
    row[i] = arr[i];

  _rows.push_back(row);
}
