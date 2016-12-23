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
   vector<string> ins;
   while(patternFile >> str) {
      if(str.size() != _i && str.size() != 0) {
         cerr << "Error: Pattern(" << str <<") length(" << str.size() 
              << ") does not match the number of inputs("
              << _i << ") in a circuit!!" << endl;
         cout << "0 patterns simulated." << endl;
         return;
      }  
      ins.push_back(str);
   }
   unsigned div = 0, mod = ins.size() % BIT_32; // div: num of round, e.g. 64 is 2 rounds
   if(mod != 0) {
      div = ins.size() / BIT_32;
      mod = BIT_32 * (div + 1) - ins.size();
   }
   for(unsigned j = 0; j < mod; j ++) {
      char zero_str [_i];
      memset(zero_str, '0', sizeof(zero_str));
      zero_str[_i] = '\0';
      ins.push_back(zero_str);
   }
   div = ins.size() / BIT_32;
   vector<size_t> _32bitvec;

   for(unsigned i = 0; i < div; i++) {
      for(unsigned j = 0; j < _i; j++) {
         size_t pattern = 0;
         for(unsigned k = 0; k < BIT_32; k++) {
            int new_bit = ins[k + i * BIT_32][j] - '0';
            if(new_bit != 0 && new_bit != 1) {
               cerr << "Error: Pattern(" << ins[k + i * BIT_32] 
                    << ") contains a non-0/1 character('" 
                    << ins[k + i * BIT_32][j] << "')." << endl;

               cout << "0 patterns simulated." << endl;
               return;
            } 
            pattern = (pattern << 1) + new_bit; // key!
         }
         _32bitvec.push_back(pattern);
      }
   }
   cout << div * BIT_32 - mod << " patterns simulated." << endl;
   // start sim
   // buildDfsList();
   // unsigned ins_count = 0;
   // for(unsigned a = 0; a < div; a++) {
   //    for(unsigned i = 0; i < _dfsList.size(); i++) {
   //       CirGate* g = _dfsList[i];
   //       if(g->getType() == PI_GATE) {
   //          size_t pattern = _32bitvec[a * _i + ins_count];
   //          ins_count++;
   //          if(ins_count == _i)
   //             ins_count = 0;
   //          g->setLastValue(pattern);
   //       } else if(g->getType() == AIG_GATE) {
   //          CirGate* lhs0 = g->getInput(0), lhs1 = g->getInput(1);
   //          size_t v0 = lhs0->getLastValue(), v1 = lhs1->getLastValue();
   //          g->setLastValue(v0 & v1);
   //       }
   //    }
   // }
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
