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
      if(_data.size() == 0) return 0;
      else if(_data.size() == 1) return 0; // ERROR
      else return _data.size() - 1; 
   }

   // TODO
   const Data& min() const { 
      // if (!_data.empty()) 
         return _data[1];
      // return Data(); //DUMMY
   }
   void insert(const Data& d) { 
      if(_data.size() == 0)
         _data.push_back(Data()); // NULL
      
      _data.push_back(d);
      size_t t = _data.size();
      
      while(t > 1) {
         int p = t / 2;
         if(_data[p] < d || _data[p] == d)
            break;
         _data[t] = _data[p];
         t = p;
      }
      _data[t] = d;
   }
   void delMin() {delData(0);}

   void delData(size_t i) {
      i++;
      swap(_data[i], _data[_data.size() - 1]);
      //MIN_HEAPIFY(i)
      bool run = 1;
      while(run) {
         size_t l = (2 * i), r = (2 * i + 1);
         size_t largest = 0;

         if(l < _data.size()) {
            if(_data[l] < _data[i])
               largest = l;
         } else largest = i;
         if(r < _data.size()) {
            if(_data[r] < _data[largest])
               largest = r;
         }
         if(largest != i) {
            swap(_data[i], _data[largest]);
            i = largest;
            run = 1;
         } else run = 0;
      }
      _data.pop_back();
      if(_data.size() == 1)
         _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
