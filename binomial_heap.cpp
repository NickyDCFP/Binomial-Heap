#include "binomial_heap.h"

/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                                binomial_heap::node implementation                                *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/

template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node() {}

template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node(
    const binomial_heap<T, Comp>::node& rhs
) { this->operator=(rhs); }

template<typename T, typename Comp>
binomial_heap<T, Comp>::node& binomial_heap<T, Comp>::node::operator=(const node& rhs) {
    return *this;
}

template<typename T, typename Comp>
binomial_heap<T, Comp>::node::~node() { delete_children(); children.clear(); }

template<typename T, typename Comp>
void binomial_heap<T, Comp>::node::delete_children() { for(node* child: children) delete child; }


/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                                   binomial_heap implementation                                   *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/
template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap() {}

template<typename T,  typename Comp>
binomial_heap<T, Comp>::binomial_heap(const binomial_heap& rhs) { this->operator=(rhs); }

template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(const binomial_heap& rhs) {
    return *this;
}

template<typename T, typename Comp>
binomial_heap<T, Comp>::~binomial_heap() { delete_trees(); trees.clear(); }

template<typename T, typename Comp>
void binomial_heap<T, Comp>::delete_trees() { for(node* tree: trees) delete tree; }