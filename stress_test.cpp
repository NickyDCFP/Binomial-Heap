/**
 *  @file   stress_test.cpp
 *  @brief  Stress tests the sorting algorithm by sorting a number of vectors.
 * 
 *  @author Nicky Kriplani
 *  @date   February 12, 2023
*/
#include <chrono>
#include <numeric>
#include <iostream>
#include <algorithm>
#include "heap_sort.h"
#define SAMPLE_SIZE 10000
#define NUM_SAMPLES 10000
using namespace std::chrono;

template<typename InputIterator, typename Comp = std::less<typename InputIterator::value_type>>
void binary_heap_sort(InputIterator start, InputIterator stop, const Comp& compare = Comp());

int main() {
    std::srand(std::time(0));

    std::vector<int> unsorted_sample(SAMPLE_SIZE);
    std::iota(unsorted_sample.begin(), unsorted_sample.end(), 1);
    std::vector<std::vector<int>> unsorted_1(NUM_SAMPLES, unsorted_sample);
    for(std::vector<int> v: unsorted_1) std::random_shuffle(v.begin(), v.end());
    std::vector<std::vector<int>> unsorted_2(unsorted_1);
    std::vector<std::vector<int>> unsorted_3(unsorted_1);

    auto sort_start = high_resolution_clock::now();
    for(std::vector<int> v: unsorted_1) std::sort(v.begin(), v.end());
    auto binary_start = high_resolution_clock::now();
    for(std::vector<int> v: unsorted_2) binary_heap_sort(v.begin(), v.end());
    auto binom_start = high_resolution_clock::now();
    for(std::vector<int> v: unsorted_3) binom_heap_sort(v.begin(), v.end());
    auto binom_stop = high_resolution_clock::now();


    int sort_duration = duration_cast<milliseconds>(binary_start - sort_start).count();
    int binary_heap_duration = duration_cast<milliseconds>(binom_start - binary_start).count();
    int binom_heap_duration = duration_cast<milliseconds>(binom_stop - binom_start).count();
    double sort_avg = ((double)sort_duration) / NUM_SAMPLES;
    double binary_heap_avg = ((double)binary_heap_duration) / NUM_SAMPLES;
    double binom_heap_avg = ((double)binom_heap_duration) / NUM_SAMPLES;

    std::cout << "For " << NUM_SAMPLES << " samples of size " << SAMPLE_SIZE << ",\n\t";
    std::cout <<  "std::sort:\n\t\tTotal: " << sort_duration << " ms\n\t\tAverage: " <<
                 sort_avg << " ms\n\t";
    std::cout << "binary_heap_sort:\n\t\tTotal: " << binary_heap_duration << " ms\n\t\tAverage: " <<
                  binary_heap_avg << " ms\n\t";
    std::cout << "binom_heap_sort:\n\t\tTotal: " << binom_heap_duration << " ms\n\t\tAverage: " <<
                  binom_heap_avg << " ms.";
}

/*
Test 1, 2/13/2023 12:16 am, pre-any optimization
For 10000 samples of size 10000,
        std::sort:
                Total: 12534 ms
                Average: 1.2534 ms
        binary_heap_sort:
                Total: 46525 ms
                Average: 4.6525 ms
        binom_heap_sort:
                Total: 128731 ms
                Average: 12.8731 ms.
*/

/**
 *  @brief  Sorts the data by converting it to a binary max-heap
 * 
 *  @param  start   the beginning of the range to be sorted
 *  @param  stop    the end of the range to be sorted
 *  @param  compare the comparison function to be used for sorting, defaults to std::less
 */
template<typename InputIterator, typename Comp>
void binary_heap_sort(InputIterator start, InputIterator stop, const Comp& compare) {
    std::make_heap(start, stop, compare);
    std::sort_heap(start, stop, compare);
}