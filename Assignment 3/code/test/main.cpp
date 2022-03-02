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
    int bstNumbers[7] = {4,2,1,3,6,5,7};

    BST tree;
    for (int i = 0; i < 7; ++i)
    {
        std::cout << "Inserting: " << bstNumbers[i] << std::endl;
        tree.Insert(bstNumbers[i]);
    }

    tree.PreOrder();

    // std::cout << "InOrder: ";
    // tree.InOrder();
    // std::cout << std::endl;

    // std::cout << "PostOrder: ";
    // tree.PostOrder();
    // std::cout << std::endl;

    for (int i = 0; i < 7; ++i)
    {
        std::cout << "Removing: " << bstNumbers[i] << std::endl;
        tree.Remove(bstNumbers[i]);
        tree.PreOrder();
        std::cout << std::endl;
    }

    // int avlNumbers[7] = {6,4,5,7,2,1,3};

    // AVL tree;
    // std::cout << "Is Tree Empty? " << (tree.Empty() ? "Yes" : "No") << std::endl;

    // for (int i = 0; i < 7; ++i)
    // {
    //     std::cout << "Inserting " << avlNumbers[i] << std::endl;
    //     tree.Insert(avlNumbers[i]);
    // }

    // std::cout << "Is Tree Empty? " << (tree.Empty() ? "Yes" : "No") << std::endl;
    // std::cout << "Height: " << tree.Height() << std::endl;
    // std::cout << "Size: " << tree.Size() << std::endl;

    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(7);
    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(6);
    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(5);
    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;
}