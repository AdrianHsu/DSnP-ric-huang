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
         if(_data[t] < _data[p])
            swap(_data[t], _data[p]);
         else break;
         t = p;
      }
   }
   void delMin() {delData(0);}

   void delData(size_t i) {
      if(_data.empty()) return;

      swap(_data[i], _data.back());
      _data.pop_back();
      size_t s = _data.size();
      if(i == s) return;
      //floating
      size_t a = i;
      size_t p = (a - 1) / 2;
      bool flt = 0;
      while(p > 0) {
         if(_data[a] < _data[p]) {
            swap(_data[a], _data[p]);
            flt = 1;
            a = p;
            p = (a - 1) / 2;
         } else break;
      }
      if(flt) return;
      //sinking
      size_t b = a * 2 + 1;
      while(b < s) {
         if(b + 1 < s && (_data[b + 1] < _data[b])) b++;
         if(_data[b] < _data[a]) {
            swap(_data[a], _data[b]);
            a = b;
            b = 2 * a + 1;
         } else break;
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
