#ifndef RB_TREE_NODE
#define RB_TREE_NODE

#include <iostream>
using namespace std;

template <class key_t, class val_t>
class RBTreeNode
{
  protected:
   key_t key;
   val_t value;

   //True for red
   //False for false
   bool color;

   RBTreeNode<key_t, val_t>* leftChild;
   RBTreeNode<key_t, val_t>* rightChild;
   RBTreeNode<key_t, val_t>* parent;

  public:
   //Initializes all members
   RBTreeNode(const key_t& key, const val_t& value, bool color, RBTreeNode<key_t, val_t>* leftChild, RBTreeNode<key_t, val_t>* rightChild, RBTreeNode<key_t, val_t>* parent);
   //Same as above, but value is initialized using its default
   //constructor
   RBTreeNode(const key_t& key, bool color, RBTreeNode<key_t, val_t>* leftChild, RBTreeNode<key_t, val_t>* rightChild, RBTreeNode<key_t, val_t>* parent);
   //All members initialized using their default constructors
   //Makes color = false (black), and points = 0
   RBTreeNode();

   virtual const key_t& getKey() const;
   virtual void setKey(const key_t& newKey);

   virtual const val_t& getValue() const;   
   virtual void setValue(const val_t& newValue);

   virtual void setColor(bool newColor);
   virtual bool getColor() const;

   virtual RBTreeNode<key_t, val_t>* getLeftChild() const;
   virtual RBTreeNode<key_t, val_t>* getRightChild() const;
   virtual void setLeftChild(RBTreeNode<key_t, val_t>* child);
   virtual void setRightChild(RBTreeNode<key_t, val_t>* child);

   virtual RBTreeNode<key_t, val_t>* getParent();
   virtual void setParent(RBTreeNode<key_t, val_t>* newParent);

};

////////////////////////////////////////////
//Implementation of RBTreeNode begins here//
////////////////////////////////////////////

template <class key_t, class val_t>
RBTreeNode<key_t, val_t>::RBTreeNode(const key_t& key, const val_t& value, bool color, RBTreeNode<key_t, val_t>* leftChild, RBTreeNode<key_t, val_t>* rightChild, RBTreeNode<key_t, val_t>* parent)
{
   this->key = key;
   this->value = value;
   this->color = color;
   this->leftChild = leftChild;
   this->rightChild = rightChild;
   this->parent = parent;
}

template <class key_t, class val_t>
RBTreeNode<key_t, val_t>::RBTreeNode(const key_t& key, bool color, RBTreeNode<key_t, val_t>* leftChild, RBTreeNode<key_t, val_t>* rightChild, RBTreeNode<key_t, val_t>* parent) : value()
{
   this->key = key;
   this->color = color;
   this->leftChild = leftChild;
   this->rightChild = rightChild;
   this->parent = parent;
}

template <class key_t, class val_t>
RBTreeNode<key_t, val_t>::RBTreeNode() : key(), value(), color(), leftChild(), rightChild(), parent()
{
}

template <class key_t, class val_t>
const key_t& RBTreeNode<key_t, val_t>::getKey() const
{
   return key;
}

template <class key_t, class val_t>
void RBTreeNode<key_t, val_t>::setKey(const key_t& newKey)
{
   key = newKey;
}

template <class key_t, class val_t>
const val_t& RBTreeNode<key_t, val_t>::getValue() const
{
   return value;
}

template <class key_t, class val_t>
void RBTreeNode<key_t, val_t>::setValue(const val_t& newValue)
{
   value = newValue;
}

template <class key_t, class val_t>
bool RBTreeNode<key_t, val_t>::getColor() const
{
   return color;
}

template <class key_t, class val_t>
void RBTreeNode<key_t, val_t>::setColor(bool newColor)
{
   color = newColor;
}


template <class key_t, class val_t>
RBTreeNode<key_t, val_t>* RBTreeNode<key_t, val_t>::getLeftChild() const
{
   return leftChild;
}

template <class key_t, class val_t>
RBTreeNode<key_t, val_t>* RBTreeNode<key_t, val_t>::getRightChild() const
{
   return rightChild;
}

template <class key_t, class val_t>
void RBTreeNode<key_t, val_t>::setLeftChild(RBTreeNode<key_t, val_t>* child)
{
   leftChild = child;
}

template <class key_t, class val_t>
void RBTreeNode<key_t, val_t>::setRightChild(RBTreeNode<key_t, val_t>* child)
{
   rightChild = child;
}

template <class key_t, class val_t>
RBTreeNode<key_t, val_t>* RBTreeNode<key_t, val_t>::getParent()
{
   return parent;
}

template <class key_t, class val_t>
void RBTreeNode<key_t, val_t>::setParent(RBTreeNode<key_t, val_t>* newParent)
{
   parent = newParent;
}

#endif
