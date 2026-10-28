#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    // Keeps the base class nodeSwap visible (avoids an overloaded-virtual warning)
    using BinarySearchTree<Key, Value>::nodeSwap;

    // Balance of a node = height(right subtree) - height(left subtree)
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int diff);
};

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    // Walk down to find where the new node belongs
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* n = NULL;
    while(n == NULL) {
        if(new_item.first < parent->getKey()) {
            if(parent->getLeft() == NULL) {
                n = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
                parent->setLeft(n);
            }
            else {
                parent = parent->getLeft();
            }
        }
        else if(parent->getKey() < new_item.first) {
            if(parent->getRight() == NULL) {
                n = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
                parent->setRight(n);
            }
            else {
                parent = parent->getRight();
            }
        }
        else {
            // Key already exists, so overwrite the value
            parent->setValue(new_item.second);
            return;
        }
    }

    // Parent was leaning one way, so the new node evens it out
    if(parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
        return;
    }
    if(parent->getLeft() == n) {
        parent->updateBalance(-1);
    }
    else {
        parent->updateBalance(1);
    }
    insertFix(parent, n);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
    if(p == NULL || p->getParent() == NULL) {
        return;
    }
    AVLNode<Key, Value>* g = p->getParent();

    if(g->getLeft() == p) {
        g->updateBalance(-1);
        if(g->getBalance() == 0) {
            return;
        }
        if(g->getBalance() == -1) {
            insertFix(g, p);
            return;
        }
        // g->getBalance() == -2
        if(p->getLeft() == n) {
            // zig-zig
            rotateRight(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            rotateLeft(p);
            rotateRight(g);
            if(n->getBalance() == -1) {
                p->setBalance(0);
                g->setBalance(1);
            }
            else if(n->getBalance() == 0) {
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                p->setBalance(-1);
                g->setBalance(0);
            }
            n->setBalance(0);
        }
    }
    else {
        g->updateBalance(1);
        if(g->getBalance() == 0) {
            return;
        }
        if(g->getBalance() == 1) {
            insertFix(g, p);
            return;
        }
        // g->getBalance() == 2
        if(p->getRight() == n) {
            // zig-zig
            rotateLeft(g);
            p->setBalance(0);
            g->setBalance(0);
        }
        else {
            // zig-zag
            rotateRight(p);
            rotateLeft(g);
            if(n->getBalance() == 1) {
                p->setBalance(0);
                g->setBalance(-1);
            }
            else if(n->getBalance() == 0) {
                p->setBalance(0);
                g->setBalance(0);
            }
            else {
                p->setBalance(1);
                g->setBalance(0);
            }
            n->setBalance(0);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(n == NULL) {
        return;
    }
    if(n->getLeft() != NULL && n->getRight() != NULL) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
        nodeSwap(n, pred);
    }

    // Removing a left child makes the parent lean right (+1), and vice versa
    AVLNode<Key, Value>* parent = n->getParent();
    int diff = 0;
    if(parent != NULL) {
        if(parent->getLeft() == n) {
            diff = 1;
        }
        else {
            diff = -1;
        }
    }

    // n now has at most one child, which gets promoted
    AVLNode<Key, Value>* child = n->getLeft();
    if(child == NULL) {
        child = n->getRight();
    }
    if(child != NULL) {
        child->setParent(parent);
    }
    if(parent == NULL) {
        this->root_ = child;
    }
    else if(parent->getLeft() == n) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }
    delete n;

    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff)
{
    if(n == NULL) {
        return;
    }
    // Work out the next call's diff before any rotation changes n's parent
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0;
    if(p != NULL) {
        if(p->getLeft() == n) {
            ndiff = 1;
        }
        else {
            ndiff = -1;
        }
    }

    int newBalance = n->getBalance() + diff;
    if(diff == -1) {
        if(newBalance == -2) {
            AVLNode<Key, Value>* c = n->getLeft();
            if(c->getBalance() == -1) {
                // zig-zig
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {
                // zig-zig, height unchanged so we are done
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            else {
                // zig-zag
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if(g->getBalance() == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                }
                else {
                    n->setBalance(1);
                    c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if(newBalance == -1) {
            n->setBalance(-1);
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    else {
        if(newBalance == 2) {
            AVLNode<Key, Value>* c = n->getRight();
            if(c->getBalance() == 1) {
                // zig-zig
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if(c->getBalance() == 0) {
                // zig-zig, height unchanged so we are done
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else {
                // zig-zag
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if(g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                }
                else {
                    n->setBalance(-1);
                    c->setBalance(0);
                }
                g->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if(newBalance == 1) {
            n->setBalance(1);
        }
        else {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}

/*
 * Makes n's right child the root of this subtree, with n as its left child
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* c = n->getRight();
    AVLNode<Key, Value>* middle = c->getLeft();

    n->setRight(middle);
    if(middle != NULL) {
        middle->setParent(n);
    }
    c->setLeft(n);
    n->setParent(c);

    c->setParent(p);
    if(p == NULL) {
        this->root_ = c;
    }
    else if(p->getLeft() == n) {
        p->setLeft(c);
    }
    else {
        p->setRight(c);
    }
}

/*
 * Makes n's left child the root of this subtree, with n as its right child
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* p = n->getParent();
    AVLNode<Key, Value>* c = n->getLeft();
    AVLNode<Key, Value>* middle = c->getRight();

    n->setLeft(middle);
    if(middle != NULL) {
        middle->setParent(n);
    }
    c->setRight(n);
    n->setParent(c);

    c->setParent(p);
    if(p == NULL) {
        this->root_ = c;
    }
    else if(p->getLeft() == n) {
        p->setLeft(c);
    }
    else {
        p->setRight(c);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
