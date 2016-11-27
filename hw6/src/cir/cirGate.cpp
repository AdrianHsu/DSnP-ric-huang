/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

unsigned CirGate::index = 0;
string unsToStr(unsigned n) {
   stringstream ss;
   ss << n;
   return ss.str();
}

/**************************************/
/*   class CirGate member functions   */
/**************************************/
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
   if(l == -1) { index--; return; };
   for (unsigned i = 0; i < index; ++i)   cout << "  ";
   if(inv) cout << "!";

   if(faninList.size() == 0)
      cout << getTypeStr() << " " << getId() << endl;
   else
      cout << getTypeStr() << " " << getId();
   bool star = 1;

   for(int i = 0; i < faninList.size(); i++) { //size must <= 2
      if(getInput(i)->color == 0 || (getInput(i)->color == 1 && color == 0)) {
         if(star) {
            cout << endl;
            star = 0;
         }
         index++;
         getInput(i)->faninDfsVisit(l - 1, isInv(i));
      }
   }
   if(star && faninList.size() != 0) {
      cout << " (*)" << endl;
   }
   index--;
   setColor(1);
}

void
CirGate::reportFanin(int level) const
{ 
   assert (level >= 0);
   faninDfsVisit(level, 0);
   cirMgr->resetColors();
   index = 0;
}
void
CirGate::fanoutDfsVisit(int l, bool inv) const
{
   if(l == -1) { index--; return; };
   for (unsigned i = 0; i < index; ++i)   cout << "  ";
   if(inv) cout << "!";

   if(fanoutList.size() == 0)
      cout << getTypeStr() << " " << getId() << endl;
   else
      cout << getTypeStr() << " " << getId();
   bool star = 1;

   for(int i = 0; i < fanoutList.size(); i++) {
      if(getOutput(i)->color == 0 || (getOutput(i)->color == 1 && color == 0)) {
         if(star) {
            cout << endl;
            star = 0;
         }
         index++;
         bool myinv = 0;
         unsigned j = 0;
         CirGate* g = getOutput(i);
         while (true) {
            CirGate* g2 = g->getInput(j);
            if (g2 == 0)   break;
            if (this == g2) {
               myinv = g->isInv(j);
               break;
            }
            ++j;
         }

         g->fanoutDfsVisit(l - 1, myinv);
      }
   }
   if(star && fanoutList.size() != 0) {
      cout << " (*)" << endl;
   }
   index--;
   setColor(1);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   fanoutDfsVisit(level, 0);
   cirMgr->resetColors();
   index = 0;
}

void
CirPiGate::printGate() const
{
   //[9] PI  7 (7GAT)
   //for(int i = 0; i < faninList.size(); i++)
   //   faninList[i]->printGate();
   if(color) return;
   cout << "[" << index++ << "] " << setw(4) << left << getTypeStr()
      << getId();
   if(!getName().empty())
      cout << " (" << getName() << ")" << endl;
   else
      cout << endl;
   setColor(1);
}
void
CirPoGate::printGate() const
{
   //[8] PO  24 !22 (22GAT$PO)
   if(color) return;
   for(int i = 0; i < faninList.size(); i++)
      getInput(i)->printGate();
   cout << "[" << index++ << "] " << setw(4) << left << getTypeStr() << getId() << " ";
   CirGate* fin = getInput(0);
   string str;
   if(fin->getType() == UNDEF_GATE)
      str += "*";
   if(isInv(0)) str += "!";
   str += unsToStr( fin->getId() );
   cout << str;

   if(!getName().empty())
      cout << " (" << getName() << ")" << endl;
   else
      cout << endl;
   setColor(1);
}
void
CirAigGate::printGate() const
{
   //[7] AIG 22 !10 !16
   if(color) return;
   for(int i = 0; i < faninList.size(); i++)
      getInput(i)->printGate();
   cout << "[" << index++ << "] " << setw(4) << left << getTypeStr() << getId() << " ";
   CirGate* rhs0 = getInput(0);
   CirGate* rhs1 = getInput(1);
    
   if(rhs0 == NULL || rhs1 == NULL) return; //error
   string str;
   if(rhs0->getType() == UNDEF_GATE)
      str += "*";
   if(isInv(0)) str += "!";
   str += unsToStr( rhs0->getId() );
   cout << str << " ";
   str.clear();
   if(rhs1->getType() == UNDEF_GATE)
      str += "*";
   if(isInv(1)) str += "!";
   str += unsToStr( rhs1->getId() );
   cout << str;
   cout << endl;
   setColor(1);
}
void
CirConstGate::printGate() const
{
   //[1] CONST0
   //for(int i = 0; i < faninList.size(); i++)
   //   faninList[i]->printGate();
   if(color) return;
   cout << "[" << index++ << "] " << getTypeStr();
   cout << getId() << endl;
   setColor(1);
}
void
CirUndefGate::printGate() const
{
   // will not be called
}
