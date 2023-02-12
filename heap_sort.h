#ifndef HEAP_SORT
#define HEAP_SORT 1
#include "binomial_heap.h"

template<class InputIterator, typename Comp = std::less<typename InputIterator::value_type>>
void heap_sort(InputIterator start, InputIterator stop, const Comp& compare = Comp()) {
    binomial_heap<typename InputIterator::value_type, Comp> heap(start, stop, compare);
    for(; start != stop; ++start) *start = heap.extract();
}
#endif