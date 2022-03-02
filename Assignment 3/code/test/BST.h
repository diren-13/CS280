/************************************************************************************//*!
 @file        BST.h
 @author      Diren (diren.dbharwani@digipen.edu)
 @brief       Interface of an integer BST.
 @date        2022-03-02
 
 @copyright Copyright (c) 2022
*//*************************************************************************************/

#pragma once

struct BSTNode
{
public:
    BSTNode*    Left;
    BSTNode*    Right;
    int         Value;
    int         BalanceFactor;
    int         Count;

    BSTNode();
    BSTNode(int v);

    bool IsLeaf() const;
};

class BST
{
public:
    BST();
    ~BST();

    const BSTNode*  operator[]      (int index)                 const;

    const BSTNode&  GetRoot         ()                          const;
    int             Height          ()                          const;
    int             Size            ()                          const;
    bool            Empty           ()                          const; 

    void            Insert          (int value);
    void            Remove          (int value);
    bool            Find            (int value)                 const;

    void            PreOrder        ()             const;
    void            InOrder         ()             const;
    void            PostOrder       ()             const;

protected:
    BSTNode*            newNode         (int value);
    void                removeNode      (BSTNode* node);
    virtual BSTNode*    insert          (BSTNode* node, int value);
    virtual BSTNode*    remove          (BSTNode* node, int value);
    bool                find            (BSTNode* node, int value)  const;
    const BSTNode*      findNode        (BSTNode* node, int value)  const;
    BSTNode*            findSuccessor   (BSTNode* node);
    void                preOrder        (BSTNode* node)             const;
    void                inOrder         (BSTNode* node)             const;
    void                postOrder       (BSTNode* node)             const;
    int                 subTreeHeight   (const BSTNode* node)       const;
    int                 subTreeSize     (const BSTNode* node)       const;
    
    BSTNode*    root;
};

class AVL : public BST
{
public:
    AVL() = default;

protected:
    BSTNode*    insert      (BSTNode* node, int value) override;
    BSTNode*    remove      (BSTNode* node, int value) override;

private:
    int         getBalance  (BSTNode* node) const;
    BSTNode*    rotateLeft  (BSTNode* node);
    BSTNode*    rotateRight (BSTNode* node);

};
