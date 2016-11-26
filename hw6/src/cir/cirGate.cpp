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
      ss << "\"" << g->getName() << "\"";
   } else if(type == PI_GATE) {
      const CirPoGate* g = (CirPoGate*)this;
      ss << "\"" << g->getName() << "\"";
   }
   ss << ", line " << getLineNo();
   cout << setw(49) << left << ss.str() << "=\n";
   cout << "==================================================\n";
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

void
CirPiGate::printGate() const
{
   //[9] PI  7 (7GAT)
   cout << "[" << getLineNo() << "] " << setw(4) << left << getTypeStr()
      << getId() << " (" << getName() << ")" << endl;
}
void
CirPoGate::printGate() const
{
   //[8] PO  24 !22 (22GAT$PO)
   cout << "[" << getLineNo() << "] " << setw(4) << left << getTypeStr() << getId() << " ";
   CirGate* fin = faninList[0];
   if(fin == NULL) return; // error
   string str;
   if(fin->getType() == UNDEF_GATE)
      str += "*";
   if(inv) str += "!";
   str += unsToStr( fin->getId() );
   
   cout << str << " (" << getName() << ")" << endl;
}
void
CirAigGate::printGate() const
{
   //[7] AIG 22 !10 !16
   cout << "[" << getLineNo() << "] " << setw(4) << left << getTypeStr() << getId() << " ";
   CirGate* rhs0 = faninList[0];
   CirGate* rhs1 = faninList[1];
    
   if(rhs0 == NULL || rhs1 == NULL) return; //error
   string str;
   if(rhs0->getType() == UNDEF_GATE)
      str += "*";
   if(inv_rhs0) str += "!";
   str += unsToStr( rhs0->getId() );
   cout << str << " ";
   str.clear();
   if(rhs1->getType() == UNDEF_GATE)
      str += "*";
   if(inv_rhs1) str += "!";
   str += unsToStr( rhs1->getId() );
   cout << str;
   cout << endl;
}
void
CirConstGate::printGate() const
{
   //[1] CONST0
   cout << "[" << getLineNo() << "] " << getTypeStr();
   cout << getId() << endl;
}
void
CirUndefGate::printGate() const
{
   // will not be called
}
