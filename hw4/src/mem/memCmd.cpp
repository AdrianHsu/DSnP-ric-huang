/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
     
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {

         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   // check option   

   vector<string> t; //tokens
   if(!CmdExec::lexOptions(option, t))
      return CMD_EXEC_ERROR;
   if(t.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING, "");

   if(t.size() == 1) {
      int num;
      if(myStr2Int(t[0], num)) { //suppose num <= INT_MAX
         if(num <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         mtest.newObjs((size_t)num);    
      } else {
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
      }
   } else if(t.size() > 3) {
      return CmdExec::errorOption(CMD_OPT_EXTRA, t[t.size() - 1]);
      
   } else if(t.size() == 2) {
      int arg1 = 0, arg2 = 0;
      bool arg1IsInt = myStr2Int(t[0], arg1);
      bool arg2IsInt = myStr2Int(t[1], arg2);
      if(arg1IsInt && arg2IsInt) {
         if(arg1 <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         else 
            return CmdExec::errorOption(CMD_OPT_EXTRA, t[1]);
         
      } else if (!arg1IsInt && arg2IsInt) {
         if(myStrNCmp("-Array", t[0], 2) == 0) {
            if(arg2 <= 0)
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
            else
               return CmdExec::errorOption(CMD_OPT_MISSING, "");
         } else {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         }
      } else if (arg1IsInt && !arg2IsInt) {
         if(arg1 <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         else {
            if(myStrNCmp("-Array", t[1], 2) == 0) {
               return CmdExec::errorOption(CMD_OPT_MISSING, t[1]);
            } else {
               return CmdExec::errorOption(CMD_OPT_EXTRA, t[1]);
            }
         }
      } else {
         if(myStrNCmp("-Array", t[0], 2) == 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
         } else {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         }
      }
       
   } else {
      // t.size() == 2 or 3; however 2 must return error
      size_t num = 0;
      size_t arr = 0;
      int arg0 = 0, arg1 = 0, arg2 = 0;
      if(!myStr2Int(t[0], arg0)) {
         if(myStrNCmp("-Array", t[0], 2) != 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         } else {
            if(!myStr2Int(t[1], arg1)) {
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
            } else {
               if(arg1 <= 0)
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
               else {
                  arr = (size_t)arg1;
                  if(!myStr2Int(t[2], arg2))
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
                  else {
                     if(arg2 <= 0) 
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
                     else
                        num = (size_t)arg2;
                  }
               }
            }
         }
      } else {
         if(arg0 <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
         else {
            num = (size_t)arg0;
            if(myStrNCmp("-Array", t[1], 2) != 0)
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
            else {
               if(!myStr2Int(t[2], arg2))
                  return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
               else {
                  if(arg2 <= 0)
                     return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
                  else
                     arr = (size_t)arg2;
               }
            }
         }
      }
      if(num == 0 || arr == 0)
         cerr << "ERROR: MTNewCmd" << endl;
      mtest.newArrs(num, arr);
   }
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> t; //tokens
   if(!CmdExec::lexOptions(option, t))
      return CMD_EXEC_ERROR;
   if(t.size() == 0) return CmdExec::errorOption(CMD_OPT_MISSING, "");
   if(myStrNCmp("-Index", t[0], 2) == 0) {
      if(t.size() == 1)
         return CmdExec::errorOption(CMD_OPT_MISSING, t[0]);
      int arg1 = 0;
      if(!myStr2Int(t[1], arg1))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
      if(arg1 < 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
      
      if(t.size() == 2) { 
         size_t objId = (size_t)arg1;
         if(objId >= mtest.getObjListSize()) {
            cerr << "Size of object list (" << mtest.getObjListSize()
               << ") is <= " << objId << "!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
         }
         mtest.deleteObj(objId);
      } else if(t.size() == 3){
         if(myStrNCmp("-Array", t[2], 2) != 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
         } else {
            size_t arrId = (size_t)arg1;
            if (arrId >= mtest.getArrListSize()) {
               cerr << "Size of array list (" << mtest.getArrListSize()
                  << ") is <= " << arrId << "!!\n";
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
            }
            mtest.deleteArr(arrId);
         }
      } else { // must be error
         // weird but don't need to check objListSize & arrListSize, according to ref
         if(myStrNCmp("-Array", t[2], 2) != 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
         } else {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[3]);//weird but identical to ref
         }
      }
   } else if(myStrNCmp("-Random", t[0], 2) == 0) { 
      if(t.size() == 1)
         return CmdExec::errorOption(CMD_OPT_MISSING, t[0]);
      int arg1 = 0;
      if(!myStr2Int(t[1], arg1))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
      if(arg1 <= 0)
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
      size_t numRandId = (size_t)arg1;
      if(t.size() == 2) { 
         size_t objSize = mtest.getObjListSize();
         if (objSize == 0) {
            cout << "Size of object list is 0!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
         }
         for (size_t i = 0; i < numRandId; ++i)
            mtest.deleteObj(rnGen(objSize));
      } else if(t.size() == 3){ 
         if(myStrNCmp("-Array", t[2], 2) != 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[1]);
         size_t arrSize = mtest.getArrListSize();
         if (arrSize == 0) {
            cout << "Size of array list is 0!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
         }
         for (size_t i = 0; i < numRandId; ++i)
            mtest.deleteArr(rnGen(arrSize));
      } else {
         if(myStrNCmp("-Array", t[2], 2) != 0) {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[2]);
         } else {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[3]);//weird but identical to ref
         }
      }
   } else 
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, t[0]);
   
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


