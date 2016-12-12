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
      int p = (t - 1) / 2; 
      _data.push_back(d);
      while(t > 0) {
         if(t > 0) p = (t - 1) / 2;
         else p = 0;
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
      int s = _data.size();
      _data.resize(--s);
      if(i == (size_t) s) return;
      //floating
      int a = i;
      int p;
      if(a > 0) p = (a - 1) / 2;
      else p = 0;

      bool flt = 0;
      while(p > 0) {
         if(_data[p] < _data[a] || _data[a].getLoad() == _data[p].getLoad()) {
            break;
         } else {
            swap(_data[a], _data[p]);
            flt = 1;
            a = p;
            if(a > 0) p = (a - 1) / 2;
            else p = 0;
         }
      }
      if(flt) return;
      //sinking
      int b = a * 2 + 1;
      while(b < s) {
         if(b + 1 < s && (_data[b + 1] < _data[b])) b++;
         if(_data[a] < _data[b] || _data[b].getLoad() == _data[a].getLoad()) {
            break;
         } else {
            swap(_data[a], _data[b]);
            a = b;
            b = 2 * a + 1;
         }
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
