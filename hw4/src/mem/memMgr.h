/****************************************************************************
  FileName     [ memMgr.h ]
  PackageName  [ cmd ]
  Synopsis     [ Define Memory Manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef MEM_MGR_H
#define MEM_MGR_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// Turn this on for debugging
// #define MEM_DEBUG

//--------------------------------------------------------------------------
// Define MACROs
//--------------------------------------------------------------------------
#define MEM_MGR_INIT(T) \
   MemMgr<T>* const T::_memMgr = new MemMgr<T>

#define USE_MEM_MGR(T)                                                      \
   public:                                                                     \
void* operator new(size_t t) { return (void*)(_memMgr->alloc(t)); }      \
void* operator new[](size_t t) { return (void*)(_memMgr->allocArr(t)); } \
void  operator delete(void* p) { _memMgr->free((T*)p); }                 \
void  operator delete[](void* p) { _memMgr->freeArr((T*)p); }            \
static void memReset(size_t b = 0) { _memMgr->reset(b); }                \
static void memPrint() { _memMgr->print(); }                             \
private:                                                                    \
static MemMgr<T>* const _memMgr

// You should use the following two MACROs whenever possible to 
// make your code 64/32-bit platform independent.
// DO NOT use 4 or 8 for sizeof(size_t) in your code
//
#define SIZE_T      sizeof(size_t)
#define SIZE_T_1    (sizeof(size_t) - 1)

// TODO: Define them by SIZE_T and/or SIZE_T_1 MACROs.
//
// To promote 't' to the nearest multiple of SIZE_T; 
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16
#define toSizeT(t)   (t % SIZE_T == 0) ? t : SIZE_T +  SIZE_T * (t / SIZE_T)  // TODO
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
#define downtoSizeT(t)  (t % SIZE_T == 0) ? t : SIZE_T * (t / SIZE_T)  // TODO

// R_SIZE is the size of the recycle list
#define R_SIZE 256

//--------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------
template <class T> class MemMgr;


//--------------------------------------------------------------------------
// Class Definitions
//--------------------------------------------------------------------------
// T is the class that use this memory manager
//
// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemBlock
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemBlock(MemBlock<T>* n, size_t b) : _nextBlock(n) {
      _begin = _ptr = new char[b]; _end = _begin + b; }
   ~MemBlock() { delete [] _begin; }

   // Member functions
   void reset() { _ptr = _begin; }
   bool getMem(size_t t, T*& ret) {
      // TODO
      // 1. Get (at least) 't' bytes memory from current block
      //    Promote 't' to a multiple of SIZE_T
      // 2. Update "_ptr" accordingly
      // 3. The return memory address is stored in "ret"
      // 4. Return false if not enough memory
      t = toSizeT(t);
      if(getRemainSize() < t) return false;

      ret = (T*)_ptr; // return current (not yet changed) _ptr address' begin, casted into T*
      _ptr += t; // after ret was set, we can change _ptr's position finally

      return true;
   }
   size_t getRemainSize() const { return size_t(_end - _ptr); }

   MemBlock<T>* getNextBlock() const { return _nextBlock; }

   // Data members
   char*             _begin;
   char*             _ptr;
   char*             _end;
   MemBlock<T>*      _nextBlock;
};

// Make it a private class;
// Only friend to MemMgr;
//
template <class T>
class MemRecycleList
{
   friend class MemMgr<T>;

   // Constructor/Destructor
   MemRecycleList(size_t a = 0) : _arrSize(a), _first(0), _nextList(0) {}
   ~MemRecycleList() { reset(); }

   // Member functions
   // ----------------
   size_t getArrSize() const { return _arrSize; }
   MemRecycleList<T>* getNextList() const { return _nextList; }
   void setNextList(MemRecycleList<T>* l) { _nextList = l; }
   // pop out the first element in the recycle list, and return it
   T* popFront() {
      // TODO
      if(_first == 0) {
         //cerr << "error popFront()" << endl;
         return _first;
      }
      T* _tmpFirst = _first;
      _first = getNext(_first);

      return _tmpFirst;
   }
   // push the element 'p' to the beginning of the recycle list
   void  pushFront(T* p) {
      // TODO

      size_t next = (size_t) _first;// get _first its own SIZE_T bytes
      *(size_t*)p = next;// "(size_t)p" is not equal to "*(size_t*)p"
      _first = p;
   }
   // Release the memory occupied by the recycle list(s)
   // DO NOT release the memory occupied by MemMgr/MemBlock
   void reset() {
      // TODO
      if(getNextList() != 0) {
         delete _nextList; // _nextList's destructor will call its reset(), thus clear all of them 
         setNextList(0);
      }
      _first = 0;
   }

   // Helper functions
   // ----------------
   // Iterate to the next element after 'p' in the recycle list
   T* getNext(T* p) const {
      // TODO

      //Since 'p' is an element in the recycle list, 
      //the first SIZE_T bytes of p contain the address of the next element.
      //To get the "content" of the first SIZE_T bytes, 
      //you need to convert p to _______ so that you can read the value of the first SIZE_T bytes.
      //After you get the value of the first SIZE_T bytes, just cast it to _____ and return.
      if(p == 0) return 0;

      size_t next = *(size_t*)p; // only require firse SIZE_T bytes, thus cast into size_t
      return (T*)next;
   }
   //
   // count the number of elements in the recycle list
   size_t numElm() const {
      size_t count = 0;
      T* p = _first;
      while (p) {
         p = getNext(p);
         ++count;
      }
      return count;
   }

   // Data members
   size_t              _arrSize;   // the array size of the recycled data
   T*                  _first;     // the first recycled data
   MemRecycleList<T>*  _nextList;  // next MemRecycleList
   //      with _arrSize + x*R_SIZE
};

template <class T>
class MemMgr
{
#define S sizeof(T)

   public:
      MemMgr(size_t b = 65536) : _blockSize(b) {
         assert(b % SIZE_T == 0);
         // _blockSize is SIZE_T * n, thus no need to alter it in MemBlock contructor
         _activeBlock = new MemBlock<T>(0, _blockSize); 
         for (int i = 0; i < R_SIZE; ++i)
            _recycleList[i]._arrSize = i;
      }
      ~MemMgr() { reset(); delete _activeBlock; }


      void reset(size_t b = 0) {

         assert(b % SIZE_T == 0);
#ifdef MEM_DEBUG
         cout << "Resetting memMgr...(" << b << ")" << endl;
#endif // MEM_DEBUG
         // TODO

         // 1. Remove the memory of all but the firstly allocated MemBlocks
         //    That is, the last MemBlock searched from _activeBlock.
         while(_activeBlock->getNextBlock() != 0) {
            MemBlock<T>*  tmp = _activeBlock;
            _activeBlock = _activeBlock->_nextBlock;
            delete tmp;
         }
         //    reset its _ptr = _begin (by calling MemBlock::reset())
         _activeBlock->reset();
         // 2. reset _recycleList[]
         for(size_t i = 0; i < R_SIZE; i ++) {
            _recycleList[i].reset();
         }
         // 3. 'b' is the new _blockSize; "b = 0" means _blockSize does not change
         //    if (b != _blockSize) reallocate the memory for the first MemBlock

         if (b != 0 && b != _blockSize){
            delete _activeBlock;
            _blockSize = b;
            // 4. Update the _activeBlock pointer
            _activeBlock = new MemBlock<T>(0, b);
         }
      }
      // Called by new
      T* alloc(size_t t) {
         assert(t == S);
#ifdef MEM_DEBUG
         cout << "Calling alloc...(" << t << ")" << endl;
#endif // MEM_DEBUG
         return getMem(t);
      }
      // Called by new[]
      T* allocArr(size_t t) {
#ifdef MEM_DEBUG
         cout << "Calling allocArr...(" << t << ")" << endl;
#endif // MEM_DEBUG
         // Note: no need to record the size of the array == > system will do
         return getMem(t);
      }
      // Called by delete
      void  free(T* p) {
#ifdef MEM_DEBUG
         cout << "Calling free...(" << p << ")" << endl;
#endif // MEM_DEBUG
         getMemRecycleList(0)->pushFront(p);
      }
      // Called by delete[]
      void  freeArr(T* p) {
#ifdef MEM_DEBUG
         cout << "Calling freeArr...(" << p << ")" << endl;
#endif // MEM_DEBUG
         // TODO
         // Get the array size 'n' stored by system,
         // which is also the _recycleList index

         size_t n = *(size_t*)p;
#ifdef MEM_DEBUG
         cout << ">> Array size = " << n << endl;
         cout << "Recycling " << p << " to _recycleList[" << n << "]" << endl;
#endif // MEM_DEBUG
         // add to recycle list...
         MemRecycleList<T>* list = getMemRecycleList(n);
         list->pushFront(p);
      }
      void print() const {
         cout << "=========================================" << endl
            << "=              Memory Manager           =" << endl
            << "=========================================" << endl
            << "* Block size            : " << _blockSize << " Bytes" << endl
            << "* Number of blocks      : " << getNumBlocks() << endl
            << "* Free mem in last block: " << _activeBlock->getRemainSize()
            << endl
            << "* Recycle list          : " << endl;
         int i = 0, count = 0;
         while (i < R_SIZE) {
            const MemRecycleList<T>* ll = &(_recycleList[i]);
            while (ll != 0) {
               size_t s = ll->numElm();
               if (s) {
                  cout << "[" << setw(3) << right << ll->_arrSize << "] = "
                     << setw(10) << left << s;
                  if (++count % 4 == 0) cout << endl;
               }
               ll = ll->_nextList;
            }
            ++i;
         }
         cout << endl;
      }

   private:
      size_t                     _blockSize;
      MemBlock<T>*               _activeBlock;
      MemRecycleList<T>          _recycleList[R_SIZE];

      // Private member functions
      //
      // t: #Bytes; MUST be a multiple of SIZE_T
      // return the size of the array with respect to memory size t
      // [Note] t must >= S
      size_t getArraySize(size_t t) const {
         assert(t % SIZE_T == 0);
         assert(t >= S);
         // TODO

         return (t - SIZE_T) / S; //discard
      }
      // [Note]: This function will be called by MemMgr->getMem() to get the
      //         recycle list. Therefore, the recycle list is first created
      //         by the MTNew command, not MTDelete.
      MemRecycleList<T>* getMemRecycleList(size_t n) {
         size_t m = n % R_SIZE;// e.g. n = 258, m = 2
         // TODO
         MemRecycleList<T>* list = &_recycleList[m]; 
         if(n == m) return list;
         // Go through _recycleList[m], its _nextList, and _nextList->_nextList, etc,
         //    to find a recycle list whose "_arrSize" == "n"
         while(list->getNextList() != 0) {
            m = list->getArrSize(); 
            if(n != m)
               list = list->getNextList();
            else {
               return list;
            }
         }
         if(list->getArrSize() == n) return list;

         // If not found, create a new MemRecycleList with _arrSize = n
         //    and add to the last MemRecycleList
         // So, should never return NULL

         // AH: "list" must be the last one now after while loop
         list->setNextList(new MemRecycleList<T>(n)); 
         return list->getNextList();

      }
      // t is the #Bytes requested from new or new[]
      // Note: Make sure the returned memory is a multiple of SIZE_T
      T* getMem(size_t t) {
         T* ret = 0;
#ifdef MEM_DEBUG
         cout << "Calling MemMgr::getMem...(" << t << ")" << endl;
#endif // MEM_DEBUG
         // TODO ---
         // 1. Make sure to promote t to a multiple of SIZE_T
         t = toSizeT(t);
         // 2. Check if the requested memory is greater than the block size.
         //    If so, throw a "bad_alloc()" exception.
         assert(t % SIZE_T == 0);
         if(t > _blockSize) {
            cerr << "Requested memory (" << t << ") is greater than block size"
               << "(" << _blockSize << "). " << "Exception raised...\n";
            throw bad_alloc();
         }

         // 3. Check the _recycleList first...

         // [NOTE] Use getArraySize(t) function in (at least) getMem() to get the size of array
         //        and call getMemRecycleList() later to get the index for
         //        the _recycleList[]
         size_t n = getArraySize(t);
         MemRecycleList<T>* list = getMemRecycleList(n);
         //    => 'n' is the size of array
         if (list->_first != 0) {
            ret = list->popFront();
#ifdef MEM_DEBUG
            cout << "Recycled from _recycleList[" << arrSize
               << "]..." << ret << endl;
#endif // MEM_DEBUG
            return ret;
         }
         if(ret == 0) {
            // If no match from recycle list...
            // 4. Get the memory from _activeBlock
            if(!_activeBlock->getMem(t, ret)) {

               // 5. If not enough, recycle the remained memory and print out ---
               //    Note: recycle to the as biggest array index as possible
               //    Note: rn is the array size
               size_t recycle = _activeBlock->getRemainSize();

               if (recycle >= S) {
                  _activeBlock->getMem(recycle, ret);
                  size_t rn = getArraySize(recycle);
                  list = getMemRecycleList(rn);

#ifdef MEM_DEBUG
                  cout << "Recycling " << ret << " to _recycleList[" << rn << "]\n";
#endif // MEM_DEBUG
                  list->pushFront(ret); //recycling
               }
               //    ==> allocate a new memory block, and print out ---
               _activeBlock = new MemBlock<T>(_activeBlock, _blockSize);
#ifdef MEM_DEBUG
               cout << "New MemBlock... " << _activeBlock << endl;
#endif // MEM_DEBUG

               _activeBlock->getMem(t, ret); // get new memory block
            }

            // 6. At the end, print out the acquired memory address
#ifdef MEM_DEBUG
            cout << "Memory acquired... " << ret << endl;
#endif // MEM_DEBUG  
         }

         return ret;
      }
      // Get the currently allocated number of MemBlock's
      size_t getNumBlocks() const {
         // TODO
         size_t count = 0;
         MemBlock<T>* p = _activeBlock;
         while (p) {
            p = p->getNextBlock();
            ++count;
         }
         return count;
      }

};

#endif // MEM_MGR_H
