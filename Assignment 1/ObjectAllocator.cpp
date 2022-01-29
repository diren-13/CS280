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

#define MAX(x, y) x > y ? x : y

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

    unsigned int leftOffset = static_cast<unsigned int>(c.HBlockInfo_.size_ + c.PadBytes_ + PTR_SIZE);
    config.LeftAlignSize_ = computeLeftAlign(c.Alignment_, leftOffset);

    unsigned int interOffset = static_cast<unsigned int>(objectSize + c.HBlockInfo_.size_ + 2 * c.PadBytes_);
    config.InterAlignSize_ = computeInterAlign(c.Alignment_, interOffset);

    blockSize = objectSize;
    blockSize += (2 * c.PadBytes_);         // Add padding
    blockSize += c.HBlockInfo_.size_ ;      // Add header
    blockSize += config.InterAlignSize_;    // Add alignment
    
    stats.PageSize_ = PTR_SIZE;
    stats.PageSize_ += config.LeftAlignSize_;
    stats.PageSize_ += blockSize * c.ObjectsPerPage_;
    // Remove one interAlignment
    stats.PageSize_ -= config.InterAlignSize_;

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
        catch(const std::bad_alloc&)
        {
            throw OAException{OAException::E_NO_MEMORY, "No physical memory left."};
        }
    }

    // Take block from free list
    if (FreeList_ == nullptr)
    {
        createPage();
    }

    unsigned char* obj = TO_UCHAR_PTR(FreeList_);
    FreeList_ = FreeList_->Next;

    setPattern(obj, ALLOCATED_PATTERN);
    incrementStats();
    createHeader(obj, label);

    return obj;
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

    unsigned char* cFL = TO_UCHAR_PTR(FreeList_);
    setPattern(cFL, FREED_PATTERN);
    destroyHeader(cFL);

    decrementStats();
}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
    if (config.HBlockInfo_.type_ == OAConfig::hbNone)
        return stats.ObjectsInUse_;

    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);

    // Traverse pages
    GenericObject* page = PageList_;
    for (size_t i = 0; i < stats.PagesInUse_; ++i)
    {
        unsigned char* cP = TO_UCHAR_PTR(page);
        unsigned char* block = firstBlock(cP);

        for (size_t j = 0; j < config.ObjectsPerPage_; ++j)
        {
            switch (config.HBlockInfo_.type_)
            {
                case OAConfig::hbBasic:
                case OAConfig::hbExtended:
                {
                    // Check flag for each header
                    unsigned char* flag = block - PAD_BYTES - sizeof(char);
                    if (*flag)
                    {
                        fn(block, stats.ObjectSize_);
                    }

                    break;
                }
                case OAConfig::hbExternal:
                {
                    unsigned char* header = block - PAD_BYTES - config.HBlockInfo_.size_;
                    MemBlockInfo** info = reinterpret_cast<MemBlockInfo**>(header);

                    if (*info && (*info)->in_use)
                    {
                        fn(block, stats.ObjectSize_);
                    }

                    break;
                }
                default: break;
            }

            block += blockSize;
        }

        page = page->Next;
    }

    return stats.ObjectsInUse_;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
    unsigned int numCorrupted = 0;

    // Traverse pages
    GenericObject* page = PageList_;
    for (size_t i = 0; i < stats.PagesInUse_; ++i)
    {
        unsigned char* cP = TO_UCHAR_PTR(page);
        const unsigned char* block = firstBlock(cP);

        for (size_t j = 0; j < config.ObjectsPerPage_; ++j)
        {
            // Check corruption in each block
            bool corrupted = checkCorruption(block);

            if (corrupted)
            {
                ++numCorrupted;
                fn(block, stats.ObjectSize_);
            }

            block += blockSize;
        }

        page = page->Next;
    }

    return numCorrupted;
}

unsigned ObjectAllocator::FreeEmptyPages()
{
    if (PageList_ == nullptr)
        return 0; 

    unsigned int numFreed = 0;
    
    // Traverse pages
    GenericObject* page = PageList_;
    GenericObject* prev = nullptr;

    while (page != nullptr)
    {
        if (isPageEmpty(page))
        {
            GenericObject* next = page->Next;
            freePage(page);
            ++numFreed;
            
            // Handle head
            if (!prev)
            {
                PageList_ = next; 
            }
            else
            {
                prev->Next = next;
            }

            page = next;
        }
        else
        {
            prev = page;
            page = page->Next;
        }
    }

    return numFreed;
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

    setLeftAlignment(page);

    // Prepare blocks and assign FreeList to point to the last block.
    for (size_t i = 0; i < config.ObjectsPerPage_; ++i)
    {
        size_t offset = i ? blockSize : PTR_SIZE + PAD_BYTES + config.HBlockInfo_.size_ + config.LeftAlignSize_;

        unsigned char* cFL = TO_UCHAR_PTR(FreeList_);
        GenericObject* currentBlock = FreeList_;
        GenericObject* nextBlock    = TO_GENERIC_OBJECT_PTR(cFL + offset);

        FreeList_ = i ? nextBlock : TO_GENERIC_OBJECT_PTR(page + offset);
        FreeList_->Next = i ? currentBlock : nullptr;

        // Set header flag where relevant
        cFL = TO_UCHAR_PTR(FreeList_);

        switch (config.HBlockInfo_.type_)
        {
            case OAConfig::hbBasic:
            case OAConfig::hbExtended:
            {
                setHeaderFlag(cFL, false);
                setHeaderAllocNumber(cFL, 0);

                // Set use count and user-defined bytes for extended header
                if (config.HBlockInfo_.type_ == OAConfig::hbExtended)
                {
                    unsigned char* setter = cFL - PAD_BYTES - config.HBlockInfo_.size_;
                    memset(setter, 0, config.HBlockInfo_.size_ - sizeof(unsigned char) - sizeof(int));
                }

                break;
            }
            case OAConfig::hbExternal:
            {
                unsigned char* header = cFL - PAD_BYTES - config.HBlockInfo_.size_;
                memset(header, 0, config.HBlockInfo_.size_);
                break;
            }
            default: break;
        } 

        if (i != config.ObjectsPerPage_ -1)
        {
            setInterAlignment(cFL);
        }
        setPattern(cFL, PAD_PATTERN);
        setPattern(cFL, UNALLOCATED_PATTERN);
    }
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

                short* useCount = reinterpret_cast<short*>(setter);
                ++(*useCount);
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

void ObjectAllocator::setHeaderAllocNumber(unsigned char* block, int num)
{
    unsigned char* setter = block;
    setter -= static_cast<size_t>(config.PadBytes_);
    setter -= sizeof(unsigned char);
    setter -= sizeof(int);

    int* allocNum = reinterpret_cast<int*>(setter);
    *allocNum = num;
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
            if (*info)
            {
                delete[] (*info)->label;
                delete *info;
            }

            memset(header, 0, config.HBlockInfo_.size_);
            break;
        }
        default: break;
    }
}

void ObjectAllocator::freePage(GenericObject* page)
{
    GenericObject* fL = FreeList_;
    GenericObject* prev = nullptr;

    // Traverse freelist
    while (fL != nullptr)
    {
        unsigned char* cFL = TO_UCHAR_PTR(fL);

        if (isInPage(page, cFL))
        {
            GenericObject* next = fL->Next;

            if (prev)
            {
                prev->Next = next;
            }
            else
            {
                FreeList_ = next;
            }

            --stats.FreeObjects_;
        }
        else
        {
            prev = fL;
        }

        fL = fL->Next;
    }

    delete[] TO_UCHAR_PTR(page);
    --stats.PagesInUse_;
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

unsigned int ObjectAllocator::computeLeftAlign(unsigned int alignment, unsigned int offset) const
{
    if (config.Alignment_ <= 1)
        return 0;

    const unsigned int closestMultiple = offset / alignment + 1;
    return (alignment * closestMultiple) - offset;
}

unsigned int ObjectAllocator::computeInterAlign(unsigned int alignment, unsigned int offset) const
{
    if (config.Alignment_ <= 1)
        return 0;

    const unsigned int closestMultiple = offset / alignment + 1;
    return (alignment * closestMultiple) - offset;
}

unsigned char* ObjectAllocator::firstBlock(unsigned char* page) const
{
    return page + PTR_SIZE + config.LeftAlignSize_ + config.HBlockInfo_.size_ + config.PadBytes_;
}

bool ObjectAllocator::isPageEmpty(GenericObject* page) const
{
    GenericObject* fL = this->FreeList_;

    unsigned int freeBlocks = 0;

    while (fL)
    {
        unsigned char* cFL = TO_UCHAR_PTR(fL);

        if (isInPage(page, cFL))
        {
            ++freeBlocks;

            if (freeBlocks >= config.ObjectsPerPage_)
                return true;
        }

        fL = fL->Next;
    }
    return false;
}

bool ObjectAllocator::isInPage(GenericObject* page, unsigned char* block) const
{
    unsigned char* cP = TO_UCHAR_PTR(page);
    return block > cP && block < (cP + stats.PageSize_);
}

bool ObjectAllocator::isBlockAllocated(unsigned char* block) const
{
    switch (config.HBlockInfo_.type_)
    {
        case OAConfig::hbNone:
        {
            GenericObject* b    = TO_GENERIC_OBJECT_PTR(block);
            GenericObject* fL   = FreeList_;

            // Iterate through free list and compare addresses
            while (fL != nullptr)
            {
                // If block is on the free list, it is unallocated
                if (fL == b)
                    return false;
                
                fL = fL->Next;
            }

            // Block cannot be found on the free list, therefore it has been allocated
            return true;
        }   
        default: break;
    }

    return false;
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
            unsigned char* patternHead = block + PTR_SIZE;
            interval = stats.ObjectSize_ - PTR_SIZE;

            memset(patternHead, pattern, interval);

            break;
        }
        case ALLOCATED_PATTERN:
        {
            unsigned char* patternHead = block;
            interval = stats.ObjectSize_;

            memset(patternHead, pattern, interval);
            break;
        }
        case PAD_PATTERN:
        {
            interval = static_cast<size_t>(config.PadBytes_);
            
            for (int i = 0; i < 2; ++i)
            {
                unsigned char* pad = block;
                pad += i ? stats.ObjectSize_ : -interval;
                memset(pad, pattern, interval);
            }
    
            break;
        }
        default: break;
    }
}

void ObjectAllocator::setLeftAlignment(unsigned char* head)
{
    head += PTR_SIZE;
    memset(head, ALIGN_PATTERN, config.LeftAlignSize_);
}

void ObjectAllocator::setInterAlignment(unsigned char* block)
{
    block += (stats.ObjectSize_ + static_cast<size_t>(config.PadBytes_));
    memset(block, ALIGN_PATTERN, config.InterAlignSize_);
}

void ObjectAllocator::checkForInvalidFree(void* block) const
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

bool ObjectAllocator::checkWithinPages(const unsigned char* block, void*& currentPage) const
{
    // Check if block is within a pages
    GenericObject* page = PageList_;
    while(page != nullptr)
    {
        const unsigned char* CP = TO_UCHAR_PTR(page);

        if (block > CP && block < (CP + stats.PageSize_))
        {
            currentPage = static_cast<void*>(page);
            return true;
        }

        page = page->Next;
    }
    return false;
}

bool ObjectAllocator::checkAlignment(void* currentPage, const unsigned char* block) const
{
    const unsigned char* CP = TO_UCHAR_PTR(currentPage);

    ptrdiff_t blockToPage = 0;
    ptrdiff_t offset = 0;

    if (config.Alignment_ > 1)
    {
        blockToPage = block - (CP);
        return (static_cast<size_t>(blockToPage) % config.Alignment_ == 0);
    }

    block -= static_cast<size_t>(config.PadBytes_);
    block -= config.HBlockInfo_.size_;

    blockToPage = block - (CP);
    offset = blockToPage - PTR_SIZE;

    return (static_cast<size_t>(offset) % blockSize == 0);
}

bool ObjectAllocator::checkCorruption(const unsigned char* block) const
{
    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);
    return !(checkPadding(block - PAD_BYTES) && checkPadding(block + stats.ObjectSize_));
}

bool ObjectAllocator::checkPadding(const unsigned char* padPtr) const
{
    const size_t PAD_BYTES = static_cast<size_t>(config.PadBytes_);
    for (size_t i = 0; i < PAD_BYTES; ++i)
    { 
        if (*(padPtr + i) != PAD_PATTERN)
            return false;
    }
    return true;
}

bool ObjectAllocator::checkMultipleFree(unsigned char* block) const
{
    // Compare against all items in the free list
    GenericObject* b = TO_GENERIC_OBJECT_PTR(block);
    GenericObject* fL = FreeList_;

    while(fL != nullptr)
    {
        if (b == fL)
            return true;

        fL = fL->Next;
    }
    return false;
}