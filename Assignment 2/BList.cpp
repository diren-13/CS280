/************************************************************************************//*!
\file           BList.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Jan 19, 2022
\brief          Contains the implementation of the BList class.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "BList.h"
// Standard Libraries
#include <exception>
#include <cstring>

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
BList<T, Size>::BList()
: head  { nullptr }
, tail  { nullptr }
, stats {}
{
    try
    {
        // Allocate a single node.
        head = tail = new BNode;
        head->count = 0;
        ++stats.NodeCount;
        
        stats.NodeSize    = sizeof(BNode);
        stats.ArraySize   = static_cast<int>(Size);
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
BList<T, Size>::BList(const BList<T, Size>& rhs)
: head  { nullptr }
, tail  { nullptr }
, stats {}
{
    try
    {
        int toAlloc = rhs.stats.NodeCount;

        // Allocate the first node
        head = tail = new BNode;
        head->count = rhs.head->count;
        memcpy(head->values, rhs.head->values, static_cast<size_t>(Size));
        ++stats.NodeCount;
        --toAlloc;

        // Allocate remaining nodes and reassign
        while (toAlloc != 0)
        {
            allocateNodeAtBack();
        }

        stats.NodeSize  = rhs.nodesize();
        stats.ItemCount = rhs.stats.ItemCount;
        stats.ArraySize = Size;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
BList<T, Size>::~BList()
{
    clear();
}
/*-------------------------------------------------------------------------------------*/
/* Operator Overloads                                                                  */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
BList<T, Size>& BList<T, Size>::operator=(const BList<T, Size>& rhs)
{
    // Clear the BList and reallocate to copy data from rhs
    clear();

    // Copy stats over
    try
    {
        int toAlloc = rhs.stats.NodeCount;

        // Allocate the first node
        head = tail = new BNode;
        head->count = rhs.head->count;
        memcpy(head->values, rhs.head->values, static_cast<size_t>(Size));
        ++stats.NodeCount;
        --toAlloc;

        // Allocate remaining nodes and reassign
        while (toAlloc != 0)
        {
            allocateNodeAtBack();
        }

        stats.NodeSize  = rhs.nodesize();
        stats.ItemCount = rhs.stats.ItemCount;
        stats.ArraySize = Size;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }

    return *this;
}
template <typename T, unsigned int Size>
T& BList<T, Size>::operator[](int)
{
    static T t;
    return t;
}
template <typename T, unsigned int Size>
const T& BList<T, Size>::operator[](int) const
{
    static T t;
    return t;
}
/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
    return head;
}
template <typename T, unsigned int Size>
BListStats BList<T,Size>::GetStats() const
{
    return stats;
}
template <typename T, unsigned int Size>
size_t BList<T, Size>::size() const
{
    return stats.ItemCount;
}
template <typename T, unsigned int Size>
size_t BList<T, Size>::nodesize(void)
{
    return sizeof(BNode);
}
/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
void BList<T, Size>::push_back(const T&)
{

}
template <typename T, unsigned int Size>
void BList<T, Size>::push_front(const T& value)
{
    if (head->count == Size)
    {
        allocateNodeInFront();
    }

    // Front deals with head node
    for (int i = head->count; i > 0; --i)
    {
        head->values[i] = head->values[i - 1];
    }
    head->values[0] = value;

    // Increment stats
    ++head->count;
    ++stats.ItemCount;
}
template <typename T, unsigned int Size>
void BList<T, Size>::insert(const T&)
{

}
template <typename T, unsigned int Size>
void BList<T, Size>::remove(int)
{

}
template <typename T, unsigned int Size>
void BList<T, Size>::remove_by_value(const T&)
{

}
template <typename T, unsigned int Size>
int BList<T, Size>::find(const T&) const
{
    return 0;
}
template <typename T, unsigned int Size>
void BList<T, Size>::clear()
{
    while (head)
    {
        BNode* next = head->next;
        delete head;
        head = next;
    }
} 


/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
template <typename T, unsigned int Size>
void BList<T, Size>::allocateNodeInFront()
{
    try
    {
        BNode* temp = head;
        head = new BNode;
        temp->prev = head;
        head->next = temp;

        head->count = 0;
        ++stats.NodeCount;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}
template <typename T, unsigned int Size>
void BList<T, Size>::allocateNodeAtBack()
{
    try
    {
        BNode* temp = tail;
        tail = new BNode;
        temp->next = tail;
        tail->prev = temp;

        tail->count = 0;
        ++stats.NodeCount;
    }
    catch(const std::bad_alloc&)
    {
        throw BListException{BListException::E_NO_MEMORY, "No physical memory left for allocation!"};
    }
}