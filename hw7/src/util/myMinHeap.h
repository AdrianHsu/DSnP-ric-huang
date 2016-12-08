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
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { 
      return _data.size();
   }

   // TODO
   const Data& min() const { 
      // if (!_data.empty()) 
      return _data[0];
   }
   void insert(const Data& d) { 
      
      int t = _data.size();
      _data.push_back(d);
      while(t > 0) {
         int p = (t - 1) / 2;
         if(d < _data[p]) {
            _data[t] = _data[p];
            t = p; 
         }
         else break;
      }
      _data[t] = d;
   }
   void delMin() {delData(0);}

   void delData(size_t i) {
      Data ret = _data.back();
      _data.pop_back();
      int s = _data.size();
      int p = i, c = p * 2 + 1;
      if(p == s) return;
      while (c < s) {
         if(c + 1 < s && _data[c + 1] < _data[c]) ++c;
         if(ret < _data[c]) break;
         _data[p] = _data[c];
         p = c;
         c = p * 2 + 1;
      }

      c = (p - 1) / 2;
      while (c > 0) {
         if(ret < _data[c]) {
            _data[p] = _data[c];
            p = c;
            c = (p - 1) / 2;
         }
         else break;
      }
      
      _data[p] = ret;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
