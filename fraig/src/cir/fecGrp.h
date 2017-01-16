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
      void removeGate(unsigned i) {
         if (i >= _gateList.size()) return;
         _gateList[i] = 0;
         _gateList.erase(_gateList.begin() + i);
      }
      void setGate(CirGate* g, unsigned i) {
         _gateList[i] = g;
      }      
      bool isInv(unsigned i) const { return ((size_t)_gateList[i] & NEG); }
      size_t getSize() { 
        return _gateList.size(); 
      }
      GateList getList(){ return _gateList; }
      CirGate* get1stGate() { 
         if(_gateList.size() > 0) {
            unsigned i = 0;
            CirGate* g = _gateList[i];
            return g;
         }
         return 0; // no gates
      }
      void setSimValue(size_t s) { simValue = s; }
      size_t getSimValue() { return simValue; }
      // void clear() {
      //   for(unsigned i = 0; i < _gateList.size(); i++)
      //      getGate(i)->clearMyFecGrp();
      //   _gateList.clear();
      //   simValue = 0;
      // }

   private:
      GateList _gateList;
      size_t simValue;
};


#endif
