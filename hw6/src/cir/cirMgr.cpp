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

// Convert string "str" to unsigned integer "num". Return false if str does not appear
// to be a number
unsigned
myStr2Uns(const string& str)
{
   unsigned num = 0;
   size_t i = 0;
   //int sign = 1;
   //if (str[0] == '-')  return false;
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += unsigned(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   return num;
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
CirMgr::aigerAddAnd(string& str, unsigned lNo) {
   
   unsigned var[3] = {0, 0, 0}; // lhs, rhs0, rhs1
   vector<string> tmp;
   if(!lexOptions(str, tmp, 3))
      return false;
   for(int i = 0; i < 3; i++) {
      var[i] = myStr2Uns(tmp[i]);
      var[i] = aiger_lit2var(var[i]);
   }
   CirAigGate* aig = new CirAigGate(var[0], lNo);
   gateList[ var[0] ] = aig;
   return true;
}
bool
CirMgr::aigerAddUndef(string& str) {
   
   unsigned lit[3] = {0, 0, 0}; // lhs, rhs0, rhs1
   unsigned var[3] = {0, 0, 0}; // lhs, rhs0, rhs1
   vector<string> tmp;
   if(!lexOptions(str, tmp, 3))
      return false;
   for(int i = 0; i < 3; i++) {
      lit[i] = myStr2Uns(tmp[i]);
      var[i] = aiger_lit2var(lit[i]);
   }
   
   CirAigGate* g1 = static_cast<CirAigGate*>(getGate( var[0] ));
   CirGate* g2 = getGate( var[1] );
   if(g2 == 0)
      g2 = gateList[ var[1] ] = new CirUndefGate(var[1], 0);
   
   CirGate* g3 = getGate( var[2] );
   if(g3 == 0)
      g3 = gateList[ var[2] ] = new CirUndefGate(var[2], 0);
   // add fanin, fanout 
   g1->addInput(g2, aiger_sign(lit[1]));
   g1->addInput(g3, aiger_sign(lit[2]));
   g2->addOutput(g1);
   g3->addOutput(g1);

   return true;
}
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream ifs(fileName);
   if(!ifs.is_open())
      return false;
   
   // without error handling
   string str;
   vector<string> cmd;
   while(getline(ifs, str, '\n'))
      cmd.push_back(str);
   ifs.close(); 

   vector<string> tmp;
   // aag M I L O A
      cout << "??" << endl;
   if(!lexOptions(cmd[0], tmp, 6))
      return false;
   for(int i = 1; i <= 5; i++)
      miloa[i - 1] = myStr2Uns(tmp[i]);
   
   unsigned _m = miloa[0], _i = miloa[1], _o = miloa[3], _a = miloa[4];
   gateList.clear();
   gateList.resize(miloa[0] + miloa[3] + 1, 0);
   // const gate
   gateList[0] = new CirConstGate(0);// dummy lineNo
   ins.clear();
   for(int i = 0; i < _i; i++) {
      
      unsigned lit = myStr2Uns(cmd[i + 1]);
      unsigned var = aiger_lit2var(lit);
      ins.push_back(var);
      gateList[ var ] = new CirPiGate(var, i + 2);
   }
   for(int i = 0; i < _a; i++) {
      unsigned lNo = i + _i + _o + 2;
      aigerAddAnd(cmd[i + _i + _o + 1], lNo); 
   }
   for(int i = 0; i < _a; i++)
      aigerAddUndef(cmd[i + _i + _o + 1]); 
   
   for(int i = 0; i < _o; i++) {
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
         static_cast<CirPiGate*>(getGate(ins[id]))->setName(sym);
      else if(ilo == 'o')
         static_cast<CirPoGate*>(getGate(id + _m + 1))->setName(sym);
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
CirMgr::resetColors() const
{
   unsigned _m = miloa[0], _o = miloa[3];
   for (unsigned i = 0, size = _m + _o + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      g->setColor(0);
   }
}

void
CirMgr::printNetlist() const
{
   unsigned _m = miloa[0], _o = miloa[3];
   cout << endl;
   for (unsigned i = 0, size = _m + _o + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if (g->getType() == PO_GATE) {
         g->printGate();
      }
   }
   CirGate::index = 0;
   resetColors();
}

void
CirMgr::printPIs() const
{
   if(ins.size() != 0) {
      cout << "PIs of the circuit:";
      for (unsigned i = 0; i < ins.size(); ++i) {
         cout << " " << ins[i];
      }
      cout << endl;
   }
   CirGate::index = 0;
   resetColors();
   
}
void
CirMgr::printPOs() const
{
   unsigned _m = miloa[0], _o = miloa[3];
   if(_o != 0) {
      cout << "POs of the circuit:";
      for (unsigned i = _m + 1, size = _m + _o + 1; i < size; ++i) {
         CirGate *g = getGate(i);
         if (g != 0 && g->getType() == PO_GATE)
            cout << " " << g->getId();
      }
      cout << endl;
   }
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
      for(int i = 0; i < case1.size(); i++)
         cout << " " << case1[i];
      cout << endl;
   }
   for (unsigned i = 0, size = _m + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if (g->getType() == PI_GATE || g->getType() == AIG_GATE) {
         if(g->getfoutSize() == 0)
            case2.push_back(g->getId());
      }
   }
   if(case1.size() != 0) {
      cout << "Gates defined but not used  :";
      for(int i = 0; i < case2.size(); i++)
         cout << " " << case2[i];
      cout << endl;
   }
}
void
CirMgr::writeDfsVisit(CirGate* g, vector<unsigned>& aigs, bool inv, vector<string>& ins_symbol) const
{
   if(g->getColor()) return;
   for(int i = 0; i < g->getfinSize(); i++)
      writeDfsVisit(g->getInput(i), aigs, g->isInv(i), ins_symbol);
   if(g->getType() == PI_GATE) {
      //ins.push_back( aiger_var2lit( g->getId() ) );
      CirPiGate* pi = (CirPiGate*) g;
      ins_symbol.push_back(pi->getName());
   } else if(g->getType() == AIG_GATE) {
      unsigned _lhs = aiger_var2lit(g->getId());
      aigs.push_back(_lhs); //_lhs must even
      unsigned _rhs0 = aiger_var2lit(g->getInput(0)->getId());
      if(g->isInv(0))_rhs0++;
      aigs.push_back(_rhs0);
      unsigned _rhs1 = aiger_var2lit(g->getInput(1)->getId());
      if(g->isInv(1))_rhs1++;
      aigs.push_back(_rhs1);
   }
   g->setColor(1);
}
void
CirMgr::writeAag(ostream& outfile) const
{
   
   unsigned _m = miloa[0], _i = miloa[1], _l = miloa[2], _o = miloa[3], _a = miloa[4];
   //vector<unsigned> mins;
   vector<unsigned> outs;
   vector<unsigned> aigs;
   vector<string> ins_symbol;
   outfile << "aag " << _m << " " << _i << " "
      << _l << " " << _o << " " << _a << endl;
   for (unsigned i = 0, size = _m + _o + 1; i < size; ++i) {
      CirGate *g = getGate(i);
      if (g == 0) continue;
      if (g->getType() == PO_GATE) {
         writeDfsVisit(g, aigs, g->isInv(0), ins_symbol);
         unsigned lit = aiger_var2lit(g->getInput(0)->getId());
         if(g->isInv(0))lit++;
         outs.push_back(lit);
      }
   }
   CirGate::index = 0;
   resetColors();
   for(unsigned i = 0; i < ins.size(); i++)
      outfile << aiger_var2lit(ins[i]) << endl;
   for(unsigned i = 0; i < outs.size(); i++)
      outfile << outs[i] << endl;
   for(unsigned j = 0; j < aigs.size(); j++) {
      outfile << aigs[j] << " ";
      outfile << aigs[++j] << " ";
      outfile << aigs[++j] << endl;
   }
   for(int i = 0; i < ins.size(); i++)
      if(ins_symbol[i] != "")outfile << "i" << i << " " << ins_symbol[i] << endl;
   
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
