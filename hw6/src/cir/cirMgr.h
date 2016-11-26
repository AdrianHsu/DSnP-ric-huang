/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(): flag(0) {
      for(int i = 0; i < 5; i++) miloa[i] = 0;
   }
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if(gid > miloa[0] + miloa[3]) return 0;
      return gateList[gid];
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // by AH
   bool lexOptions(const string&, vector<string>&, size_t) const;
   bool aigerAddAnd(string&);
   bool aigerAddUndef(string&);
   void setComment(string& str){ comment.clear(); comment = str; };

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

private:
   bool flag;
   // aag M I L O A == aag [0] [1] [2] [3] [4]
   unsigned miloa[5];
   GateList gateList;
   string comment;
};

#endif // CIR_MGR_H
