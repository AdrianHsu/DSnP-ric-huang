/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
// #include "fecGrp.h"
using namespace std;



// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   if(!patternFile.is_open()) {
      return;
   }
   string str;
   size_t _i = miloa[1];
   vector<string> inputs;
   while(patternFile >> str) {
      if(str.size() != _i && str.size() != 0) {
         cerr << "Error: Pattern(" << str <<") length(" << str.size() 
              << ") does not match the number of inputs("
              << _i << ") in a circuit!!" << endl;
         cout << "\r" << "0 patterns simulated." << endl;
         return;
      }  
      inputs.push_back(str);
   }
   unsigned div = 0, mod = inputs.size() % BIT_32; // div: num of round, e.g. 64 is 2 rounds
   if(mod != 0) {
      div = inputs.size() / BIT_32;
      mod = BIT_32 * (div + 1) - inputs.size();
   }
   for(unsigned j = 0; j < mod; j ++) {
      char zero_str [_i];
      memset(zero_str, '0', sizeof(zero_str));
      zero_str[_i] = '\0';
      inputs.push_back(zero_str);
   }
   div = inputs.size() / BIT_32;
   vector<unsigned> _32bitvec;
   for(unsigned i = 0; i < div; i++) {
      for(unsigned j = 0; j < _i; j++) {
         unsigned pattern = 0;
         for(unsigned k = 0; k < BIT_32; k++) {
            int new_bit = inputs[k + i * BIT_32][j] - '0';
            if(new_bit != 0 && new_bit != 1) {
               cerr << "Error: Pattern(" << inputs[k + i * BIT_32] 
                    << ") contains a non-0/1 character('" 
                    << inputs[k + i * BIT_32][j] << "')." << endl;
               cout << "\r" << "0 patterns simulated." << endl;
               return;
            } 
            pattern = (pattern << 1) + new_bit; // key!
         }
         _32bitvec.push_back(pattern);
      }
   }
   // start sim
   unsigned a = 0;
   buildDfsList();
   fecGrpsInit(); //first time build map
   for( ; a < div; a++) {
      simulate(a, _32bitvec);
      fecGrpsIdentify();
   }

   // cout << "\rTotal #FEC Group = " << _fecList.numGroups();
   cout << "\r" << div * BIT_32 - mod << " patterns simulated." << endl;

}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

void
CirMgr::simulate(unsigned& round, vector<unsigned>& _32bitvec)
{
// All-gate simulation:
// Perform simulation for each gate on the DFS list
   unsigned ins_index = 0;
   unsigned _i = miloa[1];
   for(unsigned i = 0; i < _dfsList.size(); i++) {
      CirGate* g = _dfsList[i];
      if(g->getType() == PI_GATE) {
         CirPiGate* pi = (CirPiGate*) g;
         ins_index = pi->getPiIndex();
         unsigned pattern = _32bitvec[round * _i + ins_index]; //
         g->setSimValue(pattern);
      } else if (g->getType() == CONST_GATE) {
         g->setSimValue(0);
      } else if(g->getType() == AIG_GATE) {
         CirGate* lhs0 = g->getInput(0);
         CirGate* lhs1 = g->getInput(1);
         unsigned v0 = lhs0->getSimValue(), v1 = lhs1->getSimValue();
         if(g->isInv(0)) v0 = ~v0;
         if(g->isInv(1)) v1 = ~v1;
         g->setSimValue(v0 & v1);
      } else if (g->getType() == UNDEF_GATE) {
         g->setSimValue(0);
      } else { // PO_GATE
         CirGate* lhs = g->getInput(0);
         unsigned v0 = lhs->getSimValue();
         if(g->isInv(0)) v0 = ~v0;
         g->setSimValue(v0);
      }
   }
}

void
CirMgr::fecGrpsInit()
{
   // simValue are already given in 1st round
   // Initial: put all the signals in ONE FEC group.
   FecGrp *fecGrp = new FecGrp(0);
   for(unsigned i = 0; i < _dfsList.size(); i++) {
      CirGate* g = _dfsList[i];
      if(g->getType() == AIG_GATE) {
         bool b = false;
         fecGrp->addGate(g, b);
         g->setMyFecGrp(fecGrp, b);
      }
   }
   //Add this FEC group into fecGrps (list of FEC groups)
   _listFecGrps.push_back(fecGrp);
}

void
CirMgr::fecGrpsIdentify()
{

// LET'S DO THE MOST IMPORTANT PART!
// for_each(fecGrp, fecGrps): 
//    Hash<SimValue, FECGroup> newFecGrps; 
//    for_each(gate, fecGrp)
//       grp = newFecGrps.check(gate);
//       if (grp != 0) // existed
//          grp.add(gate);
//       else 
//          createNewGroup(newFecGrps,gate);
// CollectValidFecGrp(newFecGrps, fecGrp,fecGrps);

   unsigned listSize = _listFecGrps.size(); // listSize = 1
   unsigned _a = miloa[4];
   FecGrp* fecGrp = 0;
   for(unsigned i = 0; i < listSize; i ++) {
      FecMap newFecMap( getHashSize(_a + 1) );
      fecGrp = _listFecGrps[i];
      for(unsigned j = 0; j < fecGrp->getSize(); j++) {
         CirGate* gate = fecGrp->getGate(j);
         FecGrp* grp = NULL;
         FecHashKey key(gate->getSimValue());
         bool inv = 0;
         bool isChecked = newFecMap.check(key, grp, inv);
         if(isChecked) {
            grp->addGate(gate, inv);
            gate->setMyFecGrp(grp, inv);
         } else
            createNewGroup(newFecMap, gate, inv);
      }
      collectValidFecGrp(newFecMap, fecGrp, i);
   }
   sortListFecGrps();
}
void
CirMgr::createNewGroup(FecMap& newFecMap, CirGate* g, bool& inv)
{
   FecGrp *fecGrp = new FecGrp(g->getSimValue());
   fecGrp->addGate(g, inv);
   g->setMyFecGrp(fecGrp, inv);
   FecHashKey key(g->getSimValue());
   newFecMap.insert(key, fecGrp);
}
bool orderInsideSort (CirGate* i, CirGate* j) { return (i->getId() < j->getId()); }  
void
CirMgr::collectValidFecGrp(FecMap& newFecMap, FecGrp* fecGrp, unsigned& i){
   // "fecGrps" in pdf, is _listFecGrps

   //delete original fecGrp
   unsigned s = _listFecGrps.size();
   _listFecGrps[i] = _listFecGrps.back();
   _listFecGrps.resize(s - 1);
   FecMap::iterator it = newFecMap.begin();
   for(; it != newFecMap.end(); it++) {
      FecGrp* tmp = (*it).second;
      std::sort (tmp->getList().begin(), tmp->getList().end(), orderInsideSort);
      if(tmp->getSize() > 1) 
         _listFecGrps.push_back(tmp); // valid
      else {
         tmp->getGate(0)->clearMyFecGrp(); // invalid
      }
   }
}