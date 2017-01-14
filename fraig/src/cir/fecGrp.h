/****************************************************************************
  FileName     [ fecGrp.h ]
  PackageName  [ cir ]
  Synopsis     [ My Fec Group ]
  Author       [ Pin-Chun (Adrian) Hsu ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef FEC_GRP_H
#define FEC_GRP_H

#include <vector>
#include <string>
#include <iostream>

#include "cirDef.h"
#define NEG 0x1

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class FecGrp
{
public:
  FecGrp(size_t s = 0): simValue(s) {};
  ~FecGrp(){};

  void addGate(CirGate* g, bool inv = false) {
     if (inv) g = (CirGate*)((size_t)g + 1);
     _gateList.push_back(g);
  }

    CirGate* getGate(unsigned i) const {
       if (i >= _gateList.size()) return 0;
       return (CirGate*)(((size_t)_gateList[i]) & ~size_t(NEG));
    }
    bool isInv(size_t i) const { return ((size_t)_gateList[i] & NEG); }
  size_t getSize() { return _gateList.size(); }

  static bool orderInsideSort (CirGate* i, CirGate* j) { return (i->getId() < j->getId()); }  
  void sortInsideGrp() {
        std::sort (_gateList.begin(), _gateList.end(), orderInsideSort);
  }
    unsigned get1stId() { 
    if(_gateList.size() > 0) {
      CirGate* g = getGate(0);
      return g->getId();
    }
    return 0; // no gates
  }
  void setSimValue(size_t s) { simValue = s; }
  size_t getSimValue() { return simValue; }

private:
  vector<CirGate*> _gateList;
  size_t simValue;
};


#endif
