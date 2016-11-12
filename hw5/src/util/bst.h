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
         _root = NULL;
         _size = 0;
      }
      ~BSTree() { clear(); }
      class iterator { 
         friend class BSTree;

         public:
         iterator(BSTreeNode<T>* n = 0): _node(n){}
         iterator(const iterator& i) : _node(i._node){}
         ~iterator(){}
         const T& operator * () const { return *(this); }
         T& operator * () { return _node->_key; }
         iterator& operator ++ () { 
            //find successor
            if(_node->_right != NULL) {
               _node = _node->_right;
               //treeMinimum
               while(_node->_left != NULL)
                  _node = _node->_left;
               return *(this);
            }
            BSTreeNode<T>* y(_node->_p);
            while(y != NULL && _node == y->_right) {
               _node = y;
               y = y->_p;
            }
            _node = y;
            return *(this); 
         }
         iterator operator ++ (int) { 
            iterator tmp(_node);
            //find successor
            if(_node->_right != NULL) {
               _node = _node->_right;
               //treeMinimum
               while(_node->_left != NULL)
                  _node = _node->_left;
               return *(this);
            }
            BSTreeNode<T>* y(_node->_p);
            while(y != NULL && _node == y->_right) {
               _node = y;
               y = y->_p;
            }
            _node = y;
            return tmp; 
         }
         iterator& operator -- () { 
            //find predecessor
            if(_node->_left != NULL) {
               _node = _node->_left;
               while(_node->_right != NULL)
                  _node = _node->_right;
               return *(this);
            }
            BSTreeNode<T>* y(_node->_p);
            while(y != NULL && _node == y->_left) {
               _node = y;
               y = y->_p;
            }
            _node = y;
            return *(this); 
         }
         iterator operator -- (int) { 
            iterator tmp(_node);
            //find predecessor
            if(_node->_left != NULL) {
               _node = _node->_left;
               while(_node->_right != NULL)
                  _node = _node->_right;
               return *(this);
            }
            BSTreeNode<T>* y(_node->_p);
            while(y != NULL && _node == y->_left) {
               _node = y;
               y = y->_p;
            }
            _node = y;
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
      
      iterator begin() const { return iterator(treeMinimum(_root)); }
      iterator end() const { 
         if(treeMaximum(_root) != NULL)
            return iterator(treeMaximum(_root)->_right); 
         else
            return NULL;
      }
      bool empty() const { return (_size == 0); }
      size_t size() const {  return _size; }
      //void push_back(const T& x) { }
      void pop_front() { treeDelete(treeMinimum(_root), _size); }
      void pop_back() { treeDelete(treeMaximum(_root), _size); }
      bool erase(iterator pos) { 
         if(empty()) return false;
         BSTreeNode<T>* node = pos._node;
         //BSTreeNode<T>* node = treeSearch(_root, pos._node->_key);
         if(node != NULL) 
            return treeDelete(node, _size);
         else return false;
      }
      bool erase(const T& v) { 
         BSTreeNode<T>* node = treeSearch(_root, v);
         if(node != NULL) 
            return treeDelete(node, _size);
         else return false;
      }
      void clear() { 
         if(empty()) return;
         iterator i(begin());
         for( ; i != end(); i++)
            pop_front();

         _root = 0;
      }  // delete all nodes except for the dummy node
      void sort() const { return; } //still required

      void insert(const T& v) { 
         BSTreeNode<T>* z = new BSTreeNode<T>(v, 0, 0, 0);
         treeInsert(_root, z, _size);
      }
      void print() const { 
         preorderTreeWalk(_root, 0);
      }

   private:
      BSTreeNode<T>* _root;
      size_t _size;
      // [OPTIONAL TODO] helper functions; called by public member functions
      void preorderTreeWalk(BSTreeNode<T>* x, size_t l) const {
         
         for (size_t i = 0; i < l; ++i)   cout << "  ";
         if (x == NULL) cout << "[0]\n";
         else {
            cout << x->_key << endl;
            preorderTreeWalk(x->_left, l + 1);
            preorderTreeWalk(x->_right, l + 1);
         }
      }

      BSTreeNode<T>* treeSearch(BSTreeNode<T>* x,const T& k) const { 
         while(x != NULL && k != x->_key) {
            if(k < x->_key)
               x = x->_left;
            else x = x->_right;
         }
         return x;
      }
      BSTreeNode<T>* treeMinimum(BSTreeNode<T>* x) const { 
         if(x == NULL) return NULL; //root not exist
         while(x->_left != NULL)
            x = x->_left;
         return x;
      }
      BSTreeNode<T>* treeMaximum(BSTreeNode<T>* x) const {
         if(x == NULL) return NULL;
         while(x->_right != NULL)
            x = x->_right;
         return x;
      }
      BSTreeNode<T>* treeSuccessor(BSTreeNode<T>* x) { 
         if(x->_right != NULL) {
            x = x->_right;
            //treeMinimum
            while(x->_left != NULL)
               x = x->_left;
            return x;
         }
         BSTreeNode<T>* y(x->_p);
         while(y != NULL && x == y->_right) {
            x = y;
            y = y->_p;
         }
         return y; 
      }
      BSTreeNode<T>* treePredecessor(BSTreeNode<T>* x) { 
         if(x->_left != NULL) {
            x = x->_left;
            while(x->_right != NULL)
               x = x->_right;
            return x;
         }
         BSTreeNode<T>* y(x->_p);
         while(y != NULL && x == y->_left) {
            x = y;
            y = y->_p;
         }
         return y; 
      }

      void treeInsert(BSTreeNode<T>*& _root, BSTreeNode<T>* z, size_t& _size) {
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
         
         _size++;
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
      bool treeDelete(BSTreeNode<T>* z, size_t& _size) {
         if(z == NULL) return false;
         if(z->_left == NULL)
            transPlant(z, z->_right);
         else if(z->_right == NULL)
            transPlant(z, z->_left);
         else {
            //treeMinimum
            BSTreeNode<T>* y(z->_right);
            while(y->_left != NULL)
               y = y->_left;

            if(y->_p != z) {
               transPlant(y, y->_right);
               y->_right = z->_right;
               y->_right->_p = y;
            }
            transPlant(z, y);
            y->_left = z->_left;
            y->_left->_p = y;
         }
         
         _size--;
         return true;
      }

};

#endif // BST_H
