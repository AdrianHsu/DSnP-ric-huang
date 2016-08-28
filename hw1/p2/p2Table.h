#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

#include <fstream>

using namespace std;

class Row
{
public:
   const int operator[] (size_t i) const { return _data[i]; } // TODO
   int& operator[] (size_t i) { return _data[i]; } // TODO

private:
   int  *_data;
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
