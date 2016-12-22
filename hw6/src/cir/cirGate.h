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
#include <algorithm>
#include <sstream>

#include "cirDef.h"

using namespace std;

#define NEG 0x1

#define aiger_sign(l) \
  (((unsigned)(l))&1)

#define aiger_var2lit(i) \
  (((unsigned)(i)) << 1)

#define aiger_lit2var(l) \
  (((unsigned)(l)) >> 1)


class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate {
   
   public:
      CirGate(GateType _t, unsigned _id, unsigned _n): type(_t), lineNo(_n), id(_id), color(0) {}
      virtual ~CirGate() {}

      // Printing functions
      virtual void printGate() const = 0;
      void reportGate() const;
      void reportFanin(int level) const;
      void reportFanout(int level) const;

      void addInput(CirGate* g, bool inv = false) {
         if (inv) g = (CirGate*)((size_t)g + 1);
         faninList.push_back(g);
      }
      void addOutput(CirGate* g) { fanoutList.push_back(g); }
      CirGate* getInput(size_t i) const {
         if (i >= faninList.size()) return 0;
         return (CirGate*)(((size_t)faninList[i]) & ~size_t(NEG));
      }
      CirGate* getOutput(size_t i) const {
         if (i >= fanoutList.size()) return 0;
         return fanoutList[i];
      }
      bool isInv(size_t i) const { return ((size_t)faninList[i] & NEG); }
      bool isAig() const { return type == AIG_GATE; }
      // Basic access methods
      string getTypeStr() const;
      GateType getType() const {return type;}
      unsigned getLineNo() const { return lineNo; }
      unsigned getId() const {return id; }
      size_t getfinSize(){ return faninList.size(); }
      size_t getfoutSize(){ return fanoutList.size(); }
      void setGlobalRef() const { globalRef++; }
      void resetGlobalRef() const { globalRef = 0; }
      bool isGlobalRef() const { return color == globalRef; }
      void setToGlobalRef() const {color = globalRef; }
      void faninDfsVisit(int, bool) const;
      void fanoutDfsVisit(int, bool) const;
      void printNetDFS() const;
      string unsToStr(unsigned n) const {
         stringstream ss;
         ss << n;
         return ss.str();
      };
      static bool orderSort (CirGate* i,CirGate* j) { return (i->getId() < j->getId()); }
      void sortfoutList() {
         std::sort (fanoutList.begin(), fanoutList.end(), orderSort);
      }
      static unsigned index;
      static unsigned globalRef;
      
   protected:
      GateType type;
      unsigned lineNo;
      unsigned id;
      mutable unsigned color;
      GateList faninList;
      GateList fanoutList;
};
class CirPiGate : public CirGate {

   public:
      CirPiGate(unsigned _id, unsigned _n)
         : CirGate(PI_GATE, _id, _n), name("") {}
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
   
   protected:
      string name;
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
