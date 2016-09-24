/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : if(moveBufPtr(_readBufPtr - 1)) deleteChar(); /* TODO */ break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1);/* TODO */ break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1);/* TODO */ break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : insertChar(' ', 3); break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   if(_readBuf > ptr || _readBufEnd < ptr) {
      mybeep();
      return false;
   }

   if(_readBufPtr - 1 == ptr)
      cout << '\b';
   else if(_readBufPtr + 1 == ptr)
      cout << *_readBufPtr;
   else if(_readBuf == ptr){
   
      char* _cursorPtr = _readBufPtr;
      while(_cursorPtr > _readBuf) {
         cout << '\b';
         _cursorPtr--;
      }
   } else if(_readBufEnd == ptr) {
  
      char* _cursorPtr = _readBufPtr;
      while(_cursorPtr < _readBufEnd) {
         cout << *_cursorPtr;
         _cursorPtr++;
      }
   }

   _readBufPtr = ptr;
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//


bool
CmdParser::deleteChar()
{
   // TODO...
   if(_readBuf == _readBufEnd) {
      mybeep();
      return false;
   }
   if(_readBufPtr == _readBufEnd) {
      mybeep();
      return false;
   }
   
   char* _tmpPtr = _readBufPtr;
   while(_tmpPtr < _readBufEnd) {
      *_tmpPtr = *(_tmpPtr + 1);
      _tmpPtr++;
   }
   _readBufEnd--;
   *_tmpPtr = 0;

   char* _printPtr = _readBufPtr;
   while(_printPtr < _readBufEnd) {
      cout << *_printPtr;
      _printPtr++;
   }
   cout << " ";
   while(_printPtr > _readBufPtr) {
      cout << '\b';
      _printPtr--;
   }
   cout << '\b';
   *_readBufEnd = 0; 
   
   return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{ 
   // TODO...
   char* _tmpPtr = _readBufEnd;
   _readBufEnd += repeat;

   while(_tmpPtr != _readBufPtr) {
      *(_tmpPtr + repeat) = *_tmpPtr;
      _tmpPtr--;
   }
   *(_tmpPtr + repeat) = *_tmpPtr;
   
   for(int i = 0; i < repeat; i++) {
      *_readBufPtr = ch;
      _readBufPtr++;
   }
   char* _printPtr = _tmpPtr;
   while(_printPtr < _readBufEnd) {
      cout << *_printPtr;
      _printPtr++;
   }
   while(_printPtr > _readBufPtr) {
      cout << '\b';
      _printPtr--;
   }
   
   assert(repeat >= 1);
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO... 
   
   if(_readBuf == _readBufEnd)
      return;
   
   while(_readBufPtr < _readBufEnd) {
      cout << *_readBufPtr;
      _readBufPtr++;
   }
   while(_readBuf < _readBufPtr) {
      _readBufPtr--;
      cout << '\b';
      cout << " ";
      cout << '\b';
   }

   char* _deletePtr = _readBufEnd;
   char* _movePtr = _readBufEnd;
   while(_movePtr > _readBuf) {
      _deletePtr = _movePtr;
      _movePtr--;
      *_deletePtr = 0;
   }
   *_movePtr = 0;
   _readBufPtr = _readBufEnd = _readBuf;
   *_readBufEnd = 0;
    
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0, oldest), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   if(index == _historyIdx) return;

   string tmp = "";
   if(index < _historyIdx) {
      
      if(_historyIdx == 0) {
         mybeep();
         return;
      }
      if(index < 0) index = 0;
      
      if(_historyIdx == _history.size()) {
         _tempCmdStored = true;
         string tmp1 = (string)_readBuf;
         _history.push_back(tmp1);
      }
      
      tmp = _history[index];
   } else {
     
      if(index < _history.size() - 1)
         tmp = _history[index];
      else if(index == _history.size() - 1) {
         if(_tempCmdStored) {
            _tempCmdStored = false;
            tmp = _history[index];
            _history.pop_back();
         } else {
            cout << "IMPOSSIBLE" << endl;
         }
      } else {
         if(index - _historyIdx == 1) {
            mybeep();
            return;
         } else {
           index = _history.size() - 1; 
            if(_tempCmdStored) {
               _tempCmdStored = false;
               tmp = _history[index];
               _history.pop_back();
            } else {
               //cout << "IMPOSSIBLE!" << endl;
               mybeep();
               return;
            } 
         }
      }
   }
   _historyIdx = index;
   
   deleteLine();

   int len = tmp.size();
   strcpy(_readBuf, tmp.c_str());
   char* _tmpPtr = _readBuf;
   _readBufPtr = _tmpPtr;
   for(int i = 0; i < len; i++) {
      cout << *_tmpPtr;
      _tmpPtr++;
   }
   _readBufPtr = _readBufEnd = _tmpPtr;
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   string history = "";

   char* _spaceFrontPtr = _readBuf;
   char* _spaceBackPtr = _readBufEnd;
   while(*_spaceFrontPtr == ' ') {
      _spaceFrontPtr++;
   }
   _spaceBackPtr--;
   while(*_spaceBackPtr == ' ') {
      _spaceBackPtr--;
   }
   _spaceBackPtr++;
   while(_spaceFrontPtr < _spaceBackPtr) {
      history += *_spaceFrontPtr;
      _spaceFrontPtr++;
   }
   if(_tempCmdStored) {
      _tempCmdStored = false;
      if(history.size() != 0)
         _history.back() = history;
      else
         _history.pop_back();
   } else {
      if(history.size() != 0)_history.push_back(history);
   }
   _historyIdx = _history.size();
}
// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
