/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/

void
CirMgr::buildDfsList() {
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

// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep() {
   unsigned _m = miloa[0];
   buildDfsList();
   for(unsigned i = 0; i < _m + 1; ++i) {
   	CirGate *g = getGate(i);
   	if(g == 0) continue;
   	if(!g->isGlobalRef() && g->getType() != PI_GATE && g->getType() != CONST_GATE) {
   		g->finfoutRemove();
   		cout << "Sweeping: " << g->getTypeStr() << "(" << g->getId() << ") removed..." << endl;
   		if(g->getType() == AIG_GATE)
   			miloa[4]--; // MILO "A"

   		deleteGate(i);
   	}
   }
   // DEBUG
   // for(unsigned i = 0; i < _m + _o + 1; i++) {
   //    CirGate *g = getGate(i);
   // 	if(g == 0) {
   // 		cout << i << endl;
   // 		continue;
   // 	}
   // 	cout << g->getTypeStr() << "," << g->getId() << endl;
   // }
}
OptType
CirMgr::getOptType(CirGate* g, bool& inv) const 
{
   
   CirGate* rhs0 = g->getInput(0);
   CirGate* rhs1 = g->getInput(1);
   if(rhs0 == rhs1) {
      size_t inv_num = 0;
      inv_num += g->isInv(0);
      inv_num += g->isInv(1);
      if(inv_num == 0) {
         inv = 0;
         return SAME_FANIN;
      } else if (inv_num == 2) {
         inv = 1;
         return SAME_FANIN;
      } else 
         return INVERT_FANIN;
   }
   if(rhs0->getType() == CONST_GATE) { //rhs1 must not CONST_GATE
      if(g->isInv(0)) {
         inv = 0;
         return CONST_ONE;
      } else 
         return CONST_ZERO;
   } else if(rhs1->getType() == CONST_GATE) {
      if(g->isInv(1)) {
         inv = 1;
         return CONST_ONE;
      } else 
         return CONST_ZERO;
   }
   return NO_OPT;
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   buildDfsList();
   for(size_t i = 0; i < _dfsList.size(); i++) {
      CirGate* g = _dfsList[i];
      if(g->getType() != AIG_GATE) continue;
      if(g->getfinSize() != 2) return; //error
      bool inv = 0;
      OptType type = getOptType(g, inv);
      if(type == NO_OPT) continue;
      CirGate *tmp = NULL;
      bool is_inv = 0;
      size_t id = 0;
      if(type == CONST_ZERO || type == INVERT_FANIN) { // inv no use
         tmp = getGate(0);
         is_inv = 0;
         id = 0 ;
      } else if (type == CONST_ONE) {
         // if(!inv) { // it means rhs0 is inv & rhs0 is CONST_GATE
         //    tmp = g->getInput(1);
         //    is_inv = g->isInv(1);
           
         // } else { // it means rhs1 is inv & rhs1 is CONST_GATE
         //    tmp = g->getInput(0);
         //    is_inv = g->isInv(0);
         // }
         tmp = g->getInput(!inv);
         is_inv = g->isInv(!inv);
         id = tmp->getId();
      } else if (type == SAME_FANIN) {
         // if(inv) { // it means both rhs0,1 are inv
         //    tmp = getInput(0);
         // } else { // it means both rhs0,1 are not inv
         //    tmp = getInput(0);
         // }
         tmp = g->getInput(0);
         is_inv = inv;
         id = tmp->getId();
      } 
      if(tmp == NULL) continue;
      if(is_inv)
         cout << "Simplifying: " << id << " merging !" << g->getId() << "..." << endl;
      else 
         cout << "Simplifying: " << id << " merging " << g->getId() << "..." << endl;
      for(size_t j = 0; j < g->getfoutSize(); j++) {
         tmp->addOutput(g->getOutput(j));
         g->getOutput(j)->addInput(tmp, is_inv);
      }
      g->finfoutRemove();
      miloa[4]--; // MILO "A"
      deleteGate(g->getId()); 
      // else do nothing
   }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/