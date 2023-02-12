#include <chrono>
#include <algorithm>
#include "heap_sort.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> unsorted_sample(100);
    std::iota(unsorted_sample.begin(), unsorted_sample.end(), 1);
    
}