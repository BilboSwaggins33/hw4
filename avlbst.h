#ifndef AVLBST_H
#define AVLBST_H

#include "bst.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>

struct KeyError {};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data
 * member, plus other additional helper functions. You do NOT need to implement
 * any functionality or add additional data members or helper functions.
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value> {
public:
  // Constructor/destructor.
  AVLNode(const Key &key, const Value &value, AVLNode<Key, Value> *parent);
  virtual ~AVLNode();

  // Getter/setter for the node's height.
  int8_t getBalance() const;
  void setBalance(int8_t balance);
  void updateBalance(int8_t diff);

  // Getters for parent, left, and right. These need to be redefined since they
  // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
  // for more information.
  virtual AVLNode<Key, Value> *getParent() const override;
  virtual AVLNode<Key, Value> *getLeft() const override;
  virtual AVLNode<Key, Value> *getRight() const override;

protected:
  int8_t balance_; // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
 * An explicit constructor to initialize the elements by calling the base class
 * constructor
 */
template <class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key &key, const Value &value,
                             AVLNode<Key, Value> *parent)
    : Node<Key, Value>(key, value, parent), balance_(0) {}

/**
 * A destructor which does nothing.
 */
template <class Key, class Value> AVLNode<Key, Value>::~AVLNode() {}

/**
 * A getter for the balance of a AVLNode.
 */
template <class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const {
  return balance_;
}

/**
 * A setter for the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance) {
  balance_ = balance;
}

/**
 * Adds diff to the balance of a AVLNode.
 */
template <class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff) {
  balance_ += diff;
}

/**
 * An overridden function for getting the parent since a static_cast is
 * necessary to make sure that our node is a AVLNode.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const {
  return static_cast<AVLNode<Key, Value> *>(this->parent_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const {
  return static_cast<AVLNode<Key, Value> *>(this->left_);
}

/**
 * Overridden for the same reasons as above.
 */
template <class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const {
  return static_cast<AVLNode<Key, Value> *>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value> {
public:
  virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
  virtual void remove(const Key &key);                              // TODO
protected:
  virtual void nodeSwap(AVLNode<Key, Value> *n1, AVLNode<Key, Value> *n2);

  // Add helper functions here
  AVLNode<Key, Value> *insertNode(const std::pair<const Key, Value> &new_item);
  void insertFix(AVLNode<Key, Value> *parent, AVLNode<Key, Value> *node);
  void rotateRight(AVLNode<Key, Value> *node);
  void rotateLeft(AVLNode<Key, Value> *node);
  bool isZigZag(AVLNode<Key, Value> *node);
  bool isZigZig(AVLNode<Key, Value> *node);

  void removeFix(AVLNode<Key, Value> *node, int diff);
  void patchTree(AVLNode<Key, Value> *node);
};

template <class Key, class Value>
AVLNode<Key, Value> *
AVLTree<Key, Value>::insertNode(const std::pair<const Key, Value> &new_item) {
  AVLNode<Key, Value> *curr = static_cast<AVLNode<Key, Value> *>(this->root_);
  AVLNode<Key, Value> *newNode =
      new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
  while (curr != nullptr) {
    if (newNode->getKey() < curr->getKey()) {
      if (curr->getLeft() == nullptr) {
        curr->setLeft(newNode);
        newNode->setParent(curr);
        return newNode;
      } else {
        curr = curr->getLeft();
      }
    } else if (newNode->getKey() > curr->getKey()) {
      if (curr->getRight() == nullptr) {
        curr->setRight(newNode);
        newNode->setParent(curr);
        return newNode;
      } else {
        curr = curr->getRight();
      }
    } else {
      curr->setValue(newNode->getValue());
      delete newNode;
      return nullptr;
    }
  }
  return nullptr;
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *node) {
  AVLNode<Key, Value> *child = node->getLeft();
  AVLNode<Key, Value> *parent = node->getParent();
  AVLNode<Key, Value> *a = node->getRight();
  AVLNode<Key, Value> *b = child->getRight();
  AVLNode<Key, Value> *c = child->getLeft();

  child->setParent(parent);
  node->setParent(child);
  if (parent != nullptr) {
    bool isLeft = parent->getLeft() == node;
    isLeft ? parent->setLeft(child) : parent->setRight(child);
  } else {
    this->root_ = child;
  }
  child->setRight(node);
  node->setLeft(b);
  node->setRight(a);
  child->setLeft(c);
  if (a != nullptr) {
    a->setParent(node);
  }
  if (b != nullptr) {
    b->setParent(node);
  }
  if (c != nullptr) {
    c->setParent(child);
  }
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *node) {
  AVLNode<Key, Value> *child = node->getRight();
  AVLNode<Key, Value> *parent = node->getParent();
  AVLNode<Key, Value> *a = node->getLeft();
  AVLNode<Key, Value> *b = child->getLeft();
  AVLNode<Key, Value> *c = child->getRight();
  child->setParent(parent);
  node->setParent(child);
  if (parent != nullptr) {
    bool isLeft = parent->getLeft() == node;
    isLeft ? parent->setLeft(child) : parent->setRight(child);
  } else {
    this->root_ = child;
  }
  child->setLeft(node);
  node->setRight(b);
  node->setLeft(a);
  child->setRight(c);

  if (c != nullptr) {
    c->setParent(child);
  }
  if (b != nullptr) {
    b->setParent(node);
  }
  if (a != nullptr) {
    a->setParent(node);
  }
}

template <class Key, class Value>
bool AVLTree<Key, Value>::isZigZag(AVLNode<Key, Value> *node) {
  if (node->getBalance() == 2) {
    return node->getRight()->getBalance() == -1;
  } else {
    return node->getLeft()->getBalance() == 1;
  }
}

template <class Key, class Value>
bool AVLTree<Key, Value>::isZigZig(AVLNode<Key, Value> *node) {
  if (node->getBalance() == 2) {
    return node->getRight()->getBalance() == 1;
  } else {
    return node->getLeft()->getBalance() == -1;
  }
}

template <class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value> *parent,
                                    AVLNode<Key, Value> *node) {
  if (parent == nullptr || parent->getParent() == nullptr) {
    return;
  }
  AVLNode<Key, Value> *grandParent = parent->getParent();
  bool isLeft = grandParent->getLeft() == parent;
  isLeft ? grandParent->updateBalance(-1) : grandParent->updateBalance(1);

  if (grandParent->getBalance() == 0) {
    return;
  } else if (grandParent->getBalance() == -1 ||
             grandParent->getBalance() == 1) {
    insertFix(grandParent, parent);
  } else if (grandParent->getBalance() == -2 ||
             grandParent->getBalance() == 2) {
    if (isZigZig(grandParent)) {

      isLeft ? rotateRight(grandParent) : rotateLeft(grandParent);

      grandParent->setBalance(0);
      parent->setBalance(0);
      return;

    } else if (isZigZag(grandParent)) {
      isLeft ? rotateLeft(parent) : rotateRight(parent);
      isLeft ? rotateRight(grandParent) : rotateLeft(grandParent);
      if (node->getBalance() == 0) {
        parent->setBalance(0);
        grandParent->setBalance(0);
        node->setBalance(0);
      } else if (isLeft ? node->getBalance() == -1 : node->getBalance() == 1) {

        parent->setBalance(0);
        isLeft ? grandParent->setBalance(1) : grandParent->setBalance(-1);
        node->setBalance(0);
      } else if (isLeft ? node->getBalance() == 1 : node->getBalance() == -1) {
        isLeft ? parent->setBalance(-1) : parent->setBalance(1);
        grandParent->setBalance(0);
        node->setBalance(0);
      }

      return;
    }
  }
}

/*
 * Recall: If key is already in the tree, you should
 * overwrite the current value with the updated value.
 */
template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item) {
  // TODO
  if (this->root_ == NULL) {
    this->root_ =
        new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    return;
  }

  AVLNode<Key, Value> *newNode = insertNode(new_item);
  if (newNode != nullptr) {
    AVLNode<Key, Value> *parentNode = newNode->getParent();
    if (parentNode->getBalance() == 1 || parentNode->getBalance() == -1) {
      parentNode->setBalance(0);
    } else if (parentNode->getBalance() == 0) {
      if (newNode == parentNode->getLeft()) {
        parentNode->updateBalance(-1);
      } else {
        parentNode->updateBalance(1);
      }
      insertFix(parentNode, newNode);
    }
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */

template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key &key) {
  // TODO

  AVLNode<Key, Value> *node = static_cast<AVLNode<Key, Value> *>(
      BinarySearchTree<Key, Value>::internalFind(key));
  if (node == nullptr) {
    return;
  }

  int numChildren =
      (node->getLeft() != nullptr) + (node->getRight() != nullptr);
  if (numChildren == 2) {
    AVLNode<Key, Value> *predecessor = static_cast<AVLNode<Key, Value> *>(
        BinarySearchTree<Key, Value>::predecessor(node));
    nodeSwap(node, predecessor);
  }

  int diff = 0;
  AVLNode<Key, Value> *parentNode = node->getParent();

  if (parentNode != nullptr) {
    bool isLeft = parentNode->getLeft() == node;
    diff = isLeft ? 1 : -1;
  }

  patchTree(node);
  removeFix(parentNode, diff);
}

template <class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value> *n1,
                                   AVLNode<Key, Value> *n2) {
  BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
  int8_t tempB = n1->getBalance();
  n1->setBalance(n2->getBalance());
  n2->setBalance(tempB);
}

template <class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> *node, int diff) {

  if (node == nullptr) {
    return;
  }

  AVLNode<Key, Value> *parent = node->getParent();
  int nextDiff = 0;
  bool isLeft = diff == 1 ? true : false;
  if (parent != nullptr) {
    bool leftC = parent->getLeft() == node;
    nextDiff = leftC ? 1 : -1;
  }

  if (node->getBalance() + diff == (isLeft ? 2 : -2)) {
    AVLNode<Key, Value> *child = isLeft ? node->getRight() : node->getLeft();

    if (child->getBalance() == (isLeft ? 1 : -1)) {

      isLeft ? rotateLeft(node) : rotateRight(node);
      node->setBalance(0);
      child->setBalance(0);
      removeFix(parent, nextDiff);

    } else if (child->getBalance() == 0) {

      isLeft ? rotateLeft(node) : rotateRight(node);
      isLeft ? node->setBalance(1) : node->setBalance(-1);
      isLeft ? child->setBalance(-1) : child->setBalance(1);

      return;

    } else if (child->getBalance() == (isLeft ? -1 : 1)) {

      AVLNode<Key, Value> *grandChild =
          isLeft ? child->getLeft() : child->getRight();

      isLeft ? rotateRight(child) : rotateLeft(child);
      isLeft ? rotateLeft(node) : rotateRight(node);

      if (grandChild->getBalance() == 0) {

        node->setBalance(0);
        child->setBalance(0);

      } else if (grandChild->getBalance() == (isLeft ? 1 : -1)) {
        isLeft ? node->setBalance(-1) : node->setBalance(1);
        child->setBalance(0);

      } else if (grandChild->getBalance() == (isLeft ? -1 : 1)) {
        node->setBalance(0);
        isLeft ? child->setBalance(1) : child->setBalance(-1);
      }

      grandChild->setBalance(0);
      removeFix(parent, nextDiff);
    }

  } else if (node->getBalance() + diff == (isLeft ? 1 : -1)) {
    isLeft ? node->setBalance(1) : node->setBalance(-1);
    return;

  } else if (node->getBalance() + diff == 0) {

    node->setBalance(0);

    removeFix(parent, nextDiff);
  }
}

template <class Key, class Value>
void AVLTree<Key, Value>::patchTree(AVLNode<Key, Value> *node) {
  if (node == nullptr) {
    return;
  }

  int numChildren =
      (node->getLeft() != nullptr) + (node->getRight() != nullptr);

  if (numChildren == 0) {
    AVLNode<Key, Value> *parent = node->getParent();
    if (parent != nullptr) {
      if (parent->getParent() == nullptr) {
        this->root_ = parent;
      }
      if (node == parent->getLeft()) {
        parent->setLeft(nullptr);
      } else {
        parent->setRight(nullptr);
      }
    } else {
      this->root_ = nullptr;
    }
    delete node;
  } else if (numChildren == 1) {
    AVLNode<Key, Value> *parent = node->getParent();
    AVLNode<Key, Value> *child;
    bool childLeft = node->getLeft() != nullptr;
    if (childLeft) {
      child = node->getLeft();
      child->setParent(parent);
    } else {
      child = node->getRight();
      child->setParent(parent);
    }

    if (parent != nullptr) {
      bool itemLeft = parent->getLeft() == node;
      itemLeft ? parent->setLeft(child) : parent->setRight(child);
    } else {
      this->root_ = child;
    }

    delete node;
  }
}

#endif
