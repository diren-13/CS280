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

#define PTR_SIZE sizeof(unsigned char*)

#define TO_GENERIC_OBJECT_PTR(pointer) reinterpret_cast<GenericObject*>(pointer)
#define TO_UCHAR_PTR(pointer) reinterpret_cast<unsigned char*>(pointer)
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
        delete[] TO_UCHAR_PTR(PageList_);
        PageList_ = temp;
    }
}

/*-------------------------------------------------------------------------------------*/
/* Function Members                                                                    */
/*-------------------------------------------------------------------------------------*/

void* ObjectAllocator::Allocate(const char* label)
{
    if (config.UseCPPMemManager_)
    {
        try
        {
            unsigned char* obj = new unsigned char[stats.ObjectSize_];
            ++stats.Allocations_;
            ++stats.ObjectsInUse_;

            stats.MostObjects_ = MAX(stats.MostObjects_, stats.ObjectsInUse_);
            return obj;
        }
        catch(const std::bad_alloc& )
        {
            throw OAException{OAException::E_NO_MEMORY, "No physical memory left."};
        }
    }

    // Take block from free list
    if (FreeList_ == nullptr)
    {
        createPage();
    }

    unsigned char* allocatedObject = TO_UCHAR_PTR(FreeList_);
    FreeList_ = FreeList_->Next;

    setPattern(allocatedObject, ALLOCATED_PATTERN);
    incrementStats();
    createHeader(allocatedObject, label);

    return allocatedObject;
}

void ObjectAllocator::Free(void* Object)
{
    if (config.UseCPPMemManager_)
    {
        delete[] TO_UCHAR_PTR(Object);

        ++stats.Deallocations_;
        --stats.ObjectsInUse_;

        return;
    }

    checkForInvalidFree(Object);

    GenericObject* temp = TO_GENERIC_OBJECT_PTR(Object);
    temp->Next = FreeList_;
    FreeList_ = temp;

    unsigned char* freeListCharPtr = TO_UCHAR_PTR(FreeList_);
    setPattern(freeListCharPtr, FREED_PATTERN);
    destroyHeader(freeListCharPtr);

    decrementStats();
}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
    if (config.HBlockInfo_.type_ == OAConfig::hbNone)
        return stats.ObjectsInUse_;

    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);

    // Traverse pages
    GenericObject* pagePtr = PageList_;
    for (size_t i = 0; i < stats.PagesInUse_; ++i)
    {
        unsigned char* block = TO_UCHAR_PTR(pagePtr) + PTR_SIZE + PAD_BYTES + config.HBlockInfo_.size_;

        for (size_t j = 0; j < config.ObjectsPerPage_; ++j)
        {
            // Check flag for each header
            unsigned char* header = block - PAD_BYTES;
            header -= sizeof(char);     // Flag

            if (*header)
            {
                fn(block, stats.ObjectSize_);
            }

            block += blockSize;
        }

        pagePtr = pagePtr->Next;
    }

    return stats.ObjectsInUse_;
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
    if (config.MaxPages_ && stats.PagesInUse_ == config.MaxPages_)
    {
        throw OAException {OAException::E_NO_PAGES, "Maximum number of pages have been reached."};
    }

    try
    {
        unsigned char* page = new unsigned char[stats.PageSize_];
            
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

void ObjectAllocator::insertPages(unsigned char* page)
{
    // PageList points to new page, new page next points to previous page
    GenericObject* currentPage = PageList_;
    PageList_ = TO_GENERIC_OBJECT_PTR(page);
    PageList_->Next = stats.PagesInUse_ ? currentPage : nullptr;
}

void ObjectAllocator::insertBlocks(unsigned char* page)
{
    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);


    // Prepare blocks and assign FreeList to point to the last block.
    for (size_t i = 0; i < config.ObjectsPerPage_; ++i)
    {
        size_t offset = i ? blockSize 
                          : PTR_SIZE + PAD_BYTES + config.HBlockInfo_.size_;

        GenericObject* currentBlock = FreeList_;
        GenericObject* nextBlock    = TO_GENERIC_OBJECT_PTR(TO_UCHAR_PTR(FreeList_) + offset);

        FreeList_ = i ? nextBlock : TO_GENERIC_OBJECT_PTR(page + offset);
        FreeList_->Next = i ? currentBlock : nullptr;

        // Set header flag where relevant
        unsigned char* freeListCharPtr = TO_UCHAR_PTR(FreeList_);

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
                    unsigned char* setter = freeListCharPtr - PAD_BYTES - config.HBlockInfo_.size_;
                    memset(setter, 0, config.HBlockInfo_.size_ - sizeof(unsigned char) - sizeof(int));
                }

                break;
            }
            case OAConfig::hbExternal:
            {
                unsigned char* header = freeListCharPtr - PAD_BYTES - config.HBlockInfo_.size_;
                memset(header, 0, config.HBlockInfo_.size_);
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

void ObjectAllocator::createHeader(unsigned char* block, const char* label)
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
                unsigned char* setter = block - static_cast<size_t>(config.PadBytes_);
                setter -= sizeof(unsigned char);    // Flag
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
            unsigned char* header = block - static_cast<size_t>(config.PadBytes_) - config.HBlockInfo_.size_;
            MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

            // Create info struct
            try
            {
                *info = new MemBlockInfo;
                (*info)->alloc_num  = stats.Allocations_;
                (*info)->in_use     = true;
                
                if (label)
                {
                    (*info)->label = new char[strlen(label) + 1];
                    strcpy((*info)->label, label);
                }
                else
                {
                    (*info)->label = nullptr;
                }
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

void ObjectAllocator::setHeaderFlag(unsigned char* block, bool flag)
{
    unsigned char* setter = block;
    setter -= static_cast<size_t>(config.PadBytes_);
    setter -= sizeof(unsigned char);

    *setter = static_cast<unsigned char>(flag);
}

void ObjectAllocator::setHeaderAllocNumber(unsigned char* block, int allocNum)
{
    unsigned char* setter = block;
    setter -= static_cast<size_t>(config.PadBytes_);
    setter -= sizeof(unsigned char);
    setter -= sizeof(int);

    int* intSetter = reinterpret_cast<int*>(setter);
    *intSetter = allocNum;
}

void ObjectAllocator::destroyHeader(unsigned char* block)
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
            unsigned char* header = block - static_cast<size_t>(config.PadBytes_) - config.HBlockInfo_.size_;
            MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

            // delete label & info
            delete[] (*info)->label;
            delete *info;

            memset(header, 0, config.HBlockInfo_.size_);
            break;
        }
        default: break;
    }
}

void ObjectAllocator::setPattern(unsigned char* block, unsigned char pattern)
{
    if(!config.DebugOn_)
        return;

    size_t interval = 0;

    switch (pattern)
    {
        case UNALLOCATED_PATTERN:
        case FREED_PATTERN:
        {
            unsigned char* ptrToPatternHead = block + PTR_SIZE;
            interval = stats.ObjectSize_ - PTR_SIZE;

            memset(ptrToPatternHead, pattern, interval);

            break;
        }
        case ALLOCATED_PATTERN:
        {
            unsigned char* ptrToPatternHead = block;
            interval = stats.ObjectSize_;

            memset(ptrToPatternHead, pattern, interval);
            break;
        }
        case PAD_PATTERN:
        {
            interval = static_cast<size_t>(config.PadBytes_);
            
            for (int i = 0; i < 2; ++i)
            {
                unsigned char* ptrToPadHead = block;
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
    unsigned char* currentBlock = TO_UCHAR_PTR(block);

    // False if not within a page
    if (!checkWithinPages(currentBlock, currentPage))
    {
        throw OAException{OAException::E_BAD_BOUNDARY, "Object address is not within a page."};
    }
    
    // False is misaligned
    if (!checkAlignment(currentPage, currentBlock))
    {
        throw OAException{OAException::E_BAD_BOUNDARY, "Object that is trying to be freed is misaligned."};
    }

    // True if corrupted
    if (checkCorruption(currentBlock))
    {
        throw OAException{OAException::E_CORRUPTED_BLOCK, "Pad bytes have been overwritten."};
    }

    // True is object has already been freed
    if (checkMultipleFree(currentBlock))
    {
        throw OAException{OAException::E_MULTIPLE_FREE, "Object has already been freed."};
    }    
}

bool ObjectAllocator::checkWithinPages(const unsigned char* block, void*& currentPage)
{
    // Check if block is within a pages
    GenericObject* pagePtr = PageList_;
    while(pagePtr != nullptr)
    {
        const unsigned char* pagePtrC = TO_UCHAR_PTR(pagePtr);

        if (block > pagePtrC && block < (pagePtrC + stats.PageSize_))
        {
            currentPage = static_cast<void*>(pagePtr);
            return true;
        }

        pagePtr = pagePtr->Next;
    }
    return false;
}

bool ObjectAllocator::checkAlignment(void* currentPage, const unsigned char* block)
{
    const unsigned char* CURRENT_PAGE_C = TO_UCHAR_PTR(currentPage);

    block -= static_cast<size_t>(config.PadBytes_);
    block -= config.HBlockInfo_.size_;

    const ptrdiff_t BLOCK_TO_PAGE = block - CURRENT_PAGE_C;
    const ptrdiff_t OFFSET = BLOCK_TO_PAGE - PTR_SIZE;

    return (static_cast<size_t>(OFFSET) % blockSize == 0);
}

bool ObjectAllocator::checkCorruption(const unsigned char* block)
{
    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);
    return !(checkPadding(block - PAD_BYTES) && checkPadding(block + stats.ObjectSize_));
}

bool ObjectAllocator::checkPadding(const unsigned char* padPtr)
{
    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);
    for (size_t i = 0; i < PAD_BYTES; ++i)
    { 
        if (*(reinterpret_cast<const unsigned char*>(padPtr) + i) != PAD_PATTERN)
            return false;
    }
    return true;
}

bool ObjectAllocator::checkMultipleFree(unsigned char* block)
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