#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

class Row
{
public:
  const int operator[] (size_t i) const { return _data[i]; } // TODO
  int& operator[] (size_t i) { return _data[i]; } // TODO
  Row(int s): NUM_COL(0) { _data = new int[s]; NUM_COL = s;} 

private:
   int  *_data;
   int NUM_COL;
};

class Table 
{ 
public: 
    const Row& operator[] (size_t i) const; 
    Row& operator[] (size_t i); 
    
    bool read(const string&); 

private: 
    vector<Row> _rows; 
}; 

#endif // P2_TABLE_H
