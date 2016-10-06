/****************************************************************************
  FileName     [ main.cpp ]
  PackageName  [ main ]
  Synopsis     [ main() for modCalc ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Global cmd Manager
//----------------------------------------------------------------------
CmdParser* cmdMgr = new CmdParser("mydb> ");

extern bool initCommonCmd();
extern bool initDbCmd();

static void
usage()
{
   cout << "Usage: modCalc [ -File < doFile > ]" << endl;
}

static void
myexit()
{
   usage();
   exit(-1);
}

int
main(int argc, char** argv)
{
   ifstream dof;
   string tok = "??";
   //myStrGetTok("DBSort 3 1 8 3", tok, 6, ' ');
   //cout << "AHTOKKKK" << tok << endl; // "3"
   //cout << myStrNCmp("HELp", "HELPp", 3); // -1
   //cout << myStrNCmp("HELp", "Hel", 3); // 0
   //cout << myStrNCmp("DBSort", "dbsO", 3); // 0
   
   //CmdExec* e;
   //string token = "";
   //string option = "-Row";
   //e->lexSingleOption(option, token, true);
   //cout << "token=" << token << endl; //-Row

   //string option = "";
   //e->lexSingleOption(option, token, false);
   //cout << "token=" << token << endl; //Error: Missing option!!
   
   //vector<string> token;
   //string option = "3 1 8 3";
   //e->lexOptions(option, token, 3);
   //cout << "token=" << token[3] << endl;//error

   if (argc == 3) {  // -file <doFile>
      if (myStrNCmp("-File", argv[1], 2) == 0) {
         if (!cmdMgr->openDofile(argv[2])) {
            cerr << "Error: cannot open file \"" << argv[2] << "\"!!\n";
            myexit();
         }
      }
      else {
         cerr << "Error: unknown argument \"" << argv[1] << "\"!!\n";
         myexit();
      }
   }
   else if (argc != 1) {
      cerr << "Error: illegal number of argument (" << argc << ")!!\n";
      myexit();
   }

   if (!initCommonCmd() || !initDbCmd())
      return 1;

   CmdExecStatus status = CMD_EXEC_DONE;
   while (status != CMD_EXEC_QUIT) {  // until "quit" or command error
      status = cmdMgr->execOneCmd();
      cout << endl;  // a blank line between each command
   }

   return 0;
}
