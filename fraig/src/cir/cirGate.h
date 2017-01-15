/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

#include "cirDef.h"
#include "sat.h"
// AH
#include "fecGrp.h"

using namespace std;
#define NEG 0x1

#define aiger_sign(l) \
  (((unsigned)(l))&1)

#define aiger_var2lit(i) \
  (((unsigned)(i)) << 1)

#define aiger_lit2var(l) \
  (((unsigned)(l)) >> 1)

#define BIT_32 32


// TODO: Feel free to define your own classes, variables, or functions.

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------

class CirGate {
   
   public:
      CirGate(GateType _t, unsigned _id, unsigned _n)
      : type(_t), lineNo(_n), id(_id), color(0), simValue(0), _myFecGrp(0), fecInv(0) {}
      virtual ~CirGate(){};

      // Printing functions
      virtual void printGate() const = 0;
      void reportGate() const;
      void reportFanin(int level) const;
      void reportFanout(int level) const;
      // Basic access methods
      string getTypeStr() const;
      GateType getType() const { return type; }
      unsigned getLineNo() const { return lineNo; }
      unsigned getId() const { return id; }
      // from homework #6
      void addInput(CirGate* g, bool inv = false) {
         if (inv) g = (CirGate*)((size_t)g + 1);
         faninList.push_back(g);
      }
      void addOutput(CirGate* g) { fanoutList.push_back(g); }
      CirGate* getInput(size_t i) const {
         if (i >= faninList.size()) return 0;
         return (CirGate*)(((size_t)faninList[i]) & ~size_t(NEG));
      }
      size_t getInputWithInv(size_t i) const {
         if (i >= faninList.size()) return 0;
         return (size_t)faninList[i];
      }      
      CirGate* getOutput(size_t i) const {
         if (i >= fanoutList.size()) return 0;
         return fanoutList[i];
      }
      bool isInv(size_t i) const { return ((size_t)faninList[i] & NEG); }
      bool isAig() const { return type == AIG_GATE; }
      size_t getfinSize(){ return faninList.size(); }
      size_t getfoutSize(){ return fanoutList.size(); }
      void setGlobalRef() const { globalRef++; }
      void resetGlobalRef() const { globalRef = 0; }
      bool isGlobalRef() const { return color == globalRef; }
      void setToGlobalRef() const { color = globalRef; }
      void faninDfsVisit(int, bool) const;
      void fanoutDfsVisit(int, bool) const;
      void printNetDFS() const;
      string unsToStr(unsigned n) const {
         stringstream ss;
         ss << n;
         return ss.str();
      };
      static bool orderSort (CirGate* i,CirGate* j) { return (i->getId() < j->getId()); }
      // sweep()
      void runColorDFS(GateList&);
      void finfoutRemove();
      bool removefinLink(CirGate*);
      bool removefoutLink(CirGate*);
      void removeInput(size_t i) {
         if(i >= faninList.size()) return;
         faninList[i] = 0;
         faninList.erase(faninList.begin() + i);
      }
      void removeOutput(size_t i) {
         if(i >= fanoutList.size()) return;
         fanoutList[i] = 0;
         fanoutList.erase(fanoutList.begin() + i);
      }
      // opt()
      void optMerge(CirGate*, bool&, const size_t&);
      void insertInput(CirGate* g, size_t i, bool& is_inv) {
         if (is_inv) g = (CirGate*)((size_t)g + 1);
         faninList.insert(faninList.begin() + i, 1, g); // 1 means one element
      }
      void insertOutput(CirGate* g, size_t i) {
         fanoutList.insert(fanoutList.begin() + i, 1, g);
      }
      void sortfoutList() {
         std::sort (fanoutList.begin(), fanoutList.end(), orderSort);
      }
      // strash()
      void strashfoutMerge(CirGate*);
      // file sim()
      void setSimValue(unsigned v) { simValue = v; }
      unsigned getSimValue() { return simValue; }
      string getSimValueStr() const {
         string str = "";
         unsigned val = simValue;
         for(unsigned i = 0; i < BIT_32; i++) {
            int new_bit = val & 1;
            char str_bit = new_bit + '0';
            if(i % 4 == 0 && i != 0)
               str += "_";
            str += str_bit;
            val = val >> 1;
         }
         return str;
      }
      string get1BitSimValueChar(unsigned& index) const {
         if(index >= BIT_32) return ""; //error

         unsigned val = simValue;
         string str = "";
         val = val >> index;
         int new_bit = val & 1;
         char c = new_bit + '0';
         str += c;
         return str;
      }
      string getFecStr() const {
         string str = "";
         if(_myFecGrp == 0) {
            return str; //e.g. CONST_GATE
         }
         unsigned s = _myFecGrp->getSize();
         for(unsigned i = 0; i < s; i++) {
            if(_myFecGrp->getGate(i) != this) {
               CirGate* g = _myFecGrp->getGate(i);
               bool inv = _myFecGrp->isInv(i) ^ isFecInv(); // exclusive OR
               str += " ";
               if(inv)
                  str += "!";
               str += unsToStr(g->getId());
            }
         }
         return str;
      }
      void resetSimValue() { simValue = 0; }
      void setMyFecGrp(FecGrp* grp, bool& is_inv) { 
         fecInv = is_inv;
         _myFecGrp = grp;
      }
      bool isFecInv() const { return fecInv; }
      void clearMyFecGrp() {_myFecGrp = NULL; fecInv = 0;}

      static unsigned index;
      static unsigned globalRef;

   protected:
      GateType type;
      unsigned lineNo;
      unsigned id;
      mutable unsigned color;
      unsigned simValue; // 32bit, 00100010011 etc
      FecGrp* _myFecGrp;
      bool fecInv;
      GateList faninList;
      GateList fanoutList;
};
class CirPiGate : public CirGate {

   public:
      CirPiGate(unsigned _id, unsigned _n, unsigned ind)
         : CirGate(PI_GATE, _id, _n), name(""), pi_index(ind) {}
      virtual ~CirPiGate() {}

      void printGate() const {
         //[9] PI  7 (7GAT)
         cout << "[" << index++ << "] " << setw(4) << left << getTypeStr()
            << getId();
         if(!getName().empty())
            cout << " (" << getName() << ")" << endl;
         else
            cout << endl;
      }
      void setName(string str) { if(name.empty()) name = str; }
      string getName() const { return name; } 
      unsigned getPiIndex() { return pi_index; }
   
   protected:
      string name;
      const unsigned pi_index;
};

class CirPoGate : public CirGate {

   public:
      CirPoGate(unsigned _id, unsigned _n)
         : CirGate(PO_GATE, _id, _n), name("") {}
      virtual ~CirPoGate() {}

      void printGate() const {
         //[8] PO  24 !22 (22GAT$PO)
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
      }
      void setName(string str) { if(name.empty()) name = str; }
      string getName() const { return name; } 
   
   protected:
      string name;
      //CirGate* fanin;
};

class CirAigGate : public CirGate {

   public:
      CirAigGate(unsigned _id, unsigned _n) 
         : CirGate(AIG_GATE, _id, _n) {}
      virtual ~CirAigGate() {}

      void printGate() const 
      {
         //[7] AIG 22 !10 !16
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
      }
   protected:
      //GateList faninList;
      // Aig has no name
};

class CirUndefGate : public CirGate {

   public:
      CirUndefGate(unsigned _id, unsigned _n)
         : CirGate(UNDEF_GATE, _id, _n) {}
      virtual ~CirUndefGate() {}

      void printGate() const {};
   
   protected:
};

class CirConstGate : public CirGate {

   public:
      CirConstGate(unsigned _n)
         : CirGate(CONST_GATE, 0, _n) {}
      virtual ~CirConstGate() {}

      void printGate() const
      {
         //[1] CONST0
         cout << "[" << index++ << "] " << getTypeStr();
         cout << getId() << endl;
      }

   protected:
};

#endif // CIR_GATE_H
