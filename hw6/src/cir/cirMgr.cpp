/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CirMgr::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         //errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         //errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool 
CirMgr::aigerAddAnd(string& str) {
   
   int argv[3] = {0, 0, 0}; // lhs, rhs0, rhs1
   vector<string> tmp;
   if(!lexOptions(str, tmp, 3))
      return false;
   for(int i = 0; i < 3; i++) {
      if(!myStr2Int(tmp[i], argv[i]))
         return false;
   }
   
   assert(argv[0] > 1);
   assert(!aiger_sign(argv[0]));

   CirAigGate* aig = new CirAigGate(argv[0], argv[1], argv[2]);
   gateList[ aiger_lit2var(argv[0]) ] = aig;
   return true;
}
bool
CirMgr::readCircuit(const string& fileName)
{
   if(!flag)
      flag = 1; //netlist should be constructed before any operations

   ifstream ifs(fileName);
   if(!ifs.is_open())
      return false;
   
   string str;
   // without error handling
   if(getline(ifs, str, '\n')) {
      vector<string> tmp;
      if(!lexOptions(str, tmp, 6))
         return false;
      for(int i = 1; i <= 5; i++)
         myStr2Int(tmp[i], miloa[i - 1]);
   }
   gateList.clear();
   gateList.resize(miloa[0] + miloa[3] + 1, 0);
   gateList[0] = new CirConstGate();


   vector<int> ins;
   for(int i = 0; i < miloa[1]; i++) {
      
      if(!getline(ifs, str, '\n'))
         return false;
      int lit;
      if(!myStr2Int(str, lit))
         return false;
      ins.push_back(aiger_lit2var(lit));
      gateList[ aiger_lit2var(lit) ] = new CirPiGate((unsigned)lit);
   }
   vector<int> outs;
   for(int i = 0; i < miloa[3]; i++) {
      
      if(!getline(ifs, str, '\n'))
         return false;
      int lit;
      if(!myStr2Int(str, lit))
         return false;
      outs.push_back(aiger_lit2var(lit));
      gateList[ aiger_lit2var(lit) ] = new CirPoGate((unsigned)lit);
   }
   for(int i = 0; i < miloa[4]; i++) {
      
      if(!getline(ifs, str, '\n'))
         return false;
      aigerAddAnd(str); 
   }

   while(getline(ifs, str, '\n')) {
      if(str == "c") {
         if(!getline(ifs, str, '\n'))
            return false;
         setComment(str);
         continue;
      }
      vector<string> tmp;
      if(!lexOptions(str, tmp, 2))
         return false;
      // i13 symbol
      string ilo;
      ilo += tmp[0][0];
      string str_index = tmp[0].erase(0, 1);
      int index = 0;
      if(!myStr2Int(str_index, index))
         return false;

      if(ilo == "i") {
         cout << "out[index] = " << outs[index] << endl;
         CirPiGate* pi = static_cast<CirPiGate*>( getGate(ins[(unsigned) index]) );
         pi->setName(tmp[1]);
      }
      if(ilo == "o") {
         cout << "out[index] = " << outs[index] << endl;
         CirPoGate* po = static_cast<CirPoGate*>( getGate(outs[(unsigned) index]) );
         po->setName(tmp[1]);
      }
   }
   CirPoGate* po = static_cast<CirPoGate*>( getGate(outs[1]) );
   cout << po->getName() << endl;
    
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
}

void
CirMgr::printNetlist() const
{
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
