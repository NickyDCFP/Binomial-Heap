#include <chrono>
#include <numeric>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "binomial_heap.h"
#define NUM_INPUTS 10000000

using namespace std::chrono;


void time_insertions(
    binomial_heap<int, std::greater<int>>& stress_binom,
    std::vector<int>& stress_binary,
    std::vector<int>& range_insert,
    std::vector<int>& indiv_insert
);

void time_extractions(
    binomial_heap<int, std::greater<int>>& stress_binom,
    std::vector<int>& stress_binary
);


int main() {
    std::srand(std::time(0));

    binomial_heap<int, std::greater<int>> stress_binom;
    std::vector<int> stress_binary;

    std::vector<int> range_insert(NUM_INPUTS);
    std::vector<int> indiv_insert(NUM_INPUTS);

    for(int i = 0; i < NUM_INPUTS; ++i) {
        range_insert[i] = rand() % NUM_INPUTS;
        indiv_insert[i] = rand() % NUM_INPUTS;
    }
    time_insertions(stress_binom, stress_binary, range_insert, indiv_insert);
    time_extractions(stress_binom, stress_binary);

}
void time_insertions(
    binomial_heap<int, std::greater<int>>& stress_binom,
    std::vector<int>& stress_binary,
    std::vector<int>& range_insert,
    std::vector<int>& indiv_insert
) {
    auto start_range_binom = high_resolution_clock::now();
    stress_binom.multi_insert(range_insert.begin(), range_insert.end());
    auto start_range_binary = high_resolution_clock::now();
    stress_binary = range_insert;
    std::make_heap(stress_binary.begin(), stress_binary.end());

    auto start_ind_binom = high_resolution_clock::now();
    for(int& n: indiv_insert) stress_binom.insert(n);
    auto start_ind_binary = high_resolution_clock::now();
    for(int& n: indiv_insert) {
        stress_binary.push_back(n);
        std::push_heap(stress_binary.begin(), stress_binary.end());
    }
    auto stop_ind_binary = high_resolution_clock::now();

    int binom_range = duration_cast<milliseconds>(start_range_binary - start_range_binom).count();
    int binary_range = duration_cast<milliseconds>(start_ind_binom - start_range_binary).count();
    int binom_ind = duration_cast<milliseconds>(start_ind_binary - start_ind_binom).count();
    int binary_ind = duration_cast<milliseconds>(stop_ind_binary - start_ind_binary).count();
    
    double binom_r_avg = ((double)binom_range) / NUM_INPUTS;
    double binary_r_avg = ((double)binary_range) / NUM_INPUTS;
    double binom_i_avg = ((double)binom_ind) / NUM_INPUTS;
    double binary_i_avg = ((double)binary_ind) / NUM_INPUTS;

    std::cout << "For " << NUM_INPUTS << " range insertions:\n";
    std::cout << "\tBinomial Heap:\n";
    std::cout << "\t\tTotal: " << binom_range << " ms\n";
    std::cout << "\t\tAverage: " << binom_r_avg << " ms per insertion\n";
    std::cout << "\tBinary Heap (std::make_heap):\n";
    std::cout << "\t\tTotal: " << binary_range << " ms\n";
    std::cout << "\t\tAverage: " << binary_r_avg << " ms per insertion\n";
    std::cout << "For " << NUM_INPUTS << " individual insertions\n";
    std::cout << "\tBinomial Heap:\n";
    std::cout << "\t\tTotal: " << binom_ind << " ms\n";
    std::cout << "\t\tAverage: " << binom_i_avg << " ms per insertion\n";
    std::cout << "\tBinary Heap (std::push_heap):\n";
    std::cout << "\t\tTotal: " << binary_ind << " ms\n";
    std::cout << "\t\tAverage: " << binary_i_avg << " ms per insertion\n";
}

void time_extractions(
    binomial_heap<int, std::greater<int>>& stress_binom,
    std::vector<int>& stress_binary
) {
    auto start_binom = high_resolution_clock::now();
    for(int i = 0; i < NUM_INPUTS; ++i) stress_binom.extract();
    auto start_binary = high_resolution_clock::now();
    for(int i = 0; i < NUM_INPUTS; ++i) pop_heap(stress_binary.begin(), stress_binary.end());
    auto stop_binary = high_resolution_clock::now();

    int binom_time = duration_cast<milliseconds>(start_binary - start_binom).count();
    int binary_time = duration_cast<milliseconds>(stop_binary - start_binary).count();
    
    double binom_avg = ((double)binom_time) / NUM_INPUTS;
    double binary_avg = ((double)binary_time) / NUM_INPUTS;

    std::cout << "For " << NUM_INPUTS << " extractions:\n";
    std::cout << "\tBinomial Heap:\n";
    std::cout << "\t\tTotal: " << binom_time << " ms\n";
    std::cout << "\t\tAverage: " << binom_avg << " ms per extraction\n";
    std::cout << "\tBinary Heap (std::pop_heap):\n";
    std::cout << "\t\tTotal: " << binary_time << " ms\n";
    std::cout << "\t\tAverage: " << binary_avg << " ms per extraction\n";
}