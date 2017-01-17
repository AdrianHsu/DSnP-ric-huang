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
      if (map.query(key, g)) { // collision happens!
      	CirGate *tmp = _dfsList[i]; // g is old value, and tmp is new value now
      	// replace tmp with g
      	tmp->strashMerge(g);
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
CirMgr::genProofModel(SatSolver& solver)
{
   gateList[0]->setVar(solver.newVar());
   for(unsigned i = 0; i < _dfsList.size(); i++) {
      CirGate* g = _dfsList[i];
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
CirMgr::dfsSort(GateList& live, GateList& die, vector<bool>& boolVec)
{
   GateList memlive;
   GateList memdie;
   vector<bool>  memboolVec;
   for(unsigned i = 0; i < _dfsList.size(); i++) {
      CirGate* g = _dfsList[i];
      for(int j = die.size() - 1; j >= 0; j--) {
         CirGate* h = die[j];
         if(g->getId() == h->getId()) {
            memlive.push_back( live[j] );
            memdie.push_back( die[j] );
            memboolVec.push_back( boolVec[j] );

            die.erase(die.begin() + j);
            live.erase(live.begin() + j);
            boolVec.erase(boolVec.begin() + j);
            break;
         }
      }
   }
   live = memlive;
   die = memdie;
   boolVec = memboolVec;
}
void
CirMgr::prove(SatSolver& s)
{
   int n = _listFecGrps.size();
   bool result = false;
   GateList live;
   GateList die;
   vector<bool> boolVec;
   bool sat = 0, unsat = 0;
   s.assumeProperty(gateList[0]->getVar(), false);

   for(unsigned i = 0; i < fecAigList.size(); i++) {
      CirGate* g = fecAigList[i];
      if(g == 0 || g->isDead()) continue;
      FecGrp* f = g->getGrp();

      result = false;
      if(f == 0 || f->getSize() == 0) continue;
      for(unsigned j = 0; j < f->getSize(); j++) {
         CirGate* tmp = f->getGate(j);
         if(tmp == 0 || tmp == g) continue;
         if(tmp->getId() != 0) {
            Var newV = s.newVar();
            s.addXorCNF(newV, g->getVar(), g->isFecInv(), tmp->getVar(), tmp->isFecInv());
            s.assumeRelease();
            s.assumeProperty(newV, true);
            result = s.assumpSolve();
            
            cout << "\r                                   ";
            bool inv = 0;
            inv = g->isFecInv() ^ tmp->isFecInv();
            cout << "\rProving (" << (g->isFecInv() ? "!" : "") << g->getId() << ", "
                     << (tmp->isFecInv() ? "!" : "") << tmp->getId() << ")..." << flush;
    
            if(!result) {
               cout << "UNSAT!!";

               boolVec.push_back(inv);
               live.push_back(g);
               die.push_back(tmp);
               tmp->setDead();
               for(int k = f->getSize() - 1; k >= 0; k--)
                  if(f->getGate(k) == g)
                     f->removeGate(k);
               f->removeGate(j);
               j--;
               unsat = 1;
            } else {
               cout << "SAT!!";
               for(int k = f->getSize() - 1; k >= 0; k--)
                  if(f->getGate(k) == g)
                     f->removeGate(k);
               f->removeGate(j);
               sat = 1;
               break;
            }

         }
         else {
            s.assumeRelease();
            s.assumeProperty(g->getVar(), !(g->isFecInv()));
            result = s.assumpSolve();
            cout << "\r                                   ";
            bool inv = 0;
            inv = g->isFecInv() ^ tmp->isFecInv();
            cout << "\rProving " << g->getId() << " = "
                         << !g->isFecInv() << "..." << flush;
            if(!result) {
               cout << "UNSAT!!";
               boolVec.push_back(inv);
               live.push_back(tmp);
               die.push_back(g);
               for(int k = f->getSize() - 1; k >= 0; k--)
                  if(f->getGate(k) == g)
                     f->removeGate(k);
               j--;
               unsat = 1;
            } else {
               cout << "SAT!!";
               sat = 1;
               for(int k = f->getSize() - 1; k >= 0; k--)
                  if(f->getGate(k) == g)
                     f->removeGate(k);
            }
            break;
         }
      }
      if(f->getSize() == 1) {
         f->getGate(0)->clearMyFecGrp();
         f->removeGate(0);
      }
      
   }
   cout << "\r                                   \r";

   dfsSort(live, die, boolVec);
   for(unsigned j = 0; j < live.size(); j++) {
      // replace tmp with g
      CirGate *g1 = live[j];
      CirGate *tmp = die[j];
      bool inv = boolVec[j];
      cout << "Fraig: "<< g1->getId() <<" merging " 
      << (inv ? "!" : "") << tmp->getId() << "..." << endl;
      tmp->fraigMerge(g1, inv); 
      tmp->finfoutRemove();
      miloa[4]--; // MILO "A"
      deleteGate(tmp->getId());
   }
   n = 0;
   if(unsat) cout << "Updating by UNSAT... Total #FEC Group = " << n << endl;
   if(sat) cout << "Updating by SAT... Total #FEC Group = " << n << endl;

   clearListFecGrps();
}
void
CirMgr::fraig()
{
   //initCircuit()
   if(_listFecGrps.size() == 0)
      return;
   buildDfsList(); //setGlobalRef()
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
   prove(solver);
   // strash();
   buildDfsList();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
