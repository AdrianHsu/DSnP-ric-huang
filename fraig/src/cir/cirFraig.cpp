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
#define aiger_sign(l) \
  (((unsigned)(l))&1)

#define aiger_var2lit(i) \
  (((unsigned)(i)) << 1)

#define aiger_lit2var(l) \
  (((unsigned)(l)) >> 1)
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

      HashKey key(g->getInputWithInv(0), g->getInputWithInv(1));
      // cerr << g->getInputWithInv(0) << "," << g->getInputWithInv(1) << endl;
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
CirMgr::genProofModel(GateList& fecAigList, SatSolver& solver)
{
   gateList[0]->setVar(solver.newVar());
   for(unsigned i = 0; i < _dfsList.size(); i++) {
      CirGate* g = getGate(i);
      // if(g == 0) continue;
      if(g->getType() == AIG_GATE && g->getGrp() != 0) { // in dfs list & has its own Grp
         g->setVar(solver.newVar());
         fecAigList.push_back(g);
         // Construct proof model
         solver.addAigCNF
         (g->getVar(),
          g->getInput(0)->getVar(), g->isInv(0),
          g->getInput(1)->getVar(), g->isInv(1) );
      }
   }
}
void 
CirMgr::reportResult(CirGate* g, CirGate* tmp, SatSolver& solver, bool result)
{
   cout << "\rProving (" << g->getId() << ", "
              << (tmp->isFecInv()? "!" : "")
              << tmp->getId() << ")...";
   if(!result) {
      // replace tmp with g
      cout << "Fraig: " << g->getId() << " merging " 
           << (tmp->isFecInv()? "!" : "") << tmp->getId() << endl;
      tmp->strashfoutMerge(g);
      tmp->finfoutRemove();
      miloa[4]--; // MILO "A"
      deleteGate(tmp->getId());
   } else {
      // do nothing
   }
}
void
CirMgr::prove(GateList& fecAigList, SatSolver& s)
{
   unsigned n = _listFecGrps.size();
   bool result = false;
   for(unsigned i = 0; i < fecAigList.size(); i++) {
      CirGate* g = fecAigList[i];
      if(g == 0) continue;
      FecGrp* f = g->getGrp();
      result = false;
      bool finalResult = false;
      if(f == 0 || f->getSize() == 0) continue;
      for(unsigned j = 0; j < f->getSize(); j++) {
         CirGate* tmp = f->getGate(j);
         if(tmp->getType() == CONST_GATE) continue;
         Var newV = s.newVar();

         s.addXorCNF(newV, g->getVar(), g->isFecInv(), tmp->getVar(), tmp->isFecInv());
         s.assumeRelease();
         s.assumeProperty(gateList[0]->getVar(), false);
         s.assumeProperty(newV,true);
         result = s.assumpSolve();
         reportResult(g, tmp, s, result);
         if(!result) {
            finalResult = result;
            f->removeGate(j);
         } else {

         }
      }
      if(finalResult)
         cout << "\rUpdating by UNSAT... Total #FEC Group = " << --n << endl;
      else
         cout << "\rUpdating by SAT... Total #FEC Group = " << --n << endl;

   }
   clearListFecGrps();
}
void
CirMgr::fraig()
{
   //initCircuit()
   GateList fecAigList;
   buildDfsList(); //setGlobalRef()
   SatSolver solver;
   solver.initialize();
   genProofModel(fecAigList, solver);
   prove(fecAigList, solver);
   buildDfsList();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
