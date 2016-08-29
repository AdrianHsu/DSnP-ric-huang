#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

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
    int getColTotal(){return _rows[0].get_NUM_COL();}

private: 
    vector<Row> _rows; 
}; 

#endif // P2_TABLE_H
