/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { 
      if(_data.empty()) return 0;
      return _data[i]; 
   }   
   Data& operator [] (size_t i) { 
      if(_data.empty()) return 0;
      return _data[i]; 
   }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return Data(); }
   void insert(const Data& d) { 
      if(size() == 0)
         _data.push_back(Data()); // NULL
      
      _data.push_back(d);
      size_t t = size();
      
      while(t > 1) {
         int p = t / 2;
         if(d >= _data[p])
            break;
         _data[t] = _data[p];
         t = p;
      }
      _data[t] = d;
   }
   void delMin() { 
      Data ret = _data[1];
      int p = 1, t = 2*p, n = size();
      while(t <= n) {
         if(t < n)
            if(_data[t] > _data[t + 1])
               ++t;
         if(_data[n] < _data[t])
            break;
         _data[p] = _data[t];
         p = t;
         t = 2*p;
      }
      _data.pop_back();
      n = size();
      _data[p] = _data[n];
      if(n == 1)
         _data.pop_back(); // NULL data
      return ret;
   }
   void delData(size_t i) {
      if(size() <= i) return; 
      
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
