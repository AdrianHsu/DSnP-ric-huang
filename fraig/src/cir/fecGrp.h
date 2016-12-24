/****************************************************************************
  FileName     [ fecGrp.h ]
  PackageName  [ cir ]
  Synopsis     [ My Fec Group ]
  Author       [ Pin-Chun (Adrian) Hsu ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef FEC_GRP_H
#define FEC_GRP_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"
#include "cirGate.h"

class FecGrp
{
public:
	FecGrp(size_t s = 0): simValue(s) {};
	void addGate(CirGate* g) { gates.push_back(g); }
	CirGate* getGate(size_t i) { 
		if(i >= gates.size() ) return NULL;
		return gates[i]; 
	}
	size_t getGatesSize() { return gates.size(); }
	void setSimValue(size_t s) { simValue = s; }
	size_t getSimValue() { return simValue; }

private:
	vector<CirGate*> gates;
	size_t simValue;
};

#endif // CIR_MGR_H
