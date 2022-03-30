#include "Heap.h"
#include <iostream>

int main()
{
    // MinHeap minHeap(6);

    // minHeap.InsertKey(0);
    // minHeap.InsertKey(3);
    // minHeap.InsertKey(6);
    // minHeap.InsertKey(5);
    // minHeap.InsertKey(9);
    // minHeap.InsertKey(8);
    // minHeap.Print();

    // minHeap.ExtractMin();
    // minHeap.Print();

    // MinHeap minHeap(4);

    // minHeap.InsertKey(9);
    // minHeap.InsertKey(0);
    // minHeap.InsertKey(6);
    // minHeap.InsertKey(8);
    // minHeap.Print();

    int array[10] = {15,4,13,2,11,8,7,5,6,1};
    MinHeap minHeap(array, 10, 10);

    minHeap.Print();
}
