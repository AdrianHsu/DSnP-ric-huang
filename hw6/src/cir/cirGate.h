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
      CirGate(GateType _t): type(_t) {}
      virtual ~CirGate() {}

      // Basic access methods
      string getTypeStr() const;
      unsigned getLineNo() const;

      // Printing functions
      virtual void printGate() const = 0;
      void reportGate() const;
      void reportFanin(int level) const;
      void reportFanout(int level) const;

   protected:
      GateType type;
};
class CirPiGate : public CirGate {

   public:
      CirPiGate(unsigned _lit = 0)
         : CirGate(PI_GATE), lit(_lit), name("") {}
      virtual ~CirPiGate() {}

      void printGate() const;
      void setName(string str) { if(name.empty()) name = str; }
      string getName() { return name; } 
   
   protected:
      unsigned lit;
      string name;
};

class CirPoGate : public CirGate {

   public:
      CirPoGate(unsigned _id = 0, unsigned _f = 0)
         : CirGate(PO_GATE), id(_id), fanin(_f),name("") {}
      virtual ~CirPoGate() {}

      void printGate() const;
      void setName(string str) { if(name.empty()) name = str; }
      string getName() { return name; } 
   protected:
      unsigned id;
      unsigned fanin;
      string name;
      //CirGate* fanin;
};

class CirAigGate : public CirGate {

   public:
      CirAigGate(unsigned _l = 0, unsigned _r0 = 0, unsigned _r1 = 0) 
         : CirGate(AIG_GATE), lhs(_l), rhs0(_r0), rhs1(_r1) {}
      virtual ~CirAigGate() {}

      void printGate() const;
   
   protected:
      unsigned lhs;
      unsigned rhs0;
      unsigned rhs1;
      //GateList faninList;
      // Aig has no name
};

class CirUndefGate : public CirGate {

   public:
      CirUndefGate(unsigned _lit = 0)
         : CirGate(UNDEF_GATE), lit(_lit) {}
      virtual ~CirUndefGate() {}

      void printGate() const;
   
   protected:
      unsigned lit;
};

class CirConstGate : public CirGate {

   public:
      CirConstGate()
         : CirGate(CONST_GATE), lit(0) {}
      virtual ~CirConstGate() {}

      void printGate() const;
   
   protected:
      unsigned lit;
};
#endif // CIR_GATE_H
