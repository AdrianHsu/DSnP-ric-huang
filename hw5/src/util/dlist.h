/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = true;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return *(this); }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { 
         _node = _node->_next;
         return *(this); 
      }
      iterator operator ++ (int) { 
         iterator tmp(_node);
         _node = _node->_next;
         return tmp; 
      }
      iterator& operator -- () { 
         _node = _node->_prev;
         return *(this); 
      }
      iterator operator -- (int) { 
         iterator tmp(_node);
         _node = _node->_prev;
         return tmp; 
      }

      iterator& operator = (const iterator& i) { 
         _node = i._node;
         return *(this); 
      }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head == _head->_prev); }
   size_t size() const {  
      size_t _size = 0;
      for(iterator i(begin()); i != end(); i++)
         _size++;
      return _size;
   }

   void push_back(const T& x) { 
      if(empty()) {
         DListNode<T>* tmp = new DListNode<T>(x, _head, _head);
         _head->_next = tmp;
         _head->_prev = tmp;
         _isSorted = true;
      } else {
         DListNode<T>* tmp = new DListNode<T>(x, _head->_prev, _head);
         (_head->_prev)->_next = tmp;
         _head->_prev = tmp;
         _isSorted = false;
      }
   }
   void pop_front() { erase(begin()); }
   void pop_back() { erase(end()); }

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(pos == 0 || pos._node == 0 || empty()) return false;
      
      if(pos == end()) pos--;
      DListNode<T> *my_prev = pos._node->_prev, *my_next = pos._node->_next;
      my_prev->_next = my_next;
      my_next->_prev = my_prev;
      delete pos._node;
      pos._node = 0;

      return true; 
   }
   bool erase(const T& x) { 
      for(iterator i(begin()); i != end(); i++) {
         if(i._node->_data == x)
            return erase(i);
      }

      return false; 
   }

   void clear() { 
      for(iterator i(begin()); i != end(); i++)
         erase(i);
      _isSorted = true;
   }  // delete all nodes except for the dummy node

   void sort() const { 
      if(_isSorted) return;
      _isSorted = true;
      
      DListNode<T>* i = _head->_next;
      DListNode<T>* j = i;
      for( ; i != _head; i = i->_next) {
         for( j = i ; j != _head ; j = j->_next )
            if( j->_data < i->_data )
               swap( i->_data , j->_data );
      }
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
