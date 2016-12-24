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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

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
   // fraig> cirg 1
   // ==================================================
   // = PI(1)"a", line 2                               =
   // = FECs:                                          =
   // = Value: 0110_0100_1111_0110_1101_0111_0001_0110 =
   // ==================================================
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
   cout << setw(49) << left << ss.str() << "=" << endl;
   ss.str(""); //clear ss
   cout << "= Value: " << getLastValueStr() << " =" << endl; 
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
      cout << endl;
      if(size != 0) setToGlobalRef();
      for(unsigned i = 0; i < size; i++) {
         index++;
         CirGate* g = getOutput(i);
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
void
CirGate::runColorDFS(GateList& _dfsList)
{
   if(isGlobalRef()) return;
   setToGlobalRef();
   for(size_t i = 0; i < faninList.size(); i++)
      getInput(i)->runColorDFS(_dfsList);
   _dfsList.push_back(this);
}
bool
CirGate::removefinLink(CirGate *fin)
{
   size_t fout_size = fin->fanoutList.size();
   for(int i = fout_size - 1; i >= 0; i--) {
      CirGate* me = fin->getOutput(i);
      if(me == this) {
         fin->removeOutput(i);
         return true;
      }
   }
   return false;
}
bool
CirGate::removefoutLink(CirGate *fout)
{
   size_t fin_size = fout->faninList.size();
   for(int i = fin_size - 1; i >= 0; i--) {
      CirGate* me = fout->getInput(i);
      if(me == this) {
         fout->removeInput(i);
         return true;
      }
   }
   return false;
}
void
CirGate::finfoutRemove() //clear all fin fout
{
   size_t fin_size = faninList.size();
   size_t fout_size = fanoutList.size();
   // gate is UNDEF, AIG
   for(int i = fin_size - 1; i >= 0; i--) {
      CirGate *fin = getInput(i);
      if( removefinLink(fin) )
         removeInput(i);
   }
   for(int i = fout_size - 1; i >= 0; i--) {
      CirGate *fout = getOutput(i);
      if( removefoutLink(fout) )
         removeOutput(i);
   }
}
//opt
void
CirGate::optMerge(CirGate* tmp, bool& is_inv, const size_t& aout)
{
   const bool original_inv = is_inv;
   for(size_t i = 0; i < getfoutSize(); i++) {
      is_inv = original_inv;
      CirGate *fout = getOutput(i);
      size_t k = 0;
      for(; k < fout->getfinSize(); k++) {
         CirGate* me = fout->getInput(k);
         if(me == this) {
            if(fout->isInv(k)) 
               is_inv = !is_inv;
            tmp->insertOutput(fout, aout + i);
            fout->insertInput(tmp, k, is_inv);
            break;
         }
      }
   }
}
//strash
void
CirGate::strashfoutMerge(CirGate* g)
{
   bool is_inv = 0;
   for(size_t i = 0; i < getfoutSize(); i++) {
      CirGate *fout = getOutput(i);
      size_t k = 0;
      for(; k < fout->getfinSize(); k++) {
         CirGate* me = fout->getInput(k);
         if(me == this) {
            is_inv = fout->isInv(k);
            g->addOutput(fout);
            fout->insertInput(g, k, is_inv);
            break;
         }
      }
   }
}