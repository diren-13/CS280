/************************************************************************************//*!
 @file          Heap.h
 @author        Diren (diren.dbharwani@digipen.edu)
 @brief         Interface of an Heap of integers.
 @date          2022-03-02
 
 @copyright     Copyright (c) 2022
*//*************************************************************************************/

class MinHeap
{
public:
    MinHeap(int _capacity);
    MinHeap(int* values, int _capacity, int numValues);
    ~MinHeap();

    int     Parent      (int index);
    int     Left        (int index);
    int     Right       (int index);
    int     GetMin      ();
  
    // Removes the minimum element (index 0)
    int     ExtractMin  ();
    
    void    DeleteKey   (int index);
    void    InsertKey   (int value);

    void    Print();

private:
    int*    elements;
    int     capacity;   // Max possible size
    int     count;      // Number of elements

    // A recursive method to heapify a subtree with the root at given index
    // This method assumes that the subtrees are already heapified
    void    minHeapify  (int index);

    void    minHeapifyR (int* array, int numValues, int index);
};