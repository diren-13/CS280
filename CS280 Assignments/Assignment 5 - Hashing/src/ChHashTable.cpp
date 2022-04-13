/************************************************************************************//*!
 @file    ChHashTable.cpp
 @author  Diren D Bharwani, 2002216, diren.dbharwani, diren.dbharwani@digipen.edu
 @date    Apr 10, 2022
 @brief   Contains the interface for the HashTableException, HTStats & ChHashTable
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <exception>
// Primary Headers
#include "ChHashTable.h"

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/

template <typename T>
ChHashTable<T>::ChHashTable(const HTConfig& Config, ObjectAllocator* allocator)
: heads     { nullptr }
, config    { Config }
{
    stats.Count_        = 0U;
    stats.TableSize_    = config.InitialTableSize_;
    stats.Probes_       = 0U;
    stats.Expansions_   = 0U;
    stats.HashFunc_     = config.HashFunc_;
    stats.Allocator_    = allocator;

    try
    {
        heads = new ChHTHeadNode[config.InitialTableSize_];
    }
    catch (const std::bad_alloc&)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        throw HashTableException
        (
            HashTableException::HASHTABLE_EXCEPTION::E_NO_MEMORY,
            functionSignature + ": No memory left to allocate!"
        );
    }
    
}

template <typename T>
ChHashTable<T>::~ChHashTable()
{
    delete[] heads;
}

/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/

template <typename T>
HTStats ChHashTable<T>::GetStats() const
{
    return stats;
}

template <typename T>
const typename ChHashTable<T>::ChHTHeadNode* ChHashTable<T>::GetTable() const
{
    return heads;
}

/*-------------------------------------------------------------------------------------*/
/* Public Function Members                                                             */
/*-------------------------------------------------------------------------------------*/

template <typename T>
void ChHashTable<T>::insert(const char* Key, const T& Data)
{
    const uint32_t INDEX = SpawnHornTail(Key, stats.TableSize_);

    ChHTNode* temp = heads[INDEX].Nodes;

    // Check for duplicate
    while (temp)
    {
        if (temp->Data == Data)
        {
            std::string functionSignature;
            #ifdef _MSC_VER
            functionSignature = __FUNCTION__;
            #else
            functionSignature = __PRETTY_FUNCTION__;
            #endif

            throw HashTableException
            (
                HashTableException::HASHTABLE_EXCEPTION::E_DUPLICATE,
                functionSignature + ": Attempted to insert duplicate data!"
            );
        }
        temp = temp->Next;
    }


    // If no duplicate found, insert at the head
    try
    {
        ChHTNode* newNode   = static_cast<ChHTNode*>(stats.Allocator_->Allocate(Data));
        newNode->Key        = Key;
        newNode->Next       = heads[INDEX].Nodes;

        heads[INDEX].Nodes = newNode;
    }
    catch (const std::bad_alloc&)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        
    }
    
}

template <typename T>
void ChHashTable<T>::remove(const char* Key)
{
    // Hash key to get the index of the element
    const uint32_t INDEX = SpawnHornTail(Key, stats.TableSize_);

    ChHTNode* temp = heads[INDEX].Nodes;

    // Find node with matching key
    bool keyFound = false;
    while (temp)
    {
        if (std::strcmp(temp->Key, Key))
        {
            keyFound = true;
            break;
        }
        temp = temp->Next;
    }

    if (!keyFound)
    {
        std::string functionSignature;
        #ifdef _MSC_VER
        functionSignature = __FUNCTION__;
        #else
        functionSignature = __PRETTY_FUNCTION__;
        #endif

        throw HashTableException
        (
            HashTableException::HASHTABLE_EXCEPTION::E_ITEM_NOT_FOUND,
            functionSignature + ": Attempted to insert duplicate data!"
        );
    }
}

template <typename T>
const T& ChHashTable<T>::find(const char* Key) const
{
    
}

template <typename T>
void ChHashTable<T>::clear()
{
    
}





/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/



