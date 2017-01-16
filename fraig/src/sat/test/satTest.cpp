#include <iostream>
#include <vector>
#include "sat.h"

using namespace std;

class CirGate
{
public:
   CirGate(unsigned i = 0): _gid(i) {}
   ~CirGate() {}

   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }

private:
   unsigned   _gid;  // for debugging purpose...
   Var        _var;
};

// 
//[0] PI  1 (a)
//[1] PI  2 (b)
//[2] AIG 4 1 2
//[3] PI  3 (c)
//[4] AIG 5 1 3
//[5] AIG 6 !4 !5
//[6] PO  9 !6
//[7] AIG 7 !2 !3
//[8] AIG 8 !7 1
//[9] PO  10 8
//
vector<CirGate *> gateList;

void
initCircuit()
{
   // Init gateList
   gateList.push_back(new CirGate(1));  // gateList[0] 0
   gateList.push_back(new CirGate(2));  // gateList[1] 0
   gateList.push_back(new CirGate(4));  // gateList[2] 0
   gateList.push_back(new CirGate(3));  // gateList[3] 1
   gateList.push_back(new CirGate(5));  // gateList[4] 0
   gateList.push_back(new CirGate(6));  // gateList[5] 1
   gateList.push_back(new CirGate(9));  // gateList[6] 0
   gateList.push_back(new CirGate(7));  // gateList[7] 0
   gateList.push_back(new CirGate(8));  // gateList[8] 0
   gateList.push_back(new CirGate(10)); // gateList[9] 0

   // POs are not needed in this demo example
}

void
genProofModel(SatSolver& s)
{
   // Allocate and record variables; No Var ID for POs
   for (size_t i = 0, n = gateList.size(); i < n; ++i) {
      Var v = s.newVar();
      gateList[i]->setVar(v);
   }

   // Hard code the model construction here...
   // [2] AIG 4 1 2 ==> [2] = [0] & [1]
   s.addAigCNF(gateList[2]->getId(), gateList[0]->getId(), false,
               gateList[1]->getId(), false);
   // [4] AIG 5 1 3 ==> [4] = [0] & [3]
   s.addAigCNF(gateList[4]->getId(), gateList[0]->getId(), false,
               gateList[3]->getId(), false);
   // [5] AIG 6 !4 !5 ==> [5] = ![2] & ![4]
   s.addAigCNF(gateList[5]->getId(), gateList[2]->getId(), true,
               gateList[4]->getId(), true);
   // [7] AIG 7 !2 !3 ==> [7] = ![1] & ![3]
   s.addAigCNF(gateList[7]->getId(), gateList[1]->getId(), true,
               gateList[3]->getId(), true);
   // [8] AIG 8 !7 1 ==> [8] = ![7] & [0]
   s.addAigCNF(gateList[8]->getId(), gateList[7]->getId(), true,
               gateList[0]->getId(), false);
}

void reportResult(const SatSolver& solver, bool result)
{
   solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
   if (result) {
      for (size_t i = 0, n = gateList.size(); i < n; ++i)
         cout << solver.getValue(gateList[i]->getId()) << endl;
   }
}

int main()
{
   initCircuit();

   SatSolver solver;
   solver.initialize();

   //
   genProofModel(solver);

   bool result;
   // k = Solve(CirGate(5) ^ !CirGate(8))
   Var newV = solver.newVar();
   solver.addXorCNF(newV, gateList[5]->getId(), false, gateList[8]->getId(), true);
   solver.assumeRelease();  // Clear assumptions
   solver.assumeProperty(newV, true);  // k = 1
   result = solver.assumpSolve();
   reportResult(solver, result);

   cout << endl << endl << "======================" << endl;

   // k = Solve(CirGate(3) & !CirGate(7))
   newV = solver.newVar();
   solver.addAigCNF(newV, gateList[3]->getId(), false, gateList[7]->getId(), true);
   solver.assumeRelease();  // Clear assumptions
   solver.assumeProperty(newV, true);  // k = 1
   result = solver.assumpSolve();
   reportResult(solver, result);
}
