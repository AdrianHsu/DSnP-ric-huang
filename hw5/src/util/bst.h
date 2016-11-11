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
      ~BSTree() { clear(); delete _root; }
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
      BSTreeNode<T>* treeSuccessor(BSTreeNode<T>* x) { 
         if(x->_right != NULL)
            return treeMinimum(x->_right);
         BSTreeNode<T>* y(x->_p);
         while(y != NULL && x == y->_right) {
            x = y;
            y = y->_p;
         }
         return y; 
      }
      
      void treeInsert(BSTreeNode<T>* z) {
         BSTreeNode<T>* y(NULL);
         BSTreeNode<T>* x(_root);
         while(x != NULL) {
            y = x;
            if(z->_key < x->_key)
               x = x->_left;
            else x = x->_right;
         }
         z->_p = y;
         if(y == NULL)
            _root = z;
         else if(z->_key < y->_key)
            y->_left = z;
         else y->_right = z;
      }
      void transPlant(BSTreeNode<T>* u, BSTreeNode<T>* v){
         if(u->_p == NULL)
            _root = v;
         else if(u == u->_p->_left)
            u->_p->_left = v;
         else u->_p->_right = v;
         if(v != NULL)
            v->_p = u->_p;
      }
      void treeDelete(BSTreeNode<T>* z) {
         if(z->_left == NULL)
            transPlant(z, z->_right);
         else if(z->_right == NULL)
            transPlant(z, z->_left);
         else {
            BSTreeNode<T>* y = treeMinimum(z->_right);
            if(y->_p != z) {
               transPlant(y, y->_right);
               y->_right = z->_right;
               y->_right->_p = y;
            }
            transPlant(z, y);
            y->_left = z->_left;
            y->_left->_p = y;
         }
      }

};

#endif // BST_H
