/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
#include <stddef.h> // for size_t

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define FecHashMap classes
//-----------------------
// To use FecHashMap ADT, you should define your own FecHashKey class.
// It should at least overload the "()" and "==" operators.
//
#define MY_MAX 2147483647

class FecHashKey
{
public:
   FecHashKey(unsigned v): simValue(v) { }

   size_t operator() () const {
      size_t k = 0;
      for(unsigned i = 0; i < 4; ++i)
         k ^= (simValue << (i*8));
      return k;
   }
   size_t operator~ () const {
      return ~simValue;
   } 
   bool operator == (const FecHashKey& k) const { 
      if( simValue == k.getSimValue() )
         return true; 
      else
         return false;
   }
   unsigned getSimValue() const {
      return simValue;
   }

private:
   unsigned simValue;
};

template <class FecHashKey, class FecHashData>
class FecHashMap
{
typedef pair<FecHashKey, FecHashData> FecHashNode;

public:
   FecHashMap(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~FecHashMap() { reset(); }

   // [Optional] TODO: implement the FecHashMap<FecHashKey, FecHashData>::iterator
   // o An iterator should be able to go through all the valid FecHashNodes
   //   in the FecHashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the FecHashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class FecHashMap<FecHashKey, FecHashData>;
   
   public:
      iterator(vector<FecHashNode>* b, size_t num, size_t n) 
         : _nb(num), _n(n), _i(0), _buckets(b) { }
      ~iterator(){}
      const FecHashNode& operator * () const { return _buckets[_n][_i]; }
      FecHashNode& operator *() { return _buckets[_n][_i]; }
      iterator& operator++() {
         if(_n == _nb) return *this;
         else if(_i < _buckets[_n].size() - 1) ++_i;
         else {
            while(++_n < _nb)
               if(!_buckets[_n].empty())
                  break;
            _i = 0;
         }
         return *this;
      }
      iterator operator++(int) {
         iterator tmp = iterator(*this);
         ++(*this);
         return tmp;
      }
      iterator& operator--() {
         if(_i > 0) {
            --_i;
            return (*this);
         }
         size_t t = _n;
         while(t > 0) {
            --t;
            if(!_buckets[t].empty()) {
               _i = _buckets[t].size() - 1;
               _n = t;
               break;
            }
         }
         return (*this);
      }
      iterator operator--(int) {
         iterator tmp = iterator(*this);
         --(*this);
         return tmp;
      }
      iterator& operator = (const iterator& i) {
         _nb = i._nb;
         _n = i._n;
         _i = i._i;
         _buckets = i._buckets;
         return *this;
      }
      bool operator == (const iterator& i) const {
         return((_i == i._i) && (_n == i._n));
      }
      bool operator != (const iterator& i) const {
         return !(*this == i);
      }


   private:
      size_t   _nb;
      size_t   _n, _i;
      vector<FecHashNode>*  _buckets;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<FecHashNode>[b];
   }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<FecHashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<FecHashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
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
   bool check(const FecHashKey& k, FecHashData& d, bool& inv) const {
      // find same simValue (not inverse)
      size_t n = bucketNum(k);
      for(size_t i = 0; i < _buckets[n].size(); i++) {
         if(_buckets[n][i].first == k) {
            inv = 0;
            d = _buckets[n][i].second;
            return true;
         }
      }
      // same simValue not found, let's try inv value!
      n = bucketNum(~k);
      for(size_t i = 0; i < _buckets[n].size(); i++) {
         if(_buckets[n][i].first == ~k) {
            inv = 1;
            d = _buckets[n][i].second;
            return true;
         }
      }
      return false; 
   }

   bool insert(const FecHashKey& k, const FecHashData& d) {
      size_t n = bucketNum(k);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i].first == k || _buckets[n][i].first == ~k)
            return false;

      _buckets[n].push_back(FecHashNode(k, d));
      return true;
   }

private:

   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<FecHashNode>*        _buckets;

   size_t bucketNum(const FecHashKey& k) const {
      return (k() % _numBuckets); }

};


class HashKey
{
public:
   HashKey(size_t l0, size_t l1): lhs0(l0), lhs1(l1) { }
// Cantor pairing function
// https://hbfs.wordpress.com/2011/09/27/pairing-functions/
   size_t operator() () const { 
      size_t x = (lhs0 % MY_MAX); //getHashSize() max value
      size_t y = (lhs1 % MY_MAX);
      if(x < y)
         swap(x, y); // make x >= y,for ordering
      size_t p = 0;
      size_t i = 0;
      while (x || y) {
         p |= ((size_t)(x & 1) << i);
         x >>= 1;
         p |= ((size_t)(y & 1) << (i + 1));
         y >>= 1;
         i += 2;
      }
      return p;
   }
   bool operator == (const HashKey& k) const { 
      // if( (*this)() == k() )
      if( (*this).sameInput(k) )
         return true; 
      else
         return false;
   }
   size_t getlhs0() const {
      return lhs0;
   }
   size_t getlhs1() const {
      return lhs1;
   }
   bool sameInput(const HashKey& k) const {
      if(lhs0 == k.getlhs0() && lhs1 == k.getlhs1())
         return true;
      else if(lhs0 == k.getlhs1() && lhs1 == k.getlhs0())
         return true;
      return false;
   }

private:
   size_t lhs0;//140316368 699952
   size_t lhs1;//140316368 700032
};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;
   
   public:
      iterator(vector<HashNode>* b, size_t num, size_t n) 
         : _nb(num), _n(n), _i(0), _buckets(b) { }
      ~iterator(){}
      const HashNode& operator * () const { return _buckets[_n][_i]; }
      HashNode& operator *() { return _buckets[_n][_i]; }
      iterator& operator++() {
         if(_n == _nb) return *this;
         else if(_i < _buckets[_n].size() - 1) ++_i;
         else {
            while(++_n < _nb)
               if(!_buckets[_n].empty())
                  break;
            _i = 0;
         }
         return *this;
      }
      iterator operator++(int) {
         iterator tmp = iterator(*this);
         ++(*this);
         return tmp;
      }
      iterator& operator--() {
         if(_i > 0) {
            --_i;
            return (*this);
         }
         size_t t = _n;
         while(t > 0) {
            --t;
            if(!_buckets[t].empty()) {
               _i = _buckets[t].size() - 1;
               _n = t;
               break;
            }
         }
         return (*this);
      }
      iterator operator--(int) {
         iterator tmp = iterator(*this);
         --(*this);
         return tmp;
      }
      iterator& operator = (const iterator& i) {
         _nb = i._nb;
         _n = i._n;
         _i = i._i;
         _buckets = i._buckets;
         return *this;
      }
      bool operator == (const iterator& i) const {
         return((_i == i._i) && (_n == i._n));
      }
      bool operator != (const iterator& i) const {
         return !(*this == i);
      }


   private:
      size_t   _nb;
      size_t   _n, _i;
      vector<HashNode>*  _buckets;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b];
   }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
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

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
      size_t n = bucketNum(k);
      for(size_t i = 0; i < _buckets[n].size(); i++) {
         if(_buckets[n][i].first == k) {
            d = _buckets[n][i].second;
            return true;
         }
      }
      return false; 
   }
   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   // bool update(const HashKey& k, HashData& d) { 
   //    size_t n = bucketNum(k);
   //    for(size_t i = 0; i < _buckets[n].size(); i++)
   //       if(_buckets[n][i].first == k) {
   //          _buckets[n][i].second = d;
   //          return true;
   //       }
   //    _buckets[n].push_back(HashNode(k, d));
   //    return false;
   // }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
      size_t n = bucketNum(k);
      for(size_t i = 0; i < _buckets[n].size(); i++)
         if(_buckets[n][i].first == k)
            return false;

      _buckets[n].push_back(HashNode(k, d));
      return true;
   }
   // return true if removed successfully (i.e. k is in the hash)
   // return false otherwise (i.e. nothing is removed)
   // bool remove(const HashKey& k) { 
   //    size_t n = bucketNum(k);
   //    size_t s = _buckets[n].size();
   //    for(size_t i = 0; i < _buckets[n].size(); i++)
   //       if(_buckets[n][i].first == k) {
   //          _buckets[n][i] = _buckets[n].back();
   //          _buckets[n].resize(s - 1);
   //          return true;
   //       }
   //    return false;
   // }

private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 

// template <class CacheKey, class CacheData>
// class Cache
// {
// typedef pair<CacheKey, CacheData> CacheNode;

// public:
//    Cache() : _size(0), _cache(0) {}
//    Cache(size_t s) : _size(0), _cache(0) { init(s); }
//    ~Cache() { reset(); }

//    // NO NEED to implement Cache::iterator class

//    // TODO: implement these functions
//    //
//    // Initialize _cache with size s
//    void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
//    void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

//    size_t size() const { return _size; }

//    CacheNode& operator [] (size_t i) { return _cache[i]; }
//    const CacheNode& operator [](size_t i) const { return _cache[i]; }

//    // return false if cache miss
//    bool read(const CacheKey& k, CacheData& d) const {
//       size_t i = k() % _size;
//       if (k == _cache[i].first) {
//          d = _cache[i].second;
//          return true;
//       }
//       return false;
//    }
//    // If k is already in the Cache, overwrite the CacheData
//    void write(const CacheKey& k, const CacheData& d) {
//       size_t i = k() % _size;
//       _cache[i].first = k;
//       _cache[i].second = d;
//    }

// private:
//    // Do not add any extra data member
//    size_t         _size;
//    CacheNode*     _cache;
// };

#endif // MY_HASH_H
