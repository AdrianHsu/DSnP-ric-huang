/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
   

   string str;
   int num = 0;

   while( getline(ifs, str, '\r') ) {

      if(str == "\n")
         break;
      else if(str == "") { // prevent case: 3^M^M
         // however, problem will occured when "^M at the end"
         
         DBRow row;
         row.addData(INT_MAX);
         t.addRow(row);
         
         continue;

      }
      // you don’t need to handle syntax errors in reading .csv file 
      // (e.g. missing ^M at the end) or parsing commands.

      vector<int> vec;
      stringstream ss(str);

      while( getline(ss, str, ',') ) {

         if(str == "") { //prevent case: ^M,3^M
            num = INT_MAX;
         } else {
            num = atoi(str.c_str());
         }
         vec.push_back(num);
      }
      // prevent case: 3,^M ...etc
      if(str == "") {
         num = INT_MAX;
         vec.push_back(num);
      }

      DBRow row(vec);
      t.addRow(row);
   }
   
   if(t.nRows() != 0)
      t.delRow(t.nRows() - 1); // solve "^M at the end", 
   // since "missing ^M at the end" special case will not happened

   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin() + c); 
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
   return 0.0;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   return 0.0;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   return 0.0;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
   return 0;
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   return 0.0;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   size_t size = _table[c].size();
   size_t i = 0;
   for(i = 0; i < size; ++i) {
      printData(cout, _table[c][i]);
      cout << " ";
   }
   printData(cout, _table[c][i]);
   cout << endl;
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

