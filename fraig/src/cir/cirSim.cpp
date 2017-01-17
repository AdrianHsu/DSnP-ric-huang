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
#include <cstdlib> //srand
#include <cmath>
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

#define MY_RAND_MAX INT_MAX // 4294967295 (2^32-1), type unsigned long int
#define MAGIC_NUMBER 3.5
/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned MAX_FAILS = 0;
static unsigned CURRENT_FAILS = 0;
static bool IS_RANDSIM = 0;
static bool IS_FIRST_SIM = 1;
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   srand(time(0));
   IS_RANDSIM = 1;
   IS_FIRST_SIM = 1;

   unsigned _i = miloa[1];
   // unsigned _a = miloa[4];
   // double aig = (double) _a;
   double in = (double)_i;

   // MAX_FAILS = BIT_32 * log(aig);
   MAX_FAILS = (size_t)(MAGIC_NUMBER * sqrt(in));
   if(MAX_FAILS == 0) MAX_FAILS++;
   cout << "MAX_FAILS = " << MAX_FAILS << endl;
   startRandSim();
   
   IS_RANDSIM = 0;
   CURRENT_FAILS = 0;
}
void
CirMgr::startRandSim()
{
   unsigned _i = miloa[1], _o = miloa[3], _m = miloa[0];
   unsigned a = 0;
   buildDfsList();
   fecGrpsInit(); //first time build map
   vector<unsigned> _32bitvec;
   
   for(; ; a++) {
      if(CURRENT_FAILS >= MAX_FAILS)
         break;
      _32bitvec.clear();
      for(unsigned i = 0; i < _i; i++) {
         unsigned val = 2 * rnGen(MY_RAND_MAX);
         _32bitvec.push_back(val);
      }
      unsigned r = 0; // must be 0
      simulate(r , _32bitvec);
      if(a % 2 == 0) {
         fecGrpsIdentify(_listFecGrps, _tmpListFecGrps);
         cout << "\rTotal #FEC Group = " << _tmpListFecGrps.size() << flush;
      } else {
         fecGrpsIdentify(_tmpListFecGrps, _listFecGrps);
         cout << "\rTotal #FEC Group = " << _listFecGrps.size() << flush;
      }
      cout << "\r                              ";
      if(_simLog) { //writeLog
         for(unsigned i = a * BIT_32; i < BIT_32 * (a + 1); i ++) {
            for(unsigned j = 0; j < _m + 1; j ++) { // PI
               CirGate* g = getGate(j);
               if(g == 0) continue;
               if(g->getType() == PI_GATE) {
                  unsigned index = BIT_32 - ( i % BIT_32 ) - 1;
                  string c = g->get1BitSimValueChar(index);
                  *_simLog << c;
               }
            }
            *_simLog << " ";
            for(unsigned j = 0; j < _o; j ++) { // PO
               CirGate* g = getGate(_m + j + 1);
               if(g == 0) continue;
               unsigned index = BIT_32 - ( i % BIT_32 ) - 1;
               string c = g->get1BitSimValueChar(index);
               *_simLog << c;
            }
            *_simLog << endl;
         }
      }
   }
   if(a % 2 == 1) {
      _listFecGrps.clear();
      _listFecGrps = _tmpListFecGrps;
      _tmpListFecGrps.clear();
   } //else do nothing
   unsigned mod = 0;
   cout << "\r" << a * BIT_32 - mod << " patterns simulated." << endl;

}
void
CirMgr::fileSim(ifstream& patternFile)
{
   if(!patternFile.is_open()) {
      return;
   }
   IS_FIRST_SIM = 1;
   string str;
   unsigned _i = miloa[1];
   vector<string> inputs;
   while(patternFile >> str) {
      if(str.size() != _i && str.size() != 0) {
         cerr << "\nError: Pattern(" << str <<") length(" << str.size() 
              << ") does not match the number of inputs("
              << _i << ") in a circuit!!" << endl;
         cout << "\r" << "0 patterns simulated." << endl;
         return;
      }  
      inputs.push_back(str);
   }
   unsigned inputSize = inputs.size();
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
               cerr << "\nError: Pattern(" << inputs[k + i * BIT_32] 
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
   startSim(div, mod, _32bitvec, inputs, inputSize);
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::startSim(unsigned& div, unsigned& mod, vector<unsigned>& _32bitvec, vector<string>& inputs, unsigned& inputSize)
{
   // start sim
   unsigned a = 0;
   buildDfsList();
   fecGrpsInit(); //first time build map
   for( ; a < div; a++) {
      simulate(a, _32bitvec);
      if(a % 2 == 0) {
         fecGrpsIdentify(_listFecGrps, _tmpListFecGrps);
         cout << "\rTotal #FEC Group = " << _tmpListFecGrps.size();
      } else {
         fecGrpsIdentify(_tmpListFecGrps, _listFecGrps);
         cout << "\rTotal #FEC Group = " << _listFecGrps.size();
      }
      if(_simLog) { //writeLog
         unsigned _o = miloa[3], _m = miloa[0];
         for(unsigned i = a * BIT_32; i < BIT_32 * (a + 1); i ++) {
            if(i < inputSize) {
               *_simLog << inputs[i] << " ";
               for(unsigned j = 0; j < _o; j ++) {
                  CirGate* g = getGate(_m + j + 1);
                  unsigned index = BIT_32 - ( i % BIT_32 ) - 1;
                  string c = g->get1BitSimValueChar(index);
                  *_simLog << c;
               }
               *_simLog << endl;
            }
         }    
      }
   }
   if(a % 2 == 1) {
      _listFecGrps.clear();
      _listFecGrps = _tmpListFecGrps;
      _tmpListFecGrps.clear();
   } //else do nothing
   cout << "\r" << a * BIT_32 - mod << " patterns simulated." << endl;

}

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
         unsigned pattern = _32bitvec[round * _i + ins_index];
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
   _tmpListFecGrps.clear();
   _listFecGrps.clear();

   FecGrp *fecGrp = new FecGrp(0);
   bool fal = false;
   CirGate* g = getGate(0);
   fecGrp->addGate(g, fal);
   g->setMyFecGrp(fecGrp);
   g->setMyFecInv(fal);
   for(unsigned i = 0; i < _dfsList.size(); i++) {
      g = _dfsList[i];
      if(g->getType() == AIG_GATE) {
         fecGrp->addGate(g, fal);
         g->setMyFecGrp(fecGrp);
         g->setMyFecInv(fal);
      }
   }
   //Add this FEC group into fecGrps (list of FEC groups)
   GateList list = fecGrp->getList();
   //don't need to sort for init
   _listFecGrps.push_back(fecGrp);
}

void
CirMgr::fecGrpsIdentify(ListFecGrps& fecGrps, ListFecGrps& tmpGrps)
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

   unsigned listSize = fecGrps.size(); // listSize = 1
   // unsigned _a = miloa[4];
   FecGrp* fecGrp = 0;
   for(unsigned i = 0; i < listSize; i ++) {
      fecGrp = fecGrps[i];
      unsigned grpSize = fecGrp->getSize();
      FecMap newFecMap( getHashSize(grpSize + 1) );
      for(unsigned j = 0; j < grpSize; j++) { // slow!
         CirGate* gate = fecGrp->getGate(j);
         FecGrp* grp = NULL;
         FecHashKey key(gate->getSimValue());
         bool inv = 0;
         bool isChecked = 0;
         isChecked = newFecMap.check(key, grp, inv);
         
         if(isChecked) {

            grp->addGate(gate, inv);
            gate->setMyFecGrp(grp);
            if(IS_FIRST_SIM) gate->setMyFecInv(inv);
         } else
            createNewGroup(newFecMap, gate, inv);
      }
      // cerr << i << endl;
      collectValidFecGrp(newFecMap, i, tmpGrps);
   }
   if(IS_RANDSIM && _listFecGrps.size() == _tmpListFecGrps.size())
      CURRENT_FAILS++;

   fecGrps.clear();
   sortListFecGrps(tmpGrps);
   if(IS_FIRST_SIM) {
      IS_FIRST_SIM = 0;
   }
}
void
CirMgr::createNewGroup(FecMap& newFecMap, CirGate* g, bool& inv)
{
   FecGrp *fecGrp = new FecGrp(g->getSimValue());
   fecGrp->addGate(g, inv);
   g->setMyFecGrp(fecGrp);
   if(IS_FIRST_SIM) g->setMyFecInv(inv);
   FecHashKey key(g->getSimValue());
   newFecMap.insert(key, fecGrp);
}
void quickSort(FecGrp* grp, int left, int right) {
   int i = left, j = right;
   unsigned mid = (left + right) / 2;
   CirGate* pivot = grp->getGate(mid);

   /* partition */
   while (i <= j) {
         while (grp->getGate(i)->getId() < pivot->getId())
               i++;
         while (grp->getGate(j)->getId() > pivot->getId())
               j--;
         if (i <= j) {
            CirGate* tmp = grp->getList()[i];               
            grp->setGate(grp->getList()[j], i);
            grp->setGate(tmp, j);
            i++;
            j--;
         }
   }
   /* recursion */
   if (left < j)
         quickSort(grp, left, j);
   if (i < right)
         quickSort(grp, i, right);
}
void
CirMgr::collectValidFecGrp(FecMap& newFecMap, unsigned& i, ListFecGrps& tmpGrps) {
   FecMap::iterator it = newFecMap.begin();
   for(; it != newFecMap.end(); it++) {
      FecGrp* tmp = (*it).second;
      quickSort(tmp, 0, tmp->getSize() - 1);
      if(tmp->getSize() > 1) 
         tmpGrps.push_back(tmp); // valid
      else {
         tmp->getGate(0)->clearMyFecGrp(); // invalid
      }
   }
}