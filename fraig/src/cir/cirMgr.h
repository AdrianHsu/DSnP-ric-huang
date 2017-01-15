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
// by AH
// #include "fecGrp.h"

extern CirMgr *cirMgr;

#define NEG 0x1

#define aiger_sign(l) \
  (((unsigned)(l))&1)

#define aiger_var2lit(i) \
  (((unsigned)(i)) << 1)

#define aiger_lit2var(l) \
  (((unsigned)(l)) >> 1)

class FecGrp;

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
   void buildDfsList() {
      _dfsList.clear();
      unsigned _m = miloa[0], _o = miloa[3];
      bool first = 1;
      for (unsigned i = _m + 1, size = _m + _o + 1; i < size; ++i) {
         CirGate *g = getGate(i);
         if(g == 0) return; // PO error
         if(g->getType() == PO_GATE) {
            if(first) {
               g->setGlobalRef();
               first = 0;
            }
            g->runColorDFS(_dfsList);
         }
      }
   }   
   OptType getOptType(CirGate*, bool& , size_t&) const;
   unsigned myStr2Uns(const string& str) {
     unsigned num = 0;
     size_t i = 0;
     //int sign = 1;
     //if (str[0] == '-')  return false;
     for (; i < str.size(); ++i) {
        if (isdigit(str[i])) {
           num *= 10;
           num += unsigned(str[i] - '0');
        }
        else return 0;
     }
     return num;
   }
   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   // by AH
   void simulate(unsigned&, vector<unsigned>&);
   void fecGrpsInit();
   void fecGrpsIdentify(ListFecGrps&, ListFecGrps&);
   void createNewGroup(FecMap&, CirGate*, bool&);
   void collectValidFecGrp(FecMap&, unsigned&, ListFecGrps&);

   static bool grpOrderSort (FecGrp* i,FecGrp* j) { 
      return (i->get1stGate()->getId() < j->get1stGate()->getId()); 
   }

   void sortListFecGrps(ListFecGrps& _l) {
      std::sort (_l.begin(), _l.end(), grpOrderSort);
   }
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
   ListFecGrps _listFecGrps;
   ListFecGrps _tmpListFecGrps;

private:
   ofstream           *_simLog;
   // aag M I L O A == aag [0] [1] [2] [3] [4]
   unsigned miloa[5];
   GateList gateList;
   string comment;
};

#endif // CIR_MGR_H
