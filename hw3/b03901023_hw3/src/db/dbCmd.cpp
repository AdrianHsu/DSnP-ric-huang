/****************************************************************************
  FileName     [ dbCmd.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <cassert>
#include "util.h"
#include "dbCmd.h"
#include "dbTable.h"

// Global variable
DBTable dbtbl;

// Static functions
static bool checkColIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nCols())) {
      cerr << "Error: Column index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

static bool checkRowIdx(const string& token, int& c)
{
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return false;
   }

   if (!myStr2Int(token, c)) {
      cerr << "Error: " << token << " is not a number!!\n";
      return false;
   }
   if (c < 0 || c >= int(dbtbl.nRows())) {
      cerr << "Error: Row index " << c << " is out of range!!\n";
      return false;
   }
   return true;
}

bool
initDbCmd()
{
   // TODO...
   if ( !( cmdMgr->regCmd("DBAPpend", 4, new DBAppendCmd) && 
            cmdMgr->regCmd("DBAVerage", 4, new DBAveCmd) &&
            cmdMgr->regCmd("DBCount", 3, new DBCountCmd) &&
            cmdMgr->regCmd("DBDelete", 3, new DBDelCmd) &&
            cmdMgr->regCmd("DBMAx", 4, new DBMaxCmd) &&
            cmdMgr->regCmd("DBMIn", 4, new DBMinCmd) &&
            cmdMgr->regCmd("DBPrint", 3, new DBPrintCmd) &&
            cmdMgr->regCmd("DBRead", 3, new DBReadCmd) &&
            cmdMgr->regCmd("DBSOrt", 4, new DBSortCmd) &&
            cmdMgr->regCmd("DBSUm", 4, new DBSumCmd)
         )){
      cerr << "Registering \"init\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

//----------------------------------------------------------------------
//    DBAPpend <-Row | -Column> <(int data)...>
//----------------------------------------------------------------------
CmdExecStatus
DBAppendCmd::exec(const string& option)
{
   // TODO...
   // check option
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return CMD_EXEC_ERROR;
   }

   vector<string> t; //tokens
   if(!CmdExec::lexOptions(option, t))
      return CMD_EXEC_ERROR;
   if(t.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
   string opt = t[0];
   if(myStrNCmp("-Row", opt, 2) == 0) {
      if(t.size() == 1) return CmdExec::errorOption(CMD_OPT_MISSING, t[t.size() - 1]);
      if(t.size() == 2 && t[1] == "-") {
         DBRow row;
         for(size_t i = 0; i < dbtbl.nCols(); i++)
            row.addData(INT_MAX);
         dbtbl.addRow(row);
      } else {
         DBRow row;
         size_t optnum = t.size() - 1;
         size_t nCol = dbtbl.nCols();
         size_t i = 1;
         int num = INT_MAX;
         for( ; i <= optnum; i++ ) {
            
            if(i > nCol) break;
            if(myStr2Int(t[i], num)) {
               row.addData(num);
            } else if(t[i] == "-") {
               row.addData(INT_MAX);
            } else return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[i]);
               
         }
         if(optnum < nCol) {
            for( ; i <= nCol; i++ )
               row.addData(INT_MAX);
         }
         dbtbl.addRow(row);
      }
   } else if(myStrNCmp("-Column", opt, 2) == 0) {
      if(t.size() == 1) return CmdExec::errorOption(CMD_OPT_MISSING, t[t.size() - 1]);
      if(t.size() == 2 && t[1] == "-") {
         vector<int> col;
         for(size_t i = 0; i < dbtbl.nRows(); i++)
            col.push_back(INT_MAX);
         dbtbl.addCol(col);
      } else {
         vector<int> col;
         size_t optnum = t.size() - 1;
         size_t nRow = dbtbl.nRows();
         size_t i = 1;
         int num = INT_MAX;
         for( ; i <= optnum; i++ ) {
            
            if(i > nRow) break;
            if(myStr2Int(t[i], num)) {
               col.push_back(num);
            } else if(t[i] == "-") {
               col.push_back(INT_MAX);
            } else return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[i]);
               
         }
         if(optnum < nRow) {
            for( ; i <= nRow; i++ )
               col.push_back(INT_MAX);
         }
         dbtbl.addCol(col);
      }
   } else return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);

   return CMD_EXEC_DONE;
}

void
DBAppendCmd::usage(ostream& os) const
{
   os << "Usage: DBAPpend <-Row | -Column> <(int data)...>" << endl;
}

void
DBAppendCmd::help() const
{
   cout << setw(15) << left << "DBAPpend: "
        << "append a row or column of data to the table" << endl;
}


//----------------------------------------------------------------------
//    DBAVerage <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBAveCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float a = dbtbl.getAve(c);
   ios_base::fmtflags origFlags = cout.flags();
   cout << "The average of column " << c << " is " << fixed
        << setprecision(2) << a << ".\n";
   cout.flags(origFlags);

   return CMD_EXEC_DONE;
}

void
DBAveCmd::usage(ostream& os) const
{     
   os << "Usage: DBAVerage <(int colIdx)>" << endl;
}

void
DBAveCmd::help() const
{
   cout << setw(15) << left << "DBAverage: "
        << "compute the average of a column" << endl;
}


//----------------------------------------------------------------------
//    DBCount <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBCountCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   int n = dbtbl.getCount(c);
   cout << "The distinct count of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBCountCmd::usage(ostream& os) const
{     
   os << "Usage: DBCount <(int colIdx)>" << endl;
}

void
DBCountCmd::help() const
{
   cout << setw(15) << left << "DBCount: "
        << "report the distinct count of data in a column" << endl;
}


//----------------------------------------------------------------------
//    DBDelete <-Row | -Column> <(int index)>
//----------------------------------------------------------------------
CmdExecStatus
DBDelCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options, 2))
      return CMD_EXEC_ERROR;

   bool doRow = false;
   if (myStrNCmp("-Row", options[0], 2) == 0) doRow = true;
   else if (myStrNCmp("-Column", options[0], 2) != 0)
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);

   int c;
   if (doRow) {
      if (!checkRowIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delRow(c);
   }
   else {
      if (!checkColIdx(options[1], c)) return CMD_EXEC_ERROR;
      dbtbl.delCol(c);
   }

   return CMD_EXEC_DONE;
}

void
DBDelCmd::usage(ostream& os) const
{
   os << "Usage: DBDelete <-Row | -Column> <(int index)>" << endl;
}

void
DBDelCmd::help() const
{
   cout << setw(15) << left << "DBDelete: "
        << "delete a row or column from the table" << endl;
}


//----------------------------------------------------------------------
//    DBMAx <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMaxCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMax(c);
   cout << "The max data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMaxCmd::usage(ostream& os) const
{     
   os << "Usage: DBMAx <(int colIdx)>" << endl;
}

void
DBMaxCmd::help() const
{
   cout << setw(15) << left << "DBMAx: "
        << "report the maximum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBMIn <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBMinCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getMin(c);
   cout << "The min data of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBMinCmd::usage(ostream& os) const
{     
   os << "Usage: DBMIn <(int colIdx)>" << endl;
}

void
DBMinCmd::help() const
{
   cout << setw(15) << left << "DBMIn: "
        << "report the minimum number of a column" << endl;
}


//----------------------------------------------------------------------
//    DBPrint < (int rowIdx) (int colIdx)
//            | -Row (int rowIdx) | -Column (colIdx) | -Table | -Summary>
//----------------------------------------------------------------------
   CmdExecStatus
DBPrintCmd::exec(const string& option)
{  
   // TODO...
   if (!dbtbl) {
      cerr << "Error: Table is not yet created!!" << endl;
      return CMD_EXEC_ERROR;
   }

   vector<string> t; //tokens
   if(!CmdExec::lexOptions(option, t))
      return CMD_EXEC_ERROR;

   if(t.size() > 2) 
      return CmdExec::errorOption(CMD_OPT_EXTRA, t[t.size() - 1]);
   else if(t.size() == 0)
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   
   string opt = t[0];
   int index = -1;
   int index2 = -1;
   if(myStrNCmp("-Table", opt, 2) == 0) {
      if(t.size() == 2) return CmdExec::errorOption(CMD_OPT_EXTRA, t[t.size() - 1]);
      else cout << dbtbl;

   } else if(myStrNCmp("-Summary", opt, 2) == 0) {
      if(t.size() == 2) return CmdExec::errorOption(CMD_OPT_EXTRA, t[t.size() - 1]);
      else dbtbl.printSummary();

   } else if(myStrNCmp("-Row", opt, 2) == 0) {
      if(t.size() == 1) return CmdExec::errorOption(CMD_OPT_MISSING, t[t.size() - 1]);

      if (checkRowIdx(t[1], index))   cout << dbtbl[index];
      else  return CMD_EXEC_ERROR;
   } else if(myStrNCmp("-Column", opt, 2) == 0) {
      if(t.size() == 1) return CmdExec::errorOption(CMD_OPT_MISSING, t[t.size() - 1]);

      if (checkColIdx(t[1], index)) dbtbl.printCol(index);
      else  return CMD_EXEC_ERROR;
   } else {

      if (!myStr2Int(t[0], index))   return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
      if(checkRowIdx(t[0], index)) {

         if (t.size() == 1) return CmdExec::errorOption(CMD_OPT_MISSING, t[0]);
         if (!myStr2Int(t[1], index2))   return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
         if (checkColIdx(t[1], index2)){
            int data = dbtbl.getData(index, index2);
            if(data == INT_MAX)  cout << "null\n";
            else  cout << data << endl;

         }  else return CMD_EXEC_ERROR;
      } else return CMD_EXEC_ERROR;
   }
   return CMD_EXEC_DONE;
}
void
DBPrintCmd::usage(ostream& os) const
{
   os << "DBPrint < (int rowIdx) (int colIdx)\n"
      << "        | -Row (int rowIdx) | -Column (colIdx) | -Table>" << endl;
}

void
DBPrintCmd::help() const
{
   cout << setw(15) << left << "DBPrint: "
        << "print the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBRead <(string csvFile)> [-Replace]
//----------------------------------------------------------------------
CmdExecStatus
DBReadCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool doReplace = false;
   string fileName;
   for (size_t i = 0, n = options.size(); i < n; ++i) {
      if (myStrNCmp("-Replace", options[i], 2) == 0) {
         if (doReplace) return CmdExec::errorOption(CMD_OPT_EXTRA,options[i]);
         doReplace = true;
      }
      else {
         if (fileName.size())
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
         fileName = options[i];
      }
   }

   ifstream ifs(fileName.c_str());
   if (!ifs) {
      cerr << "Error: \"" << fileName << "\" does not exist!!" << endl;
      return CMD_EXEC_ERROR;
   }

   if (dbtbl) {
      if (!doReplace) {
         cerr << "Error: Table exists. Use \"-Replace\" option for "
              << "replacement.\n";
         return CMD_EXEC_ERROR;
      }
      cout << "Table is replaced..." << endl;
      dbtbl.reset();
   }
   if (!(ifs >> dbtbl)) return CMD_EXEC_ERROR;
   cout << "\"" << fileName << "\" was read in successfully." << endl;

   return CMD_EXEC_DONE;
}

void
DBReadCmd::usage(ostream& os) const
{
   os << "Usage: DBRead <(string csvFile)> [-Replace]" << endl;
}

void
DBReadCmd::help() const
{
   cout << setw(15) << left << "DBRead: "
        << "read data from .csv file" << endl;
}


//----------------------------------------------------------------------
//    DBSOrt <(int colIdx)>...
//----------------------------------------------------------------------
CmdExecStatus
DBSortCmd::exec(const string& option)
{
   // check option
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   
   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   DBSort sortOrders;
   for (int i = 0, n = options.size(); i < n; ++i) {
      int s;
      if (!checkColIdx(options[i], s)) return CMD_EXEC_ERROR;
      sortOrders.pushOrder(s);
   }
   dbtbl.sort(sortOrders);

   return CMD_EXEC_DONE;
}

void
DBSortCmd::usage(ostream& os) const
{
   os << "Usage: DBSOrt <(int colIdx)>..." << endl;
}

void
DBSortCmd::help() const
{
   cout << setw(15) << left << "DBSort: "
        << "sort the data in the table" << endl;
}


//----------------------------------------------------------------------
//    DBSUm <(int colIdx)>
//----------------------------------------------------------------------
CmdExecStatus
DBSumCmd::exec(const string& option)
{  
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token, false))
      return CMD_EXEC_ERROR;
   int c;
   if (!checkColIdx(option, c)) return CMD_EXEC_ERROR;

   float n = dbtbl.getSum(c);
   cout << "The sum of column " << c << " is " << n << "." << endl;

   return CMD_EXEC_DONE;
}

void
DBSumCmd::usage(ostream& os) const
{     
   os << "Usage: DBSUm <(int colIdx)>" << endl;
}

void
DBSumCmd::help() const
{
   cout << setw(15) << left << "DBSUm: "
        << "compute the summation of a column" << endl;
}

