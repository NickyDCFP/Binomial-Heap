/**
 *  @file   heap_sort.h
 *  @brief  A templated sort function that uses binomial heaps to sort data provided by iterators.
 *          For use with stress testing.
 * 
 *  @author Nicky Kriplani
 *  @date   February 12, 2023
*/
#ifndef HEAP_SORT
#define HEAP_SORT 1
#include "binomial_heap.h"

/**
 *  @brief Sorts the data beginning at start and ending at stop using the comparison function
 *         provided.
 * 
 *  @param[in, out] start   the beginning of the range in which data is to be sorted
 *  @param[in, out] stop    the end of the range in which data is to be sorted
 *  @param[in]      compare the comparison function to be used for sorting, defaults to std::less
 */
template<class InputIterator, typename Comp = std::less<typename InputIterator::value_type>>
void heap_sort(InputIterator start, InputIterator stop, const Comp& compare = Comp()) {
    binomial_heap<typename InputIterator::value_type, Comp> heap(start, stop, compare);
    for(; start != stop; ++start) *start = heap.extract();
}
#endif