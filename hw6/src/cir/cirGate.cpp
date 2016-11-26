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
   cout << "[" << getLineNo() << "] " << getTypeStr() << "  "
        << getId() << " (" << getName() << ")" << endl;
   
}
void
CirPoGate::printGate() const
{
   //[8] PO  24 !22 (22GAT$PO)
   cout << "[" << getLineNo() << "] " << getTypeStr() << "  " << getId() << " ";
   CirGate* fin = faninList[0];
   if(fin == NULL) return; // error
   if(fin->getType() == UNDEF_GATE)
      cout << "*";
   if(inv) cout << "!";
   cout << fin->getId();
   cout << " (" << getName() << ")" << endl;

}
void
CirAigGate::printGate() const
{
   //[7] AIG 22 !10 !16
   cout << "[" << getLineNo() << "] " << getTypeStr() << "  " << getId() << " ";
   CirGate* rhs0 = faninList[0];
   CirGate* rhs1 = faninList[1];
   
   if(rhs0 == NULL || rhs1 == NULL) return; //error
   if(rhs0->getType() == UNDEF_GATE)
      cout << "*";
   if(inv_rhs0) cout << "!";
   cout << rhs0->getId();
   cout << " ";
   if(rhs1->getType() == UNDEF_GATE)
      cout << "*";
   if(inv_rhs1) cout << "!";
   cout << rhs1->getId();
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
