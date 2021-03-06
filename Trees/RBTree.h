#ifndef RB_TREE
#define RB_TREE

#include "AssociativeMap.h"
#include "RBTreeNode.h"

#include <string>
#include <fstream>
#include <queue>
#include <map>

template <class key_t, class val_t>
class RBTree : public AssociativeMap<key_t, val_t>
{
  protected:
   RBTreeNode<key_t, val_t>* root;
   RBTreeNode<key_t, val_t>* sentinel;
   int numItems;
   
   //Returns the node associated with given key
   //If there is no such node, returns the node
   //where the search failed.
   //Returns sentinel if the tree is empty.
   RBTreeNode<key_t, val_t>* getNode(const key_t& key) const;

   //Creates a new node with the given key, value,and color
   //and makes it the appropriate child of the given parent.
   //Also increases numItems by one.
   //The new node's children are set to the sentinel
   //If the parent is the sentinel, the new node is made the root
   //Returns a pointer to the newly created node
   RBTreeNode<key_t, val_t>* plant(const key_t& key, const val_t& value, bool color, RBTreeNode<key_t, val_t>* parent);

   //Replaces nodeToReplace with replacementNode in the tree
   //Assumes nodeToReplace is a real node (and not the sentinel!)
   //(replacementNode may be the sentinel)
   virtual void transplant(RBTreeNode<key_t, val_t>* nodeToReplace, RBTreeNode<key_t, val_t>* replacementNode);

   //Performs the rotate left operation
   //(assumes node has a right child)
   virtual void rotateLeft(RBTreeNode<key_t, val_t>* node);

   //Performs the rotate right operation
   //(assumes node has a left child)
   virtual void rotateRight(RBTreeNode<key_t, val_t>* node);

   //Fix up the tree after an insert
   //(assumes insertedNode is red)
   virtual void insertFixup(RBTreeNode<key_t, val_t>* insertedNode);

   //Fix up the tree after deleting a node
   //ReplacementNode is the node that replaced the deleted node
   //(possibly the sentinel)
   virtual void deleteFixup(RBTreeNode<key_t, val_t>* replacementNode);

  public:
   //Creates an empty binary tree
   RBTree();
   virtual ~RBTree();

   //Associates "value" with "key"
   virtual void insert(const key_t& key, const val_t& value);

   //Removes the item associated with "key"
   //If there is no such item, has no effect.
   virtual void remove(const key_t& key);

   //Returns true if there is an item with "key" as its key
   virtual bool contains(const key_t& key) const;

   //Returns the value associated with "key." If there is
   //no such item, adds an item (using the default constructor).
   virtual const val_t& find(const key_t& key);

   virtual int getSize() const;
   virtual bool isEmpty() const;

   //Prints the tree to the given file in the "dot" format for
   //visualization using graphviz
   virtual void printDOT(std::string filename);
};

////////////////////////////////////////
//Implementation of RBTree begins here//
////////////////////////////////////////

template <class key_t, class val_t>
RBTree<key_t, val_t>::RBTree()
{
   root = sentinel = new RBTreeNode<key_t,val_t>();
   sentinel->setColor(0); //color of sentinel is black
   numItems = 0;
}

template <class key_t, class val_t>
RBTree<key_t, val_t>::~RBTree()
{
  queue<RBTreeNode<key_t,val_t>*> myQueue;  
  myQueue.push(root); 
  RBTreeNode<key_t,val_t>* curNode;
  while (root!=sentinel && !myQueue.empty()) //breadth first search
   {
     curNode = myQueue.front(); 
     myQueue.pop();
     if (curNode->getLeftChild()!=sentinel) 
       myQueue.push(curNode->getLeftChild()); 
     if (curNode->getRightChild()!=sentinel) 
       myQueue.push(curNode->getRightChild()); 
     delete curNode; 
   } 
  delete sentinel;
}

template <class key_t, class val_t>
RBTreeNode<key_t, val_t>* RBTree<key_t, val_t>::getNode(const key_t& key) const
{
   RBTreeNode<key_t,val_t>* curNode = root;
   while (curNode != sentinel)
   {
      if(curNode->getKey() == key)
	 return curNode;
      else if (curNode->getKey()>key)
      {
	 if (curNode->getLeftChild()!=sentinel)
	    curNode = curNode->getLeftChild();
	 else
	    return curNode;
      }
      else if (curNode->getKey()<key)
      {
	 if (curNode->getRightChild()!=sentinel)
	    curNode = curNode->getRightChild();
	 else
	    return curNode;
      }
   }
   return sentinel;
}

template<class key_t, class val_t>
RBTreeNode<key_t, val_t>* RBTree<key_t, val_t>::plant(const key_t& key, const val_t& value, bool color, RBTreeNode<key_t, val_t>* parent)
{
   RBTreeNode<key_t,val_t>* newNode = new RBTreeNode<key_t,val_t>(key,value,color,sentinel,sentinel,parent);
   if (parent->getKey()>key)
      parent->setLeftChild(newNode);
   else
      parent->setRightChild(newNode);
   if (parent == sentinel)
      root = newNode;
   numItems++;
   return newNode;
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::insert(const key_t& key, const val_t& value)
{
   RBTreeNode<key_t,val_t>* curNode = getNode(key);
   if (curNode->getKey() == key && curNode!=sentinel)
   {
      curNode->setValue(value);
   }
   else
   {
      RBTreeNode<key_t,val_t>* newNode = plant(key,value,1,curNode);
      insertFixup(newNode);
   }
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::rotateLeft(RBTreeNode<key_t, val_t>* node)
{
   RBTreeNode<key_t,val_t>* curNode = node->getRightChild();
   node->setRightChild(curNode->getLeftChild());
   if (curNode->getLeftChild()!=sentinel)
      curNode->getLeftChild()->setParent(node);
   curNode->setParent(node->getParent());
   if (node->getParent()== sentinel)
      root = curNode;
   else if(node == node->getParent()->getLeftChild())
      node->getParent()->setLeftChild(curNode);
   else
      node->getParent()->setRightChild(curNode);
   curNode->setLeftChild(node);
   node->setParent(curNode);
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::rotateRight(RBTreeNode<key_t, val_t>* node)
{
   RBTreeNode<key_t,val_t>* curNode = node->getLeftChild();
   node->setLeftChild(curNode->getRightChild());
   if (curNode->getRightChild()!=sentinel)
      curNode->getRightChild()->setParent(node);
   curNode->setParent(node->getParent());
   if (node->getParent()== sentinel)
      root = curNode;
   else if(node == node->getParent()->getLeftChild())
      node->getParent()->setLeftChild(curNode);
   else
      node->getParent()->setRightChild(curNode);
   curNode->setRightChild(node);
   node->setParent(curNode);
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::insertFixup(RBTreeNode<key_t, val_t>* insertedNode)
{
   while (insertedNode->getParent()->getColor()) 
   {
      RBTreeNode<key_t, val_t>* uncle;
      if (insertedNode->getParent() == insertedNode->getParent()->getParent()->getLeftChild())
      {
	 uncle =  insertedNode->getParent()->getParent()->getRightChild();
	 if (uncle->getColor())
	 {
	    insertedNode->getParent()->setColor(0);
	    uncle->setColor(0);
	    insertedNode->getParent()->getParent()->setColor(1);
	    insertedNode = insertedNode->getParent()->getParent();
	 }
	 else 
	 {
	    if (insertedNode == insertedNode->getParent()->getRightChild())
	    {
	       insertedNode = insertedNode->getParent();
	       rotateLeft(insertedNode);
	    }
	    insertedNode->getParent()->setColor(0);
	    insertedNode->getParent()->getParent()->setColor(1);
	    rotateRight(insertedNode->getParent()->getParent());
	 }	 
      }
      else
      {
	 uncle =  insertedNode->getParent()->getParent()->getLeftChild();
	 if (uncle->getColor())
	 {
	    insertedNode->getParent()->setColor(0);
	    uncle->setColor(0);
	    insertedNode->getParent()->getParent()->setColor(1);
	    insertedNode = insertedNode->getParent()->getParent();
	 }
	 else 
	 {
	    if (insertedNode == insertedNode->getParent()->getLeftChild())
	    {
	       insertedNode = insertedNode->getParent();
	       rotateRight(insertedNode);
	    }
	    insertedNode->getParent()->setColor(0);
	    insertedNode->getParent()->getParent()->setColor(1);
	    rotateLeft(insertedNode->getParent()->getParent());
	 }	 
      }
   }
   root->setColor(0);
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::transplant(RBTreeNode<key_t, val_t>* nodeToReplace, RBTreeNode<key_t, val_t>* replacementNode)
{
   RBTreeNode<key_t, val_t>* parent = nodeToReplace->getParent();
   replacementNode->setParent(parent);
   if (parent->getLeftChild()==nodeToReplace)
      parent->setLeftChild(replacementNode);
   else
      parent->setRightChild(replacementNode);
   if (parent == sentinel)
      root = replacementNode;
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::remove(const key_t& key)
{
   RBTreeNode<key_t,val_t>* node = getNode(key);
   RBTreeNode<key_t,val_t>* deletedNode = node;
   if (deletedNode->getKey() == key && deletedNode!=sentinel)
   {
      RBTreeNode<key_t,val_t>* replaceNode;
      bool color = deletedNode->getColor();
      if (deletedNode->getLeftChild()==sentinel)
      {
	 replaceNode = deletedNode->getRightChild();
	 transplant(deletedNode,replaceNode);
      }
      else if (deletedNode->getRightChild()==sentinel)
      {
	 replaceNode = deletedNode->getLeftChild();	 
	 transplant(deletedNode,replaceNode);
      }
      else
      {
	 deletedNode = node->getRightChild();
	 while (deletedNode->getLeftChild()!=sentinel)
	    deletedNode = deletedNode->getLeftChild();
	 color = deletedNode->getColor();
	 replaceNode = deletedNode->getRightChild();
	 if (deletedNode->getParent()== node)
	    replaceNode->setParent(deletedNode);
	 else
	 {
	    transplant(deletedNode,replaceNode);
	    deletedNode->setRightChild(node->getRightChild());
	    deletedNode->getRightChild()->setParent(deletedNode);
	 }
	 transplant(node,deletedNode);
	 deletedNode->setLeftChild(node->getLeftChild());
	 deletedNode->getLeftChild()->setParent(deletedNode);
	 deletedNode->setColor(node->getColor());
      }
      delete node;
      numItems--;
      if (color == 0) 
	deleteFixup(replaceNode); 
   }   
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::deleteFixup(RBTreeNode<key_t, val_t>* replacementNode)
{
  RBTreeNode<key_t, val_t>* (RBTreeNode<key_t, val_t>::*getChildL)() const; //method pointers
  RBTreeNode<key_t, val_t>* (RBTreeNode<key_t, val_t>::*getChildR)() const;
  void (RBTree<key_t, val_t>::*rotateL)(RBTreeNode<key_t, val_t>* node);
  void (RBTree<key_t, val_t>::*rotateR)(RBTreeNode<key_t, val_t>* node);
  while (replacementNode!=root && replacementNode->getColor()==0)
  {
    getChildL = &RBTreeNode<key_t, val_t>::getLeftChild;
    getChildR = &RBTreeNode<key_t, val_t>::getRightChild;
    rotateL = &RBTree<key_t,val_t>::rotateLeft;
    rotateR = &RBTree<key_t,val_t>::rotateRight;
    if (replacementNode==replacementNode->getParent()->getRightChild())
      { 
	getChildL = &RBTreeNode<key_t, val_t>::getRightChild;
	getChildR = &RBTreeNode<key_t, val_t>::getLeftChild;
	rotateL = &RBTree<key_t,val_t>::rotateRight;
	rotateR = &RBTree<key_t,val_t>::rotateLeft;
      }
     RBTreeNode<key_t,val_t>* siblingNode = (replacementNode->getParent()->*getChildR)();
     if (siblingNode->getColor()==1)
     {
	siblingNode->setColor(0);
	replacementNode->getParent()->setColor(1);
	(this->*rotateL)(replacementNode->getParent());
	siblingNode = (replacementNode->getParent()->*getChildR)();
     }
     if ((siblingNode->*getChildL)()->getColor()==0 && (siblingNode->*getChildR)()->getColor()==0)
     {
	siblingNode->setColor(1);
	replacementNode = replacementNode->getParent();
     }
     else
     {
	if ((siblingNode->*getChildR)()->getColor() == 0)
	{
	   (siblingNode->*getChildL)()->setColor(0);
	   siblingNode->setColor(1);
	   (this->*rotateR)(siblingNode);
	   siblingNode = (replacementNode->getParent()->*getChildR)();
	}
	siblingNode->setColor(replacementNode->getParent()->getColor());
	replacementNode->getParent()->setColor(0);
	(siblingNode->*getChildR)()->setColor(0);
	(this->*rotateL)(replacementNode->getParent());
	replacementNode = root;
     }
  }
  replacementNode->setColor(0);
}

template <class key_t, class val_t>
bool RBTree<key_t, val_t>::contains(const key_t& key) const
{
  RBTreeNode<key_t,val_t>* node = getNode(key);
  if (node!=sentinel && node->getKey()==key)
        return true;
  return false;
}

template <class key_t, class val_t>
const val_t& RBTree<key_t, val_t>::find(const key_t& key)
{
   RBTreeNode<key_t,val_t>* curNode = getNode(key);
   if (curNode->getKey() == key && curNode!=sentinel)
   {
      return curNode->getValue();
   }
   else
   {
     RBTreeNode<key_t,val_t>* newNode = plant(key,val_t(),1,curNode);
     insertFixup(newNode);
   }
   return sentinel->getValue(); //value of the sentinel was created with default constructor too
}

template <class key_t, class val_t>
int RBTree<key_t, val_t>::getSize() const
{
   return numItems;
}

template <class key_t, class val_t>
bool RBTree<key_t, val_t>::isEmpty() const
{
   return numItems == 0;
}

template <class key_t, class val_t>
void RBTree<key_t, val_t>::printDOT(std::string filename)
{
   using namespace std;
   ofstream fout(filename.c_str());

   long long counter = 1;
   map<RBTreeNode<key_t, val_t>*, long long> nodeToCounter;
   nodeToCounter[sentinel] = counter;
   counter++;

   fout << "digraph RBTree {" << endl;

   fout << "\t node" << nodeToCounter[sentinel] << " [label=\"nil\", style=filled, color=grey, fontcolor=white];" << endl;

   if(root != sentinel)
   {
      if(!nodeToCounter[root])
      {
	 nodeToCounter[root] = counter;
	 counter++;
      }
      if(!nodeToCounter[root->getParent()])
      {
	 nodeToCounter[root->getParent()] = counter;
	 counter++;
      }
      fout << "\t node" << nodeToCounter[root] << " -> node" << nodeToCounter[root->getParent()] << " [constraint=false, tailport=n, headport=s";
      if(root->getParent() == sentinel)
      {
	 fout << ", color=grey];" << endl;
      }
      else
      {
	 //The root's parent is something weird!! Make the arrow orange so it sticks out
	 fout << ", color=orange];" << endl;
      }

      map<RBTreeNode<key_t, val_t>*, bool> printed;
   
      queue<RBTreeNode<key_t, val_t>* > q;
      q.push(root);
      while(!q.empty())
      {
	 RBTreeNode<key_t, val_t>* node = q.front();
	 q.pop();

	 printed[node] = true;
      
	 //Set up node's appearance
	 if(!nodeToCounter[node])
	 {
	    nodeToCounter[node] = counter;
	    counter++;
	 }
	 fout << "\t node" << nodeToCounter[node] << " [label=\"k: " << node->getKey() << "\\nv: " << node->getValue() << "\", style=filled, color=";
	 if(node->getColor())
	 {
	    fout << "red";
	 }
	 else
	 {
	    fout << "black, fontcolor=white";
	 }
	 fout << "];" << endl;

	 //Set up child links
	 //First the left node
	 if(!nodeToCounter[node->getLeftChild()])
	 {
	    nodeToCounter[node->getLeftChild()] = counter;
	    counter++;
	 }
	 fout << "\t node" << nodeToCounter[node] << " -> node" << nodeToCounter[node->getLeftChild()] << " [tailport=sw";    
	 if(node->getLeftChild() == sentinel)
	 {
	    fout << ", color=grey";
	 }
	 else if(printed[node->getLeftChild()])
	 {
	    fout << ", color=orange";
	 }
	 else
	 {
	    q.push(node->getLeftChild());
	 }

	 if(node->getLeftChild()->getParent() == node)
	 {
	    //The child's parent pointer points right back,
	    //so draw the second arrow
	    fout << ", dir=both];" << endl;
	 }
	 else if(node->getLeftChild() != sentinel)
	 {
	    //The child node's parent pointer is not correct!
	    //Draw it in orange so it sticks out
	    fout << "];" << endl;
	    if(!nodeToCounter[node->getLeftChild()->getParent()])
	    {
	       nodeToCounter[node->getLeftChild()->getParent()] = counter;
	       counter++;
	    }
	    fout << "\t node" << nodeToCounter[node->getLeftChild()] << " -> node" << nodeToCounter[node->getLeftChild()->getParent()] << " [tailport=n, color=orange];" << endl;
	 }
	 else
	 {
	    fout << "];" << endl;
	 }

	 //Now the right node
	 if(!nodeToCounter[node->getRightChild()])
	 {
	    nodeToCounter[node->getRightChild()] = counter;
	    counter++;
	 }
	 fout << "\t node" << nodeToCounter[node] << " -> node" << nodeToCounter[node->getRightChild()] << " [tailport=se";    
	 if(node->getRightChild() == sentinel)
	 {
	    fout << ", color=grey";
	 }
	 else if(printed[node->getRightChild()])
	 {
	    fout << ", color=orange";
	 }
	 else
	 {
	    q.push(node->getRightChild());
	 }

	 if(node->getRightChild()->getParent() == node)
	 {
	    //The child's parent pointer points right back,
	    //so draw the second arrow
	    fout << ", dir=both];" << endl;
	 }
	 else if(node->getRightChild() != sentinel)
	 {
	    //The child node's parent pointer is not correct!
	    //Draw it in orange so it sticks out
	    fout << "];" << endl;
	    if(!nodeToCounter[node->getRightChild()->getParent()])
	    {
	       nodeToCounter[node->getRightChild()->getParent()] = counter;
	       counter++;
	    }
	    fout << "\t node" << nodeToCounter[node->getRightChild()] << " -> node" << nodeToCounter[node->getRightChild()->getParent()] << " [tailport=n, color=orange];" << endl;
	 }
	 else
	 {
	    fout << "];" << endl;
	 }
      }
   }
   fout << "}" << endl;
}

#endif
