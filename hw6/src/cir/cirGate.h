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
#include "cirDef.h"

using namespace std;


class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate {
   
   public:
      CirGate(GateType _t, unsigned _id, unsigned _n): type(_t), lineNo(_n), id(_id), color(0) {}
      virtual ~CirGate() {}

      // Basic access methods
      string getTypeStr() const;
      GateType getType() const {return type;}
      unsigned getLineNo() const { return lineNo; }
      unsigned getId() const {return id; }
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

      size_t getfinSize(){ return faninList.size(); }
      size_t getfoutSize(){ return fanoutList.size(); }
      void setColor(int c) const {color = c;}
      int getColor() {return color;}
      void faninDfsVisit(int, bool) const;
      void fanoutDfsVisit(int, bool) const;

      static unsigned index;

   protected:
      GateType type;
      unsigned lineNo;
      unsigned id;
      mutable int color;
      GateList faninList;
      GateList fanoutList;
};
class CirPiGate : public CirGate {

   public:
      CirPiGate(unsigned _id, unsigned _n)
         : CirGate(PI_GATE, _id, _n), name("") {}
      virtual ~CirPiGate() {}

      void printGate() const;
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

      void printGate() const;
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

      void printGate() const;
   protected:
      //GateList faninList;
      // Aig has no name
};

class CirUndefGate : public CirGate {

   public:
      CirUndefGate(unsigned _id, unsigned _n)
         : CirGate(UNDEF_GATE, _id, _n) {}
      virtual ~CirUndefGate() {}

      void printGate() const;
   
   protected:
};

class CirConstGate : public CirGate {

   public:
      CirConstGate(unsigned _n)
         : CirGate(CONST_GATE, 0, _n) {}
      virtual ~CirConstGate() {}

      void printGate() const;
   
   protected:
};
#endif // CIR_GATE_H
