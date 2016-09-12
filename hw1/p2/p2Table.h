#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <cstring>
#include <climits>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

using namespace std;

class Row
{
public:
  const int operator[] (size_t i) const { return _data[i]; } // TODO
  int& operator[] (size_t i) { return _data[i]; } // TODO
  Row(int s): NUM_COL(0) { _data = new int[s]; NUM_COL = s;
  }
  int get_NUM_COL() { return NUM_COL; }

private:
   int  *_data;
   int NUM_COL;
};

class Table 
{ 
public: 
    
    Table(){};
    ~Table(){ 
      if(!_rows.empty())
        _rows.clear(); 
    };

    const Row& operator[] (size_t i) const; 
    Row& operator[] (size_t i); 
    
    bool read(const string&); 
        
    void print();

    int sum(int i);
    void ave(int i);
    void max(int i);
    void min(int i);
    void count(int i);

    void add(int arr[]);
    void exit();
    // the number of columns in every row is the same
    int getColTotal(){
      if (_rows.empty()) return 0;
      else return _rows[0].get_NUM_COL();}

private: 
    vector<Row> _rows; 
}; 

#endif // P2_TABLE_H
