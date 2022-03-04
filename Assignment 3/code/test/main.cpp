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
    // int nums[9] = {2,,8,3,5,1,4,6,9,7};

    // BST tree;
    // for (int i = 0; i < 10; ++i)
    // {
    //     std::cout << "Inserting: " << nums[i] << std::endl;
    //     tree.Insert(nums[i]);
    // }

    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Clear();
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(6);
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(8);
    // tree.PreOrder();
    // std::cout << std::endl;

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

    int nums[10] = {0,1,2,3,4,5,6,7,8,9};

    AVL tree;
    // std::cout << "Is Tree Empty? " << (tree.Empty() ? "Yes" : "No") << std::endl;

    for (int i = 0; i < 10; ++i)
    {
        tree.Insert(nums[i]);
        tree.PreOrder();
        std::cout << std::endl;
    }

    for (int i = 0; i < 12; ++i)
    {
        tree.Remove(nums[i]);
        tree.PreOrder();
        std::cout << std::endl;
    }
    // std::cout << "Is Tree Empty? " << (tree.Empty() ? "Yes" : "No") << std::endl;
    // std::cout << "Height: " << tree.Height() << std::endl;
    // std::cout << "Size: " << tree.Size() << std::endl;

    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(7);
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(6);
    // tree.PreOrder();
    // std::cout << std::endl;

    // tree.Remove(5);
    // std::cout << "PreOrder: ";
    // tree.PreOrder();
    // std::cout << std::endl;
}