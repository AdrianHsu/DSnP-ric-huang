/****************************************************************************
  FileName     [ cirDef.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic data or var for cir package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_DEF_H
#define CIR_DEF_H

#include <vector>
#include "myHashMap.h"

using namespace std;

// TODO: define your own typedef or enum

class CirGate;
class CirMgr;
class SatSolver;
// by AH
class CirAigGate;
class CirPiGate;
class CirPoGate;
class CirUndefGate;
class CirConstGate;

typedef vector<CirGate*>           GateList;
typedef vector<unsigned>           IdList;
typedef HashMap<FECHashKey, CirGate*> FecGrp;
typedef vector< FecGrp >           FecGrps;

enum GateType
{
   UNDEF_GATE = 0,
   PI_GATE    = 1,
   PO_GATE    = 2,
   AIG_GATE   = 3,
   CONST_GATE = 4,

   TOT_GATE
};

// by AH
enum OptType
{
   CONST_ZERO   = 0,
   CONST_ONE    = 1,
   SAME_FANIN   = 2,
   INVERT_FANIN = 3,
   NO_OPT       = 4,
   // dummy end
   OPT_TOT
};
#endif // CIR_DEF_H
