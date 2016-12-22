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

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

#define NEG 0x1

#define aiger_sign(l) \
  (((unsigned)(l))&1)

#define aiger_var2lit(i) \
  (((unsigned)(i)) << 1)

#define aiger_lit2var(l) \
  (((unsigned)(l)) >> 1)

class CirMgr
{
public:
   CirMgr() {
      for(int i = 0; i < 5; i++) miloa[i] = 0;
   }
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const {
      if(gid > miloa[0] + miloa[3]) return 0;
      return gateList[gid];
   }

   // by AH
   bool aigerAddAnd(vector<string>&);
   void setComment(string& str){ comment.clear(); comment = str; };
   string getComment() const { return comment; };
   void writeDfsVisit(CirGate*, vector<unsigned>&, bool) const;
   IdList ins; 
   // by AH for sweep
   void deleteGate(unsigned gid) {
      if(gid > miloa[0] + miloa[3] || gateList[gid] == 0) return;

      delete gateList[gid];
      gateList[gid] = 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();
   // by AH
   void buildDfsList();
   OptType getOptType(CirGate*, bool& , size_t&) const;

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   GateList _dfsList;

private:
   ofstream           *_simLog;
   // aag M I L O A == aag [0] [1] [2] [3] [4]
   unsigned miloa[5];
   GateList gateList;
   string comment;
};

#endif // CIR_MGR_H
