/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* b, size_t num, size_t n) 
      : _numBuckets(num),  _n(n), _i(0), _buckets(b) {
         if(_n != _numBuckets) _node = &_buckets[_n][_i];
         else _node = NULL;
      }
      ~iterator(){}
      const Data& operator * () const { return *(_node); }
      Data& operator *() { return *(_node); }
      iterator& operator++() {
         if(_n == _numBuckets) return *this;
         else if(_i < _buckets[_n].size() - 1) ++_i;
         else { //_i == _buckets[_n].size() - 1
            while(++_n < _numBuckets)
               if(!_buckets[_n].empty())
                  break;
            _i = 0;
         }
         if(_n != _numBuckets) _node = &_buckets[_n][_i];
         else _node = NULL;

         return *this;
      }
      iterator operator++(int) {
         iterator tmp = iterator(*this);
         ++(*this);
         return tmp;
      }
      iterator& operator--() {
         if(_i > 0)
            --_i;
         else {
            size_t t = _n;
            while(t > 0 && t <= _numBuckets) {
               --t;
               if(!_buckets[t].empty()) {
                  _i = _buckets[t].size() - 1;
                  _n = t;
                  break;
               }
            }
         }
         _node = &_buckets[_n][_i];
         return (*this);
      }
      iterator operator--(int) {
         iterator tmp = iterator(*this);
         --(*this);
         return tmp;
      }
      iterator& operator = (const iterator& i) {
         _node = i._node;
         _numBuckets = i._numBuckets;
         _i = i._i;
         _n = i._n;
         _buckets = i._buckets;
         return this;
      }
      bool operator == (const iterator& i) const {
         return((_i == i._i) && (_n == i._n) && (_node == i._node));
      }
      bool operator != (const iterator& i) const {
         return !(*this == i);
      }
         
         
   private:
      Data*             _node;
      size_t            _numBuckets, _n, _i;
      vector<Data>*     _buckets;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions

   // Point to the first valid data
   iterator begin() const {
      for(size_t n = 0; n < _numBuckets; n++)
         if(!_buckets[n].empty()) {
            return iterator(_buckets, _numBuckets, n);
         }
      return end();
   }
   // Pass the end
   iterator end() const {
      size_t n = _numBuckets;
      return iterator(_buckets, _numBuckets, n);
   }
   // return true if no valid data
   bool empty() const { 
      for (size_t n = 0; n < _numBuckets; n++)
         if (!_buckets[n].empty())  return false;
      return true;
   }
   // number of valid data
   size_t size() const { 
      size_t s = 0;
      for(size_t n = 0; n < _numBuckets; n++)
         s += _buckets[n].size(); 
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      size_t n = bucketNum(d);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i] == d)
            return true;
      return false; 
   }
   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t n = bucketNum(d);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i] == d) {
            d = _buckets[n][i];
            return true;
         }
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      size_t n = bucketNum(d);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i] == d) {
            _buckets[n][i] = d;
            return true;
         }
      _buckets[n].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t n = bucketNum(d);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i] == d)
            return false;

      _buckets[n].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return false otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t n = bucketNum(d);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i] == d) {
            _buckets[n].erase(_buckets[n].begin() + i);
            return true;
         }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
