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
      CirPiGate(GateType _t = PI_GATE, unsigned _lit = 0)
         : CirGate(_t), lit(_lit), name("") {}
      ~CirPiGate() {}

      void printGate() const;
      void setName(string str) { if(name.empty()) name = str; }
   
   protected:
      unsigned lit;
      string name;
};

class CirPoGate : public CirGate {

   public:
      CirPoGate(GateType _t = PO_GATE, unsigned _lit = 0)
         : CirGate(_t), lit(_lit), name("") {}
      ~CirPoGate() {}

      void printGate() const;
      void setName(string str) { if(name.empty()) name = str; }
   
   protected:
      unsigned lit;
      string name;
      CirGate* fanin;
};

class CirAigGate : public CirGate {

   public:
      CirAigGate(GateType _t = AIG_GATE, 
                  unsigned _l = 0, unsigned _r0 = 0, unsigned _r1 = 0) 
         : CirGate(_t), lhs(_l), rhs0(_r0), rhs1(_r1) {}
      ~CirAigGate() {}

      void printGate() const;
   
   protected:
      unsigned lhs;
      unsigned rhs0;
      unsigned rhs1;
      GateList faninList;
      // Aig has no name
};

class CirUndefGate : public CirGate {

   public:
      CirUndefGate(GateType _t = UNDEF_GATE, unsigned _lit = 0)
         : CirGate(_t), lit(_lit) {}
      ~CirUndefGate() {}

      void printGate() const;
   
   protected:
      unsigned lit;
};

class CirConstGate : public CirGate {

   public:
      CirConstGate(GateType _t = CONST_GATE, unsigned _lit = 0)
         : CirGate(_t), lit(_lit) {}
      ~CirConstGate() {}

      void printGate() const;
   
   protected:
      unsigned lit;
};
#endif // CIR_GATE_H
