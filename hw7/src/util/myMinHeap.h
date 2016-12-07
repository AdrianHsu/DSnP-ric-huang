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
      // if(_data.empty()) return 0;
      return _data[i]; 
   }   
   Data& operator [] (size_t i) { 
      // if(_data.empty()) return 0;
      return _data[i]; 
   }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { 
      // if(size() == 0) return 0; 
      // else if(size() == 1) return 0; // ERROR
      // else return _data[1]; 
      return _data[1];
   }
   void insert(const Data& d) { 
      if(size() == 0)
         _data.push_back(Data()); // NULL
      
      _data.push_back(d);
      size_t t = size();
      
      while(t > 1) {
         int p = t / 2;
         if(_data[p] < d || _data[p] == d)
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
            if(_data[t + 1] < _data[t])
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
      if(i == 0) i++;
      swap(_data[i], _data[size() - 1]);
      //MIN_HEAPIFY(i)
      bool run = 1;
      while(run) {
         size_t l = (2 * i), r = (2 * i + 1);
         size_t largest = 0;

         if(l < size()) {
            if(_data[l] < _data[i])
               largest = l;
         } else largest = i;
         if(r < size()) {
            if(_data[r] < _data[largest])
               largest = r;
         }
         if(largest != i) {
            swap(_data[i], _data[largest]);
            i = largest;
            run = 1;
         } else run = 0;
      }  
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
