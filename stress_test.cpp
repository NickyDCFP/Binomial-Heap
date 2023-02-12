#include <chrono>
#include <numeric>
#include <iostream>
#include <algorithm>
#include "heap_sort.h"
#define SAMPLE_SIZE 10000
#define NUM_SAMPLES 10000
using namespace std::chrono;

int main() {
    std::srand(std::time(0));

    std::vector<int> unsorted_sample(SAMPLE_SIZE);
    std::iota(unsorted_sample.begin(), unsorted_sample.end(), 1);
    std::vector<std::vector<int>> unsorted_1(NUM_SAMPLES, unsorted_sample);
    for(std::vector<int> v: unsorted_1) std::random_shuffle(v.begin(), v.end());
    std::vector<std::vector<int>> unsorted_2(unsorted_1);

    auto sort_start = high_resolution_clock::now();
    for(std::vector<int> v: unsorted_1) std::sort(v.begin(), v.end());
    auto sort_stop = high_resolution_clock::now();

    auto heap_start = high_resolution_clock::now();
    for(std::vector<int> v: unsorted_2) heap_sort(v.begin(), v.end());
    auto heap_stop = high_resolution_clock::now();

    int sort_duration = duration_cast<milliseconds>(sort_stop - sort_start).count();
    int heap_duration = duration_cast<milliseconds>(heap_stop - heap_start).count();

    std::cout << "For " << NUM_SAMPLES << " samples of size " << SAMPLE_SIZE << ",\n\t" <<
                 "std::sort:\n\t\tTotal: " << sort_duration << " ms\n\t\tAverage: " <<
                 (sort_duration / NUM_SAMPLES) << " ms\n\theap_sort:\n\t\tTotal: " <<
                 heap_duration << " ms\n\t\tAverage: " << (heap_duration / NUM_SAMPLES) << " ms";
}

/*
Test 1, 2/12/2023 2:29 pm, pre-any optimization
For 10000 samples of size 10000,
        std::sort:
                Total: 11797 ms
                Average: 1 ms
        heap_sort:
                Total: 117774 ms
                Average: 11 ms
*/