/************************************************************************************//*!
\file           ObjectAllocator.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Jan 19, 2022
\brief          Contains the implementation of the Matrix class.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "ObjectAllocator.h"
// Standard Libraries
#include <iostream>
#include <cstring>

#define PTR_SIZE sizeof(char*)

#define TO_GENERIC_OBJECT_PTR(pointer) reinterpret_cast<GenericObject*>(pointer)
#define TO_CHAR_PTR(pointer) reinterpret_cast<char*>(pointer)
#define MIN(X, Y) (X < Y ? X : Y)
#define MAX(X, Y) (X < Y ? Y : X)

/*-------------------------------------------------------------------------------------*/
/* Constructors & Destructors                                                          */
/*-------------------------------------------------------------------------------------*/
ObjectAllocator::ObjectAllocator(size_t objectSize, const OAConfig& c)
: PageList_     (nullptr)
, FreeList_     (nullptr)
, config        (c)
, stats         ()
, blockSize     (0)
{
    // Populate stats
    stats.ObjectSize_ = objectSize;

    blockSize = objectSize;
    blockSize += (2 * c.PadBytes_);     // Add padding
    blockSize += c.HBlockInfo_.size_ ;  // Add header
    // Add alignment
    
    stats.PageSize_ = PTR_SIZE;
    stats.PageSize_ += blockSize * c.ObjectsPerPage_;

    createPage();
}

ObjectAllocator::~ObjectAllocator()
{
    while(PageList_ != nullptr)
    {
        GenericObject* temp = PageList_->Next;
        delete[] TO_CHAR_PTR(PageList_);
        PageList_ = temp;
    }
}

/*-------------------------------------------------------------------------------------*/
/* Function Members                                                                    */
/*-------------------------------------------------------------------------------------*/

void* ObjectAllocator::Allocate(const char* label)
{
    // Take block from free list
    if (FreeList_ == nullptr)
    {
        createPage();
    }

    char* allocatedObject = TO_CHAR_PTR(FreeList_);
    FreeList_ = FreeList_->Next;

    // set blocks to allocated pattern
    setPattern(allocatedObject, ALLOCATED_PATTERN);
    incrementStats();
    createHeader(allocatedObject, label);

    return allocatedObject;
}

void ObjectAllocator::Free(void* Object)
{
    checkForInvalidFree(Object);

    GenericObject* temp = TO_GENERIC_OBJECT_PTR(Object);
    temp->Next = FreeList_;
    FreeList_ = temp;

    char* freeListCharPtr = TO_CHAR_PTR(FreeList_);
    setPattern(freeListCharPtr, FREED_PATTERN);
    destroyHeader(freeListCharPtr);

    // Set stats

}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK) const
{
    return 0;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK) const
{
    return 0;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
    return 0;
}

/*-------------------------------------------------------------------------------------*/
/* Getter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/ 
const void* ObjectAllocator::GetFreeList() const
{
    return FreeList_;
}

const void* ObjectAllocator::GetPageList() const
{
    return PageList_;
}

OAConfig ObjectAllocator::GetConfig() const
{
    return config;
}

OAStats ObjectAllocator::GetStats() const
{
    return stats;
}

/*-------------------------------------------------------------------------------------*/
/* Setter Functions                                                                    */
/*-------------------------------------------------------------------------------------*/
void ObjectAllocator::SetDebugState(bool state)
{
    config.DebugOn_ = state;
}

/*-------------------------------------------------------------------------------------*/
/* Private Function Members                                                            */
/*-------------------------------------------------------------------------------------*/
void ObjectAllocator::createPage()
{
    if (config.MaxPages_ != 0 && stats.PagesInUse_ == config.MaxPages_)
    {
        throw OAException {OAException::E_NO_PAGES, "Maximum number of pages have been reached."};
    }

    try
    {
        char* page = new char[stats.PageSize_];
            
        insertPages(page);
        insertBlocks(page);

        // Update stats
        ++stats.PagesInUse_;
        stats.FreeObjects_ += config.ObjectsPerPage_;
    }
    catch(const std::bad_alloc&)
    {
        throw OAException {OAException::E_NO_MEMORY, "No system memory available."};
    }
}

void ObjectAllocator::insertPages(char* page)
{
    // PageList points to new page, new page next points to previous page
    GenericObject* currentPage = PageList_;
    PageList_ = TO_GENERIC_OBJECT_PTR(page);
    PageList_->Next = stats.PagesInUse_ ? currentPage : nullptr;
}

void ObjectAllocator::insertBlocks(char* page)
{
    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);


    // Prepare blocks and assign FreeList to point to the last block.
    for (size_t i = 0; i < config.ObjectsPerPage_; ++i)
    {
        size_t offset = i ? blockSize 
                          : PTR_SIZE + PAD_BYTES + config.HBlockInfo_.size_;

        GenericObject* currentBlock = FreeList_;
        GenericObject* nextBlock    = TO_GENERIC_OBJECT_PTR(TO_CHAR_PTR(FreeList_) + offset);

        FreeList_ = i ? nextBlock : TO_GENERIC_OBJECT_PTR(page + offset);
        FreeList_->Next = i ? currentBlock : nullptr;

        // Set header flag where relevant
        char* freeListCharPtr = TO_CHAR_PTR(FreeList_);

        switch (config.HBlockInfo_.type_)
        {
            case OAConfig::hbBasic:
            case OAConfig::hbExtended:
            {
                setHeaderFlag(freeListCharPtr, false);
                setHeaderAllocNumber(freeListCharPtr, 0);

                // Set use count and user-defined bytes for extended header
                if (config.HBlockInfo_.type_ == OAConfig::hbExtended)
                {
                    char* setter = freeListCharPtr - PAD_BYTES - config.HBlockInfo_.size_;
                    memset(setter, 0, config.HBlockInfo_.size_ - sizeof(char) - sizeof(int));
                }

                break;
            }
            default: break;
        } 

        setPattern(freeListCharPtr, PAD_PATTERN);
        setPattern(freeListCharPtr, UNALLOCATED_PATTERN);
    }
}

void ObjectAllocator::incrementStats()
{
    ++stats.Allocations_;
    ++stats.ObjectsInUse_;

    --stats.FreeObjects_;

    stats.MostObjects_ = MAX(stats.MostObjects_, stats.ObjectsInUse_);
}

void ObjectAllocator::decrementStats()
{
    ++stats.Deallocations_;
    ++stats.FreeObjects_;

    --stats.ObjectsInUse_;
}

void ObjectAllocator::createHeader(char* block, const char* label)
{
    switch (config.HBlockInfo_.type_)
    {
        case OAConfig::hbBasic:
        case OAConfig::hbExtended:
        {
            setHeaderFlag(block, true);
            setHeaderAllocNumber(block, stats.Allocations_);
            
            // Set use count for extended headers
            if (config.HBlockInfo_.type_ == OAConfig::hbExtended)
            {
                char* setter = block - static_cast<size_t>(config.PadBytes_);
                setter -= sizeof(char);             // Flag
                setter -= sizeof(int);              // Alloc Num
                setter -= sizeof(short);            // Use Count

                short* useCountPtr = reinterpret_cast<short*>(setter);
                ++(*useCountPtr);
            }

            break;
        }
        case OAConfig::hbExternal:
        {
            // Move to header pointer
            char* header = block - static_cast<size_t>(config.PadBytes_) - config.HBlockInfo_.size_;
            MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

            // Create info struct
            try
            {
                *info = new MemBlockInfo;
                (*info)->alloc_num  = 0;
                (*info)->in_use     = true;
                
                (*info)->label = new char[strlen(label) + 1];
                strcpy((*info)->label, label);
            }
            catch(const std::bad_alloc&)
            {
                throw OAException{OAException::E_NO_MEMORY, "No physical memory available!"};
            }

            break;
        }
        default: break;
    }
}

void ObjectAllocator::setHeaderFlag(char* block, bool flag)
{
    char* setter = block;
    setter -= static_cast<size_t>(config.PadBytes_);
    setter -= sizeof(char);

    *setter = static_cast<char>(flag);
}

void ObjectAllocator::setHeaderAllocNumber(char* block, int allocNum)
{
    char* setter = block;
    setter -= static_cast<size_t>(config.PadBytes_);
    setter -= sizeof(char);
    setter -= sizeof(int);

    int* intSetter = reinterpret_cast<int*>(setter);
    *intSetter = allocNum;
}

void ObjectAllocator::destroyHeader(char* block)
{
    switch (config.HBlockInfo_.type_)
    {
        case OAConfig::hbBasic:
        case OAConfig::hbExtended:
        {
            setHeaderFlag(block, false);
            setHeaderAllocNumber(block, 0);

            break;
        }
        case OAConfig::hbExternal:
        {
            // Move to header pointer
            char* header = block - static_cast<size_t>(config.PadBytes_) - config.HBlockInfo_.size_;
            MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

            // delete label & info
            delete[] (*info)->label;
            delete *info;

            break;
        }
        default: break;
    }
}

void ObjectAllocator::setPattern(char* block, unsigned char pattern)
{
    if(!config.DebugOn_)
        return;

    size_t interval = 0;

    switch (pattern)
    {
        case UNALLOCATED_PATTERN:
        case ALLOCATED_PATTERN:
        case FREED_PATTERN:
        {
            char* ptrToPatternHead = block + PTR_SIZE;
            interval = stats.ObjectSize_ - PTR_SIZE;

            memset(ptrToPatternHead, pattern, interval);
            break;
        }
        case PAD_PATTERN:
        {
            interval = static_cast<size_t>(config.PadBytes_);
            
            for (int i = 0; i < 2; ++i)
            {
                char* ptrToPadHead = block;
                ptrToPadHead += i ? stats.ObjectSize_ : -interval;
                memset(ptrToPadHead, pattern, interval);
            }
    
            break;
        }
        case ALIGN_PATTERN:
        {
            break;
        }
        default: break;
    }
}

void ObjectAllocator::checkForInvalidFree(void* block)
{
    if (!config.DebugOn_)
        return;

    void* currentPage = nullptr;
    char* currentBlock = TO_CHAR_PTR(block);

    if (!checkWithinPages(currentBlock, currentPage))
    {
        throw OAException{OAException::E_BAD_BOUNDARY, "Object address is not within a page."};
    }
    
    // False is misaligned
    if (!checkAlignment(currentPage, currentBlock))
    {
        throw OAException{OAException::E_BAD_BOUNDARY, "Object that is trying to be freed is misaligned."};
    }

    // True is object has already been freed
    if (checkMultipleFree(currentBlock))
    {
        throw OAException{OAException::E_MULTIPLE_FREE, "Object has already been freed."};
    }    
}

bool ObjectAllocator::checkWithinPages(const char* block, void*& currentPage)
{
    // Check if block is within a pages
    GenericObject* pagePtr = PageList_;
    while(pagePtr != nullptr)
    {
        const char* pagePtrC = TO_CHAR_PTR(pagePtr);

        if (block > pagePtrC && block < (pagePtrC + stats.PageSize_))
        {
            currentPage = static_cast<void*>(pagePtr);
            return true;
        }

        pagePtr = pagePtr->Next;
    }
    return false;
}

bool ObjectAllocator::checkAlignment(void* currentPage, const char* block)
{
    const char* CURRENT_PAGE_C = TO_CHAR_PTR(currentPage);

    block -= static_cast<size_t>(config.PadBytes_);
    block -= config.HBlockInfo_.size_;

    const ptrdiff_t BLOCK_TO_PAGE = block - CURRENT_PAGE_C;
    const ptrdiff_t OFFSET = BLOCK_TO_PAGE - PTR_SIZE;

    return (static_cast<size_t>(OFFSET) % blockSize == 0);
}

bool ObjectAllocator::checkMultipleFree(char* block)
{
    // Compare against all items in the free list
    GenericObject* blockGO = TO_GENERIC_OBJECT_PTR(block);
    GenericObject* freeListPtr = FreeList_;

    while(freeListPtr != nullptr)
    {
        if (blockGO == freeListPtr)
            return true;

        freeListPtr = freeListPtr->Next;
    }
    return false;
}