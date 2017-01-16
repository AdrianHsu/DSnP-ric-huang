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
      g->setVar(solver.newVar());
      if(g->getType() == AIG_GATE) { // in dfs list & has its own Grp
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
CirMgr::prove(GateList& fecAigList, SatSolver& s)
{
   unsigned n = _listFecGrps.size();
   bool result = false;
   cout << "\r                                   ";
   for(unsigned i = 0; i < fecAigList.size(); i++) {
      CirGate* g = fecAigList[i];
      if(g == 0) continue;
      FecGrp* f = g->getGrp();
      result = false;
      bool finalResult = false;
      if(f == 0 || f->getSize() == 0) continue;
      for(unsigned j = 0; j < f->getSize(); j++) {
         CirGate* tmp = f->getGate(j);
         if(tmp->getType() == CONST_GATE || tmp == g) continue;
         Var newV = s.newVar();
      
         s.addXorCNF(newV, g->getVar(), g->isFecInv(), tmp->getVar(), tmp->isFecInv());
         s.assumeRelease();
         s.assumeProperty(gateList[0]->getVar(), false);
         s.assumeProperty(newV,true);
         result = s.assumpSolve();
         cout << "\rProving (" << g->getId() << ", "
              << (tmp->isFecInv()? "!" : "")
              << tmp->getId() << ")..."  << (result ? "SAT" : "UNSAT") << "!!";

         if(!result) {
            // replace tmp with g
            cout << "\r                                   "
                 << "\rFraig: " << g->getId() << " merging " 
                 << (tmp->isFecInv()? "!" : "") << tmp->getId() << "..." << endl;
            tmp->fraigMerge(g, tmp->isFecInv() ^ g->isFecInv()); 
            tmp->finfoutRemove();
            miloa[4]--; // MILO "A"
            deleteGate(tmp->getId());
            finalResult = result;
            f->removeGate(j);
            j--;
         } else {

         }
      }
      if(f->getSize() == 1) {
         f->getGate(0)->clearMyFecGrp(); // invalid
         f->removeGate(0);
      }
      if(!finalResult)
         cout << "Updating by UNSAT... Total #FEC Group = " << --n << endl;
      else
         cout << "Updating by SAT... Total #FEC Group = " << --n << endl;

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
