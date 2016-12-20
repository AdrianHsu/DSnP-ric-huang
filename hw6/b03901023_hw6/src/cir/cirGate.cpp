/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::index = 0;
unsigned CirGate::globalRef = 0;

string 
CirGate::getTypeStr() const 
{
   if(type == PI_GATE)
      return "PI";
   else if(type == PO_GATE)
      return "PO";
   else if(type == AIG_GATE)
      return "AIG";
   else if(type == CONST_GATE)
      return "CONST";
   else if(type == UNDEF_GATE)
      return "UNDEF";
   return "UNDEF";
}
void
CirGate::reportGate() const
{
   // e.g.
   //==================================================
   //= PO(25)”23GAT$PO”, line 9                       =
   //==================================================
   cout << "==================================================\n";
   stringstream ss;
   ss << "= " + getTypeStr() << '(' << getId() << ")";
   if(type == PI_GATE) {
      const CirPiGate* g = (CirPiGate*)this;
      if(g->getName() != "")ss << "\"" << g->getName() << "\"";
   } else if(type == PO_GATE) {
      const CirPoGate* g = (CirPoGate*)this;
      if(g->getName() != "")ss << "\"" << g->getName() << "\"";
   }
   ss << ", line " << getLineNo();
   cout << setw(49) << left << ss.str() << "=\n";
   cout << "==================================================\n";
}
void
CirGate::faninDfsVisit(int l, bool inv) const
{
   if(l == -1) return;
   for (unsigned i = 0; i < index; ++i)   cout << "  ";
   if(inv) cout << "!";
   cout << getTypeStr() << " " << id;
   unsigned size = faninList.size();
   if(l == 0) cout << endl;
   else if( isGlobalRef() ) cout << " (*)" << endl;
   else  { 
      cout << endl;
      if(size != 0) setToGlobalRef();
      for(unsigned i = 0; i < size; i++) {
         index++;
         getInput(i)->faninDfsVisit(l - 1, isInv(i));
      }
   }
   index--;
}

void
CirGate::reportFanin(int level) const
{ 
   assert (level >= 0);
   setGlobalRef();
   faninDfsVisit(level, 0);
   index = 0;
}
void
CirGate::fanoutDfsVisit(int l, bool inv) const
{
   if(l == -1) return;
   for (unsigned i = 0; i < index; ++i)   cout << "  ";
   if(inv) cout << "!";
   cout << getTypeStr() << " " << id;
   unsigned size = fanoutList.size();
   if(l == 0) cout << endl;
   else if(isGlobalRef()) cout << " (*)" << endl;
   else {
      
      vector<CirGate*> order;
      for(unsigned i = 0; i < size; i++){ 
         CirGate* g = getOutput(i);
         order.push_back(g);
      }
      std::sort (order.begin(), order.end(), orderSort);

      cout << endl;
      if(size != 0) setToGlobalRef();
      for(unsigned i = 0; i < size; i++) {
         index++;
         CirGate* g = order[i];
         bool myinv = 0;
         for(unsigned j = 0; j < g->getfinSize(); j++) {
            CirGate* g2 = g->getInput(j);
            if(g2 == 0) break;
            else if(g2 == this) {
               myinv = g->isInv(j);
               g->fanoutDfsVisit(l - 1, myinv);
               break; //found
            }
         }
      }
   }
   index--;
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   fanoutDfsVisit(level, 0);
   index = 0;
}
void
CirGate::printNetDFS() const
{
   if(isGlobalRef()) return;
   setToGlobalRef();

   for(size_t i = 0; i < faninList.size(); i++) 
      getInput(i)->printNetDFS();
   printGate();
}