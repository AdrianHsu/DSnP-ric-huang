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
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep() {
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
         g->runColorDFS();
      }
   }
   for(unsigned i = 0; i < _m + 1; ++i) {
   	CirGate *g = getGate(i);
   	if(g == 0) continue;
   	if(!g->isGlobalRef() && g->getType() != PI_GATE && g->getType() != CONST_GATE) {
   		g->finReset();
   		cout << "Sweeping: " << g->getTypeStr() << "(" << g->getId() << ") removed..." << endl;
   		if(g->getType() == AIG_GATE)
   			miloa[4]--;

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

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
