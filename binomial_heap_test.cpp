/**
 *  @file   binomial_heap_test.cpp
 *  @brief  Test file for the binomial heap data structure.
 * 
 *  @author Nicky Kriplani
 *  @date   February 10, 2023
*/
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include "binomial_heap.h"

int main() {
    std::srand(std::time(0));
    
    std::vector<int> unsorted(30);
    std::iota(unsorted.begin(), unsorted.end(), 1);
    std::random_shuffle(unsorted.begin(), unsorted.end());
    binomial_heap<int> heap(unsorted.begin(), unsorted.end());
    std::vector<int> sorted;
    while(!heap.empty()) sorted.push_back(heap.extract());
    std::cout << "Unsorted: ";
    for(int n: unsorted) std::cout << n << " ";
    std::cout << std::endl;
    std::cout << "Sorted: ";
    for(int n: sorted) std::cout << n << " ";
    std::cout << std::endl;
}