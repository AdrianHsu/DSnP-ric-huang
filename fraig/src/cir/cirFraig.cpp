/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   unsigned _m = miloa[0], _o = miloa[3];
	HashMap<HashKey, CirGate*> map( getHashSize(_m + _o + 1) );
   buildDfsList();
	for(size_t i = 0; i < _dfsList.size(); i++) {
      CirGate* g = _dfsList[i];
      if(g->getType() != AIG_GATE) continue;
      if(g->getfinSize() != 2) return; //error
      HashKey key((size_t)g->getInput(0), (size_t)g->getInput(1));
      cerr << key() << " " << (size_t)g->getInput(0) << " " << (size_t)g->getInput(1) << endl;
      if (map.query(key, g)) { // collision happens!
      	CirGate *tmp = _dfsList[i]; // g is old value, and tmp is new value now
      	// replace tmp with g
      	tmp->strashfoutMerge(g);
      	tmp->finfoutRemove();
	      miloa[4]--; // MILO "A"
      	cout << "Strashing: " << g->getId() << " merging " << tmp->getId() << "..." << endl;
      	deleteGate(tmp->getId()); 
      } else {
      	// collision not happens, just insert it
      	map.insert(key, g);
      }
	}
}
void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
