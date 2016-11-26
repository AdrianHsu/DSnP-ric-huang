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

#define aiger_false 0
#define aiger_true 1

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
      CirGate(GateType _t): type(_t) {}
      virtual ~CirGate() {}

      // Basic access methods
      string getTypeStr() const { return ""; }
      unsigned getLineNo() const { return 0; }

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
      CirPiGate(GateType _t, unsigned _lit)
         : CirGate(_t), lit(_lit), name("") {}
      ~CirPiGate() {}

      void printGate() const {}
      void setName(string str) { if(name.empty()) name = str; }
   
   protected:
      unsigned lit;
      string name;
};

class CirPoGate : public CirGate {

   public:
      CirPoGate(GateType _t, unsigned _lit)
         : CirGate(_t), lit(_lit), name("") {}
      ~CirPoGate() {}

      void printGate() const {}
      void setName(string str) { if(name.empty()) name = str; }
   
   protected:
      unsigned lit;
      string name;
};

class CirAigGate : public CirGate {

   public:
      CirAigGate(GateType _t, unsigned _l, unsigned _r0, unsigned _r1) 
         : CirGate(_t), lhs(_l), rhs0(_r0), rhs1(_r1) {}
      ~CirAigGate() {}

      void printGate() const {}
   
   protected:
      unsigned lhs;
      unsigned rhs0;
      unsigned rhs1;
      // Aig has no name
};
#endif // CIR_GATE_H
