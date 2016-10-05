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
   int i = 0, size = r.size();
   for( ; i < size; i++ ){
      DBTable::printData(os, r[i]);
      if(i != size - 1) os << ' ';
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells should be left blank (printed as ' ').
   for(int i = 0; i < t.nRows(); i++) { 
      for(int j = 0; j < t.nCols(); j++) {
         if(t[i][j] == INT_MAX)
            os << setw(6) << right;
         else
            os << setw(6) << right << t[i][j];
      }
      os << endl;
   }
   
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{

   string str;
   int num = 0;

   while( getline(ifs, str, '\r') ) {

      if(str == "\n")
         break;
      else if(str == "") { // prevent case: 3^M^M
         // however, problem will occured when "^M at the end"
         
         DBRow row = DBRow();
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
   if(c < size())
      _data.erase(_data.begin() + c); 
   else
      cerr << "error" << endl;
      
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for(size_t i = 0; i < _sortOrder.size(); i++){
      int n = _sortOrder[i];
      if(r1[n] > r2[n]) return false;
      if(r1[n] < r2[n]) return true;
   }
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   vector<DBRow> tmp;
   tmp.swap(_table);
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
   for(size_t i = 0; i < d.size(); i++) {
      
      if(d.size() != nRows()) {
         cerr << "error" << endl;
      }
      _table[i].addData(d[i]);
   }
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   if(!_table[c].empty())
      _table.erase(_table.begin() + c); 
   else
      cerr << "error" << endl;
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
   bool flag = false;
   int max = 0;
   int i = 0, n = nRows();
   for( ; i < n; i++ ){
      int val = _table[i][c];
      if((!flag && val != INT_MAX) || (val > max && val != INT_MAX)){
         max = val;
         flag = true;
      }
   }
   if(flag) return (float)max;
   return NAN;

}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
   bool flag = false;
   int min = 0;
   int i = 0, n = nRows();
   for( ; i < n; i++ ){
      int val = _table[i][c];
      if((!flag && val != INT_MAX) || (val < min && val != INT_MAX)){
         min = val;
         flag = true;
      }
   }
   if(flag) return (float)min;
   return NAN;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   int sum = 0, i = 0, n = nRows();
   bool flag = false;
   for( ; i < n; i++ ){
      int val = _table[i][c];
      if(val != INT_MAX) {
         sum += val;
         flag = true;
      }
   }
   if (flag)   return sum;
   return NAN;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells


   set<int> _set;
   int i = 0, n = nRows();
   for( ; i < n; i++ ){
      
      if(_table[i][c] != INT_MAX){
         _set.insert(_table[i][c]);
      }
   }  
   
   return _set.size();
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
   int sum = 0, count = 0, i = 0, n = nRows();
   for( ; i < n; i++ ){
      int val = _table[i][c];
      if(val != INT_MAX){
         sum += val;
         count ++;
      }
   }
   if (count == 0)   return NAN;
   return 1.0 * sum / count;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
   std::sort(_table.begin(), _table.end(), s);

}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   int i = 0, size = nRows();
   for( ; i < size; i++ ){
      printData(cout, _table[i][c]);
      if(i != size - 1) cout<< ' ';
   }
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

