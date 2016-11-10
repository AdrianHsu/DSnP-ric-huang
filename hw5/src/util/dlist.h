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
         //if(node->_next != 0)
            _node = _node->_next;
         return *(this); 
      } //prefix
      iterator operator ++ (int) { 
         iterator tmp(_node);
         //if(node->_next != 0)
            _node = _node->_next;
         return tmp; 
      } //postfix
      iterator& operator -- () { 
         //if(node->_prev != 0)
            _node = _node->_prev;
         return *(this);
      }
      iterator operator -- (int) { 
         iterator tmp(_node);
         //if(node->_prev != 0)
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
   iterator end() const { return iterator(_head); }// my design
   bool empty() const { return (_head == _head->_prev); }
   size_t size() const { 
      int _size = 0;
      iterator i = iterator(begin());
      iterator _end(end());
      for(; i != _end; i++)
         _size++;
      return _size;
   }

   void push_back(const T& x) {
      if(empty()) {
         DListNode<T>* tmp = new DListNode<T>(x, _head, _head);
         _head->_prev = tmp;
         _head->_next = tmp;
         _isSorted = true;
      } else {
         DListNode<T>* _myend = _head->_prev;
         DListNode<T>* tmp = new DListNode<T>(x, _myend, _head);
         _head->_prev = tmp;
         _myend->_next = tmp;
         _isSorted = false;
      }
   }
   void pop_front() { erase(begin()); }
   void pop_back() { erase(--end());}

   // return false if nothing to erase
   bool erase(iterator pos) { 
      if(pos == 0 || pos._node == 0 || empty()) return false;
      
      // no need to check validity: whether pos == end(), IN AH's CASE, pos == begin()

      (pos._node->_prev)->_next = pos._node->_next;
      (pos._node->_next)->_prev = pos._node->_prev;
      delete pos._node;
      pos._node = 0;

      _isSorted = false;
      return true; 
   }
   bool erase(const T& x) { 
      if(empty()) return false;
      
      iterator i(begin()), _end(--end()); //dummy begin
      bool is_ended = false;
      
      while(!is_ended) {
         if(i == _end) is_ended = true;
         
         if(i._node->_data == x)
            return erase(i);
         i++;
      }

      return false;
   }

   void clear() { 
      if(empty()) return;
      
      iterator i(begin()), _end(--end());
      bool is_ended = false;
      while(!is_ended) {
         if(i == _end) is_ended = true;
         erase(i);
         
         i++;
      }
      _isSorted = true; // since there is no nodes now
   }  // delete all nodes except for the dummy node

   void sort() const { 
      _isSorted = true;
      if(empty()) return;

      iterator i(begin()), _end(--end());
      bool is_ended = false;
      // insertion sort
      while(!is_ended) {
         if(i == _end) is_ended = true;
         T tmp = *i;
         iterator j = i;
         while (true) {
            if (j == begin()) {
               *j = tmp;
               break;
            }
            iterator k = j;
            --j;
            if (tmp < *j) {
               *k = *j;
               continue;
            }
            *k = tmp;
            break;
         }

         i++;
      }
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
