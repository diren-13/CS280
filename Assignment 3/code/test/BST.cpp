/************************************************************************************//*!
 @file        BST.cpp
 @author      Diren (diren.dbharwani@digipen.edu)
 @brief       Implementation of an integer BST.
 @date        2022-03-02
 
 @copyright Copyright (c) 2022
*//*************************************************************************************/

#include <iostream>

#include "BST.h"

BSTNode::BSTNode()
: Left          { nullptr }
, Right         { nullptr }
, Value         { 0 }
, BalanceFactor { 0 }
, Count         { 1 }
{}

BSTNode::BSTNode(int v)
: Left          { nullptr }
, Right         { nullptr }
, Value         { v }
, BalanceFactor { 0 }
, Count         { 1 }
{}

bool BSTNode::IsLeaf() const
{
    return Left == nullptr && Right == nullptr;
}

BST::BST()
: root { nullptr }
{}

BST::~BST()
{
    removeNode(root);
}

const BSTNode* BST::operator[](int index) const
{
    return findNode(root, index);
}

const BSTNode& BST::GetRoot() const
{
    return *root;
}

int BST::Height() const
{
    return subTreeHeight(root);
}

int BST::Size() const
{
    return subTreeSize(root);
}

bool BST::Empty() const
{
    return Height() < 0;
}

void BST::Insert(int value)
{
    root = insert(root, value);
}

void BST::Remove(int value)
{
    root = remove(root, value);
}

bool BST::Find(int value) const
{
    return find(root, value);
}

void BST::PreOrder() const
{
    preOrder(root);
}

void BST::InOrder() const
{
    inOrder(root);
}

void BST::PostOrder() const
{
    postOrder(root);
}

BSTNode* BST::newNode(int value)
{
    BSTNode* node = new BSTNode{value};
    return node;
}

void BST::removeNode(BSTNode* node)
{
    if (node == nullptr)
        return;

    removeNode(node->Left);
    removeNode(node->Right);

    delete node;
}

BSTNode* BST::insert(BSTNode* node, int value)
{
    if (node == nullptr)
        return newNode(value);

    // Smaller values traverse to the left
    if (value < node->Value)
    {
        node->Left = insert(node->Left, value);
    }

    // Larger values traverse to the right
    if (value > node->Value)
    {
        node->Right = insert(node->Right, value);
    }

    ++(node->Count);
    return node;
}

BSTNode* BST::remove(BSTNode* node, int value)
{
    if (node == nullptr)
        return node;

    // Smaller values traverse to the left
    if (value < node->Value)
    {
        node->Left = remove(node->Left, value);
        --(node->Count);
        return node;
    }
    if (value > node->Value)
    {
        node->Right = remove(node->Right, value);
        --(node->Count);
        return node;
    }

    --(node->Count);

    // if leaf
    if (node->IsLeaf())
    {
        delete node;
        return nullptr;
    }

    // Left child is empty
    if (node->Left == nullptr)
    {
        BSTNode* temp = node->Right;
        delete node;
        return temp;
    }
    // Right child is empty
    else if (node->Right == nullptr)
    {
        BSTNode* temp = node->Left;
        delete node;
        return temp;
    }
    // Both left and right child exist
    else
    {
        BSTNode* successor = findSuccessor(node->Right);
        node->Value = successor->Value;
        node->Right= remove(node->Right, successor->Value);
    }

    return node;
}

bool BST::find(BSTNode* node, int value) const
{
    if (node == nullptr)
        return false;

    if (value < node->Value)
        return find(node->Left, value);

    if (value > node-> Value)
        return find(node->Right, value);

    return true;
}

const BSTNode* BST::findNode(BSTNode* node, int value) const
{
    if (node == nullptr)
        return nullptr;

    if (value < node->Left->Count)
        return findNode(node->Left, value);

    if (value > node->Left->Count)
        return findNode(node->Right, value);

    return node;
}

BSTNode* BST::findSuccessor(BSTNode* node)
{
    BSTNode* successor  = node;
    while (successor and successor->Left)
    {
        successor = successor->Left;
    }
    return successor;
}

void BST::preOrder(BSTNode* node) const
{
    if (node == nullptr)
        return;
    else
    {
        std::cout << node->Value << "[" << node->Count << "]" << " ";
        preOrder(node->Left);
        preOrder(node->Right);
    }
}

void BST::inOrder(BSTNode* node) const
{
    if (node == nullptr)
        return;
    else
    {
        inOrder(node->Left);
        std::cout << node->Value << "[" << node->Count << "]" << " ";
        inOrder(node->Right);
    }
}

void BST::postOrder(BSTNode* node) const
{
    if (node == nullptr)
        return;
    else
    {
        postOrder(node->Left);
        postOrder(node->Right);
        std::cout << node->Value << "[" << node->Count << "]" << " ";
    }
}

int BST::subTreeHeight(const BSTNode* node) const
{
    return (node) ? std::max(subTreeHeight(node->Left) + 1, subTreeHeight(node->Right) + 1) : -1;
}

int BST::subTreeSize(const BSTNode* node) const
{
    return (node) ? subTreeSize(node->Left) + subTreeSize(node->Right) + 1 : 0;
}

BSTNode* AVL::insert(BSTNode* node, int value)
{
    if (node == nullptr)
        return newNode(value);
        
    if (value < node->Value)
    {
        node->Left = insert(node->Left, value);
    }
    else if (value > node->Value)
    {
        node->Right = insert(node->Right, value);
    }
    else
    {
        node->BalanceFactor = getBalance(node);
        return node;
    }
    
    // Rotate Right
    if (node->BalanceFactor > 1)
    {
        if (value > node->Left->Value)
        {
            node->Left = rotateLeft(node->Left);
        }
        return rotateRight(node);
    }

    // Rotate Left
    if (node->BalanceFactor < -1)
    {
        if (value < node->Right->Value)
        {
            node->Right = rotateRight(node->Right);
        }

        return rotateLeft(node);
    }

    return node;
}

BSTNode* AVL::remove(BSTNode* node, int value)
{
    if (node == nullptr)
        return node;

    // Smaller values traverse to the left
    if (value < node->Value)
    {
        node->Left = remove(node->Left, value);
    }
    else if (value > node->Value)
    {
        node->Right = remove(node->Right, value);
    }
    else 
    {
        if (node->Left == nullptr || node->Right == nullptr)
        {
            BSTNode* temp = node->Left ? node->Left : node->Right;;

            if (temp == nullptr)
            {
                temp = node;
                node = nullptr;
            }
            else
            {
                *node = *temp;
            }

            delete temp;
        }
        // Both left and right child exist
        else
        {
            BSTNode* successor = findSuccessor(node->Right);
            node->Value = successor->Value;
            node->Right= remove(node->Right, successor->Value);
        }
    }

    if (node == nullptr)
        return node;

    // std::cout << "Balancing from: " << node->Value << std::endl;

    node->BalanceFactor = getBalance(node);

    // Rotate Right
    if (node->BalanceFactor > 1)
    {
        if (getBalance(node->Left) < 0)
        {
            node->Left = rotateLeft(node->Left);
        }

        return rotateRight(node);
    }

    if (node->BalanceFactor < -1)
    {
        if (getBalance(node->Right) > 0)
        {
            node->Right = rotateRight(node->Right);
        }

        return rotateLeft(node);
    }

    return node;
}

int AVL::getBalance(BSTNode* node) const
{
    if (node == nullptr)
        return 0;

    return subTreeHeight(node->Left) - subTreeHeight(node->Right);
}

BSTNode* AVL::rotateLeft(BSTNode* node)
{
    /************
          n
         / \
        x   A
       / \
      B   y
     / \
    C   D
    ************/

    BSTNode* x = node->Right;
    BSTNode* y = x->Left;

    x->Left = node;
    node->Right = y;

    return x;
}

BSTNode* AVL::rotateRight(BSTNode* node)
{
    /************
          n
         / \
        A   x
           / \
          y   B
             / \
            C   D
    ************/

    BSTNode *x = node->Left;
    BSTNode *y = x->Right;

    x->Right = node;
    node->Left = y;
    
    return x;
}


