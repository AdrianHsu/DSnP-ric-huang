/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "util.h"
// #include "fecGrp.h"
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
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
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

void getTokens (const string& option, vector<string>& tokens) {
   
   string token;
   string second = option;
   myStrGetTok(option, token);
   tokens.push_back(token);
   second = second.substr(token.size() + 1);
   tokens.push_back(second);
}
// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
lexOptions
(const string& option, vector<string>& tokens, size_t nOpts)
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         return false;
      }
      if (tokens.size() > nOpts) {
         return false;
      }
   }
   return true;
}


/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr() {
   size_t s  = gateList.size();
   if(s == 0) return;
   bool first = 1;
   for(int i = s - 1; i >= 0; i--)
      if(gateList[i] != NULL) {
         if(first) {
            gateList[i]->resetGlobalRef();
            first = 0;
         }
         delete gateList[i]; gateList[i] = 0; //added in fraig
      }
   gateList.clear();

}

bool
CirMgr::aigerAddAnd( vector<string>& cmd) {

   unsigned _i = miloa[1], _o = miloa[3], _a = miloa[4];
   unsigned lNo = 0;
   string str = "";
   IdList idList;
   for(unsigned i = 0; i < _a; i++) {
      lNo = i + _i + _o + 2;
      str = cmd[i + _i + _o + 1];

      unsigned lit[3] = {0, 0, 0}; // lhs, rhs0, rhs1
      vector<string> tmp;
      if(!lexOptions(str, tmp, 3))
         return false;
      for(int j = 0; j < 3; j++) {
         lit[j] = myStr2Uns(tmp[j]);
         idList.push_back(lit[j]);
      }
      unsigned var = aiger_lit2var(lit[0]);
      CirAigGate* aig = new CirAigGate(var, lNo);
      gateList[ var ] = aig;
   }
   for(unsigned i = 0; i < _a; i++) {

      str = cmd[i + _i + _o + 1];
      unsigned lit[3] = {0, 0, 0}; // lhs, rhs0, rhs1
      unsigned var[3] = {0, 0, 0}; // lhs, rhs0, rhs1
      for(int j = 0; j < 3; j++) {
         lit[j] = idList[ 3 * i + j ];
         var[j] = aiger_lit2var(lit[j]);
      }
      
      CirAigGate* g1 = (CirAigGate*) getGate( var[0] );
      CirGate* g2 = getGate( var[1] );
      CirGate* g3 = getGate( var[2] );
      if(g2 == 0) {
         g2 = gateList[ var[1] ] = new CirUndefGate(var[1], 0);
      }
      if(g3 == 0) {
         g3 = gateList[ var[2] ] = new CirUndefGate(var[2], 0);
      }
      // add fanin, fanout 
      g1->addInput(g2, aiger_sign(lit[1]));
      g1->addInput(g3, aiger_sign(lit[2]));
      g2->addOutput(g1);
      g3->addOutput(g1);
   }
   return true;
}
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream ifs(fileName.c_str());
   if(!ifs.is_open())
      return false;
   
   // without error handling
   string str;
   vector<string> cmd;
   while(getline(ifs, str, '\n'))
      cmd.push_back(str);
   ifs.close(); 
   if(cmd[0][0] != 'a') return parseError(ILLEGAL_WSPACE);
   vector<string> tmp;
   // aag M I L O A
   if(!lexOptions(cmd[0], tmp, 6))
      return parseError(ILLEGAL_WSPACE);
   for(int i = 1; i <= 5; i++)
      miloa[i - 1] = myStr2Uns(tmp[i]);
   
   unsigned _m = miloa[0], _i = miloa[1], _o = miloa[3], _a = miloa[4];
   gateList.clear();
   gateList.resize(_m + _o + 1, 0);
   // const gate
   gateList[0] = new CirConstGate(0);// dummy lineNo
   ins.clear();
   for(unsigned i = 0; i < _i; i++) {
      
      unsigned lit = myStr2Uns(cmd[i + 1]);
      unsigned var = aiger_lit2var(lit);
      ins.push_back(var);
      gateList[ var ] = new CirPiGate(var, i + 2, i);
   }
   aigerAddAnd(cmd);
   
   for(unsigned i = 0; i < _o; i++) {
      unsigned lit = myStr2Uns(cmd[i + _i + 1]);
      unsigned lNo = i + _i + 2;
      CirPoGate *gate = new CirPoGate(i + _m + 1, lNo);
      //add fanin
      unsigned var = aiger_lit2var(lit);
      CirGate *pre = getGate( var );
      if(pre == 0)
         pre = gateList[ var ] = new CirUndefGate(var, 0);
      gate->addInput(pre, aiger_sign(lit));
      gateList[i + _m + 1] = gate;
      pre->addOutput(gate);
   }
   for(unsigned i = 0; i < _m + _o + 1; i ++) {
      CirGate* g = getGate(i);
      if(g == 0) continue;
      g->sortfoutList();
   }

   unsigned i = _i + _o + _a + 1, listSize = cmd.size();
   for( ;i < listSize; i++) {
      string s = cmd[i];
      if(s == "c") {
         if(i + 1 == listSize) break;
         string comment = cmd[++i];
         while(i != listSize - 1) {
            comment += '\n';
            comment += cmd[++i];
         }

         setComment(comment);
         break;
      }
      vector<string> tmp;
      getTokens(s, tmp);
      if(tmp.size() != 2) return false;
      string sym = tmp[1];
      unsigned id = myStr2Uns(tmp[0].substr(1));// i13, o271..etc
      char ilo = tmp[0][0];
      if(ilo == 'i')
         ((CirPiGate*)getGate(ins[id]))->setName(sym);
      else if(ilo == 'o')
         ((CirPoGate*)getGate(id + _m + 1))->setName(sym);
   }
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
   cout  << "\nCircuit Statistics\n"
      << "==================\n"
      << "  PI" << setw(12) << right << miloa[1] << "\n"
      << "  PO" << setw(12) << right << miloa[3] << "\n"
      << "  AIG" << setw(11) << right << miloa[4] << "\n"
      << "------------------\n"
      << "  Total" << setw(9) << right << miloa[1] + miloa[3] + miloa[4] << '\n';
}

void
CirMgr::printNetlist() const
{
   unsigned _m = miloa[0], _o = miloa[3];
   cout << endl;
   bool first = 1;
   for (unsigned i = _m + 1, size = _m + _o + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if (g->getType() == PO_GATE) {
         if(first) {
            g->setGlobalRef();
            first = 0;
         }
         g->printNetDFS();
      }
   }
   CirGate::index = 0;
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   if(ins.size() != 0) {
      for (unsigned i = 0; i < ins.size(); ++i) {
         cout << " " << ins[i];
      }
   }
   cout << endl;
}
void
CirMgr::printPOs() const
{
   unsigned _m = miloa[0], _o = miloa[3];
   cout << "POs of the circuit:";
   if(_o != 0) {
      for (unsigned i = _m + 1, size = _m + _o + 1; i < size; ++i) {
         CirGate *g = getGate(i);
         if (g != 0 && g->getType() == PO_GATE)
            cout << " " << g->getId();
      }
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   unsigned _m = miloa[0], _i = miloa[1] ,_o = miloa[3];
   vector<unsigned> case1;
   vector<unsigned> case2;
   for (unsigned i = _i + 1, size = _m + _o + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if(g->getType() == PO_GATE) {
         if(g->getfinSize() != 1) return; //error
         if(g->getInput(0)->getType() == UNDEF_GATE)
            case1.push_back(g->getId());

      } else if(g->getType() == AIG_GATE) {
         if(g->getfinSize() != 2) return; // error
         for(unsigned i = 0; i < g->getfinSize(); i++) {

            if(g->getInput(i)->getType() == UNDEF_GATE) {
               case1.push_back(g->getId());
               break;
            }
         }
      }
   }
   if(case1.size() != 0) {
      cout << "Gates with floating fanin(s):";
      for(unsigned i = 0; i < case1.size(); i++)
         cout << " " << case1[i];
      cout << endl;
   }
   for (unsigned i = 0, size = _m + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if (g->getType() == PI_GATE || g->getType() == AIG_GATE ) {
         if(g->getfoutSize() == 0)
            case2.push_back(g->getId());
      }
   }
   if(case2.size() != 0) {
      cout << "Gates defined but not used  :";
      for(unsigned i = 0; i < case2.size(); i++)
         cout << " " << case2[i];
      cout << endl;
   }
}
void
CirMgr::writeDfsVisit(CirGate* g, vector<unsigned>& aigs, bool inv) const
{
   if(g->isGlobalRef()) return;
   g->setToGlobalRef(); // fraig fixed bug
   for(unsigned i = 0; i < g->getfinSize(); i++)
      writeDfsVisit(g->getInput(i), aigs, g->isInv(i));
   if(g->getType() == AIG_GATE) {
      unsigned _lhs = aiger_var2lit(g->getId());
      aigs.push_back(_lhs); //_lhs must even
      unsigned _rhs0 = aiger_var2lit(g->getInput(0)->getId());
      if(g->isInv(0))_rhs0++;
      aigs.push_back(_rhs0);
      unsigned _rhs1 = aiger_var2lit(g->getInput(1)->getId());
      if(g->isInv(1))_rhs1++;
      aigs.push_back(_rhs1);
   }
}
void
CirMgr::writeAag(ostream& outfile) const
{
   
   unsigned _m = miloa[0], _i = miloa[1], _l = miloa[2], _o = miloa[3];
   vector<unsigned> outs;
   vector<unsigned> aigs;
   bool first = 1;
   for (unsigned i = _m, size = _m + _o + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if (g->getType() == PO_GATE) {
         if(first) {
            g->setGlobalRef();
            first = 0;
         }
         writeDfsVisit(g, aigs, g->isInv(0));
         unsigned lit = aiger_var2lit(g->getInput(0)->getId());
         if(g->isInv(0))lit++;
         outs.push_back(lit);
      }
   }
   outfile << "aag " << _m << " " << _i << " "
      << _l << " " << _o << " " << (aigs.size() / 3) << endl;
   CirGate::index = 0;
   for(unsigned i = 0; i < ins.size(); i++)
      outfile << aiger_var2lit(ins[i]) << endl;
   for(unsigned i = 0; i < outs.size(); i++)
      outfile << outs[i] << endl;
   for(unsigned j = 0; j < aigs.size(); j++) {
      outfile << aigs[j] << " ";
      outfile << aigs[++j] << " ";
      outfile << aigs[++j] << endl;
   }
   for(unsigned i = 0; i < ins.size(); i++) {
      CirPiGate* g = (CirPiGate*)gateList[ ins[i] ];
      if(g->getName() != "")outfile << "i" << i << " " << g->getName() << endl;
   }
   for(unsigned i = _m + 1 ,size = _m + _o + 1, count = 0; i < size; i++) {
      CirGate *g = getGate(i);
      if (g != 0 && g->getType() == PO_GATE) {
         CirPoGate *pog = (CirPoGate*)g;
         if(pog->getName() != "") {
            outfile << "o" << count << " " << pog->getName() << endl;
            count++;
         }
      }
   }
   //if(getComment() != "")
      //cout << "c\n" << "AAG output by Pin-Chun (Adrian) Hsu" << endl;
      cout << "c\n" << "AAG output by Chung-Yang (Ric) Huang" << endl;
      //cout << "c\n" << getComment() << endl;
}

void 
CirMgr::printFECPairs() const
{
   for(unsigned i = 0; i < _listFecGrps.size(); i++) {
      FecGrp* grp = _listFecGrps[i];
      cout << "[" << i << "]";
      for(unsigned j = 0; j < grp->getSize(); j++) {
         CirGate* g = grp->getGate(j);
         bool inv = grp->isInv(j);
         cout << " ";
         if(inv)
            cout << "!";
         cout << g->getId();
      }
   }
   cout << endl;
}
void
CirMgr::writeGate(ostream& os, CirGate* gate) const
{
   
}
