/************************************************************************************//*!
 @file        main.cpp
 @author      Diren (diren.dbharwani@digipen.edu)
 @brief       Test file for the interger BST.
 @date        2022-03-02
 
 @copyright Copyright (c) 2022
*//*************************************************************************************/

#include <iostream>
#include "BST.h"

int main()
{
    // int bstNumbers[10] = {5,2,0,1,3,4,8,6,7,9};

    // BST tree;
    // for (int i = 0; i < 10; ++i)
    // {
    //     std::cout << "Inserting: " << bstNumbers[i] << std::endl;
    //     tree.Insert(bstNumbers[i]);
    // }

    // tree.PreOrder();
    // std::cout << std::endl;

    // std::cout << tree[3]->Value << std::endl;
    // std::cout << tree[7]->Value << std::endl;

    // const BSTNode* node = tree[9];

    // if (node)
    // {
    //     std::cout << node->Value << std::endl;
    // }
    // else
    // {
    //     std::cout << "Not found!" << std::endl;
    // }

    // std::cout << "InOrder: ";
    // tree.InOrder();
    // std::cout << std::endl;

    // std::cout << "PostOrder: ";
    // tree.PostOrder();
    // std::cout << std::endl;

    // for (int i = 0; i < 7; ++i)
    // {
    //     tree.Remove(bstNumbers[i]);
    // }

    int avlNumbers[9] = {2,5,9,8,1,4,7,3,6};

    AVL tree;
    // std::cout << "Is Tree Empty? " << (tree.Empty() ? "Yes" : "No") << std::endl;

    for (int i = 0; i < 9; ++i)
    {
        std::cout << "Inserting " << avlNumbers[i] << std::endl;
        tree.Insert(avlNumbers[i]);
    }

    // std::cout << "Is Tree Empty? " << (tree.Empty() ? "Yes" : "No") << std::endl;
    // std::cout << "Height: " << tree.Height() << std::endl;
    // std::cout << "Size: " << tree.Size() << std::endl;

    // std::cout << "PreOrder: ";
    tree.PreOrder();
    std::cout << std::endl;

    tree.Remove(7);
    // std::cout << "PreOrder: ";
    tree.PreOrder();
    std::cout << std::endl;

    // tree.Remove(6);
    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(5);
    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;
}