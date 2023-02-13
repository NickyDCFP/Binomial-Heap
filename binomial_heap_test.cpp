/**
 *  @file   binomial_heap_test.cpp
 *  @brief  Test file for the binomial heap data structure.
 * 
 *  @author Nicky Kriplani
 *  @date   February 12, 2023
*/
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include "binomial_heap.h"
#include "heap_sort.h"

int main() {
    std::srand(std::time(0));
    
    std::vector<int> unsorted(30);
    std::iota(unsorted.begin(), unsorted.end(), 1);
    std::random_shuffle(unsorted.begin(), unsorted.end());
    std::cout << "Unsorted: ";
    for(int n: unsorted) std::cout << n << " ";
    binom_heap_sort(unsorted.begin(), unsorted.end());
    std::cout << std::endl;
    std::cout << "Sorted: ";
    for(int n: unsorted) std::cout << n << " ";
}