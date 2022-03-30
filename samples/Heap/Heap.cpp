/************************************************************************************//*!
 @file          Heap.cpp
 @author        Diren (diren.dbharwani@digipen.edu)
 @brief         Implementation of an Heap of integers.
 @date          2022-03-02
 
 @copyright     Copyright (c) 2022
*//*************************************************************************************/

#include <iostream>
#include <numeric>

#include "Heap.h"


MinHeap::MinHeap(int _capacity)
: elements  { nullptr }
, capacity  { _capacity }
, count     { 0 }
{
    elements = new int[capacity];
}

MinHeap::MinHeap(int* values, int _capacity, int numValues)
: elements { nullptr }
, capacity { _capacity }
, count    { numValues }
{
    elements = new int[capacity];

    for (int i = 0; i < numValues; ++i)
    {
        elements[i] = values[i];
    }

    const int START_IDX = (numValues / 2) - 1;

    for (int i = START_IDX; i >= 0; --i)
    {
        minHeapify(i);
    }
}

MinHeap::~MinHeap()
{
    delete[] elements;
}

int MinHeap::Parent(int index)
{
    return (index - 1) / 2;
}

int MinHeap::Left(int index)
{
    return (index * 2) + 1;
}

int MinHeap::Right(int index)
{
    return (index * 2) + 2;
}

int MinHeap::GetMin()
{
    return elements[0];
}

int MinHeap::ExtractMin()
{
    if (count < 0)
        return std::numeric_limits<int>::max();

    if (count == 1)
    {
        count = 0;
        return elements[0];
    }

    // Store the minimum value, and remove it from the heap
    int min = elements[0];
    elements[0] = elements[count - 1];
    --count;
    minHeapify(0);

    return min;
}

void MinHeap::DeleteKey(int index)
{
    elements[index] = std::numeric_limits<int>::lowest();
    while (index != 0 && elements[Parent(index)] > elements[index])
    {
       std::swap(elements[index], elements[Parent(index)]);
       index = Parent(index);
    }

    ExtractMin();
}

void MinHeap::InsertKey(int value)
{
    if (count == capacity)
    {
        std::cout << "Cannot insert key!" << std::endl;
        return;
    }
  
    // First insert the new key at the end
    int index = count;
    elements[index] = value;
  
    // Fix the min heap property if it is violated
    while (index != 0 && elements[Parent(index)] > elements[index])
    {
       std::swap(elements[index], elements[Parent(index)]);
       index = Parent(index);
    }

    ++count;
} 

void MinHeap::Print()
{
    for (int i = 0; i < count; ++i)
    {
        std::cout << elements[i] << " ";
    }
    std::cout << std::endl;
}

void MinHeap::minHeapify(int index)
{
    const int LEFT  = Left(index);
    const int RIGHT = Right(index);

    int smallest = index;

    if (LEFT < count && elements[LEFT] < elements[index])
    {
        smallest = LEFT;
    }
        
    if (RIGHT < count && elements[RIGHT] < elements[smallest])
    {
        smallest = RIGHT;
    }

    if (smallest != index)
    {
        std::swap(elements[index], elements[smallest]);
        minHeapify(smallest);
    }
}