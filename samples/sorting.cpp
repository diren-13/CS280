/*+======================================================================================
File:       sorting.cpp

Author:     Diren D Bharwani

Summary:    Explores various sorting algorithms and compares their speed with various input
            sizes.
=========================================================================================+*/

// Standard Libraries
#include <iostream>
#include <chrono>
#include <cstring>
#include <string>
#include <algorithm>

#include "sorting.h"

template <typename T>
void Swap(T& lhs, T& rhs)
{
    T tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

template <typename T>
void BubbleSort(T* arr, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (arr[j] > arr[j+1])
            {
                Swap(arr[j], arr[j+1]);
            }
        }
    }
}

template <typename T>
void BubbleSortAdaptive(T* arr, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j)
        {
            if (arr[j] > arr[j+1])
            {
                Swap(arr[j], arr[j+1]);
                swapped = true;
            }
        }

        if (!swapped)
            return;
    }
}

template <typename T>
void SelectionSort(T* arr, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        int min = i;
        for (int j = i; j < n; ++j)
        {
            if (arr[j] < arr[min])
            {
                min = j;
            }
        }

        Swap(arr[i], arr[min]);
    }
}

template <typename T>
void InsertionSort(T* arr, int n)
{
    for (int i = 1; i < n; ++i)
    {
        T key = arr[i];
        int j = i - 1;

        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

template <typename T>
void MergeSort(T* arr, int left, int right)
{
    if (left < right)
    {
        int mid = (left + right) >> 1;
        MergeSort(arr, left, mid);
        MergeSort(arr, mid+1, right);
        
        auto const subArrayOne = mid - left + 1;
        auto const subArrayTwo = right - mid;
    
        // Create temp arrays
        T* leftArray     = new T[subArrayOne];
        T* rightArray    = new T[subArrayTwo];
    
        // Copy data to temp arrays leftArray[] and rightArray[]
        for (int i = 0; i < subArrayOne; ++i)
        {
            leftArray[i] = arr[left + i];
        }
        for (int j = 0; j < subArrayTwo; ++j)
        {
            rightArray[j] = arr[mid + 1 + j];
        }
            
    
        int indexOfSubArrayOne = 0; // Initial index of first sub-array
        int indexOfSubArrayTwo = 0; // Initial index of second sub-array
        int indexOfMergedArray = left; // Initial index of merged array
    
        // Merge the temp arrays back into array[left..right]
        while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) 
        {
            if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) 
            {
                arr[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
                indexOfSubArrayOne++;
            }
            else 
            {
                arr[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
                indexOfSubArrayTwo++;
            }
            indexOfMergedArray++;
        }
        // Copy the remaining elements of
        // left[], if there are any
        while (indexOfSubArrayOne < subArrayOne) 
        {
            arr[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
            indexOfMergedArray++;
        }
        // Copy the remaining elements of
        // right[], if there are any
        while (indexOfSubArrayTwo < subArrayTwo) 
        {
            arr[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
            indexOfMergedArray++;
        }
    }
}

void RunSortingAlgorithm(const std::string& algoName, void (*sort)(int*, int))
{
    static int size = 50000;

    int* test = new int[size];
    memcpy(test, numbers, sizeof(int) * size);

    auto before = std::chrono::high_resolution_clock::now();
    sort(test, size);
    auto after = std::chrono::high_resolution_clock::now();

    delete[] test;

    std::chrono::duration<double, std::milli> time = after - before;
    std::cout << algoName << ": \n" << time.count() << " ms" << std::endl;
}

int main()
{
    static int size = 50000;

    // std::sort
    int* stdTest = new int[size];
    memcpy(stdTest, numbers, sizeof(int) * size);
    auto beforeSTD = std::chrono::high_resolution_clock::now();
    std::sort(stdTest, stdTest + size, [](int lhs, int rhs){ return lhs < rhs;});
    auto afterSTD = std::chrono::high_resolution_clock::now();
    delete[] stdTest;
    std::chrono::duration<double, std::milli> stdTime = afterSTD - beforeSTD;
    std::cout << "std::sort" << ": \n" << stdTime.count() << " ms" << std::endl;

    RunSortingAlgorithm("BubbleSort", BubbleSort<int>);
    RunSortingAlgorithm("BubbleSortAdaptive", BubbleSortAdaptive<int>);
    RunSortingAlgorithm("SelectionSort", SelectionSort<int>);
    RunSortingAlgorithm("InsertionSort", InsertionSort<int>);
    
    // Recursive Sorting
    int* mergeTest = new int[size];
    memcpy(mergeTest, numbers, sizeof(int) * size);
    auto beforeMS = std::chrono::high_resolution_clock::now();
    MergeSort(mergeTest, 0, size-1);
    auto afterMS = std::chrono::high_resolution_clock::now();
    delete[] mergeTest;
    std::chrono::duration<double, std::milli> msTime = afterMS - beforeMS;
    std::cout << "MergeSort" << ": \n" << msTime.count() << " ms" << std::endl;
}