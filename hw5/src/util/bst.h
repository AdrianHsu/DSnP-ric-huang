/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& v, BSTreeNode<T>* p = NULL, BSTreeNode<T>* l = NULL, 
         BSTreeNode<T>* r = NULL): _key(v), _p(p), _left(l), _right(r) {}

   T _key;
   BSTreeNode<T>* _p;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   public:
      BSTree() {
         _root = new BSTreeNode<T>(T());
      }
      ~BSTree() { delete _root; }
      class iterator { 
         friend class BSTree;

         public:
         iterator(BSTreeNode<T>* n = 0): _node(n){}
         iterator(const iterator& i) : _node(i._node){}
         ~iterator(){}
         const T& operator * () const { return *(this); }
         T& operator * () { return _node->_key; }
         iterator& operator ++ () { 
            _node = _node->_right;
            return *(this);
         }
         iterator operator ++ (int) { 
            iterator tmp(_node);
            _node = _node->_right;
            return tmp; 
         }
         iterator& operator -- () { 
            _node = _node->_left;
            return *(this); 
         }
         iterator operator -- (int) { 
            iterator tmp(_node);
            _node = _node->_left;
            return tmp; 
         }
         iterator& operator = (const iterator& i) { 
            _node = i._node;
            return *(this); 
         }
         bool operator != (const iterator& i) const { return (_node != i._node); }
         bool operator == (const iterator& i) const { return (_node == i._node); }
         
         private:
         BSTreeNode<T>* _node;
      };
      
      iterator begin() const { return 0; }
      iterator end() const { return 0; }
      bool empty() const { return false; }
      size_t size() const {  return 0; }
      void push_back(const T& x) { }
      void pop_front() { }
      void pop_back() { }
      bool erase(iterator pos) { return false; }
      bool erase(const T& x) { return false; }
      void clear() { }  // delete all nodes except for the dummy node
      void sort() const { } //still required

      void insert(const T& x) const { }
      void print() const { }

   private:
      BSTreeNode<T>* _root;

      // [OPTIONAL TODO] helper functions; called by public member functions
      BSTreeNode<T>* treeSearch(BSTreeNode<T>* x, T& k) { 
         if(x == NULL || k == x->_key)
            return x;
         if(k < x->_key)
            return treeSearch(x->_left, k);
         else return treeSearch(x->_right, k);
      }
      BSTreeNode<T>* treeMinimum(BSTreeNode<T>* x) { 
         while(x->_left != NULL)
            x = x->_left;
         return x;
      }
      BSTreeNode<T>* treeMaximum(BSTreeNode<T>* x) { 
         while(x->_right != NULL)
            x = x->_right;
         return x;
      }
      BSTreeNode<T>* treeSuccessor(){ return _root; }
      
      void treeInsert(){}
      void transPlant(){}
      void treeDelete(){}

};

#endif // BST_H
