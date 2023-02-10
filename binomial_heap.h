/**
 *  @file   binomial_heap.h
 *  @brief  A templated binomial heap that supports constant time amortized insertion and
 *          logarithmic time merging.
 * 
 *  @author Nicky Kriplani
 *  @date   February 10, 2023
*/
#ifndef BINOMIAL_HEAP
#define BINOMIAL_HEAP 1
#include <forward_list>
#include <vector>
#include <functional>
#include <stdexcept>

template<typename T, typename Comp = std::less<T>>
class binomial_heap {
    struct node;
public:
    class iterator;
    binomial_heap(const Comp& compare = Comp());
    template<class InputIterator>
    binomial_heap(InputIterator start, InputIterator stop, const Comp& compare = Comp());
    binomial_heap(const binomial_heap& rhs);
    binomial_heap(binomial_heap&& rhs);
    binomial_heap& operator=(const binomial_heap& rhs);
    binomial_heap& operator=(binomial_heap&& rhs);
    ~binomial_heap();
    iterator find(T key);
    T front();
    T extract();
    void merge(binomial_heap& rhs);
    iterator insert(T key);
    template<class InputIterator>
    std::vector<iterator> multi_insert(InputIterator start, InputIterator stop);
    void decrease_key(const iterator& iter, T new_key);
    void remove(iterator&& iter);
    class iterator {
        T operator*();
        node* data;
    };
private:
    void delete_trees();
    void set_min();
    void zip();
    void fast_zip();
    void merge_lists(std::forward_list<node*>&& rhs);
    struct node {
        node();
        node(const T& key);
        node(const node& rhs);
        node(node&& rhs);
        node& operator=(const node& rhs);
        node& operator=(node&& rhs);
        ~node();
        node* search(const T& target, const Comp& compare);
        void delete_children();
        node* promote(node* to_merge);

        T key;
        std::forward_list<node*> children;
        uint64_t degree;
    };
    Comp compare;
    std::forward_list<node*> trees;
    node* min;
};


/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                              binomial_heap::iterator implementation                              *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/

/**
 *  @brief 
 *  @return 
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::iterator::operator*() { return data->key; }

/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                                binomial_heap::node implementation                                *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/
/**
 *  @brief      Default constructor for nodes
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node() : key(T()), degree(0) {}

/**
 *  @brief      Constructs a node with provided key   
 *  @param[in]  key the key of the node to be constructed
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node(const T& key) : key(key), degree(0) {}

/**
 *  @brief      Copy constructor for nodes
 *  @param[in]  rhs node to be copied
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node(
    const typename binomial_heap<T, Comp>::node& rhs
) { this->operator=(rhs); }

/**
 *  @brief      Move copy constructor for nodes
 *  @param[in]  rhs node to be copied
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node(
    typename binomial_heap<T, Comp>::node&& rhs
) { this->operator=(rhs); }

/**
 *  @brief      Assignment operator for nodes
 *  @param[in]  rhs node to be copied
 *  @return     this node by reference for operator chaining
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::node& binomial_heap<T, Comp>::node::operator=(
    const typename binomial_heap<T, Comp>::node& rhs
) {
    key = rhs.key;
    delete_children();
    children.clear();
    for(node* child: rhs.children) children.push_front(new node(child));
    return *this;
}

/**
 *  @brief      Move assignment operator for nodes
 *  @param[in]  rhs node to be copied
 *  @return     this node by reference for operator chaining
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::node& binomial_heap<T, Comp>::node::operator=(
    typename binomial_heap<T, Comp>::node&& rhs
) {
    delete_children();
    children.clear();
    key = std::move(rhs.key);
    children = std::move(rhs.children);
    degree = std::move(rhs.degree);
    return *this;
}

/**
 *  @brief      Destructor for nodes
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::node::~node() { delete_children(); }

/**
 *  @brief      Deletes all of a node's children and resets its degree to zero
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::node::delete_children() {
    for(node* child: children) delete child;
    children.clear();
    degree = 0;
}
/**
 *  @brief 
 *  @param[in]  target 
 *  @param[in]  compare 
 *  @return 
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::node* binomial_heap<T, Comp>::node::search(
    const T& target,
    const Comp& compare
) {
    if(!compare(target, key) && !compare(key, target)) return this;
    for(node* child: children) {
        node* found = child->search(target, compare);
        if(found) return found;
    }
    return nullptr;
}

/**
 *  @brief 
 *  @param[in]  to_merge 
 *  @return 
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::node* binomial_heap<T, Comp>::node::promote(
    binomial_heap<T, Comp>::node* to_merge
) {
    if(compare(key, to_merge->key)) {
        trees.push_front(to_merge);
        ++degree;
        return this;
    }
    else {
        to_merge.push_front(this);
        ++to_merge.degree;
        return to_merge;
    }
}

/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                                   binomial_heap implementation                                   *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/
/**
 *  @brief 
 *  @param[in]  compare 
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap(const Comp& compare): compare(compare), min(nullptr) {}

/**
 *  @brief 
 *  @param[in]  start 
 *  @param[in]  stop 
 *  @param[in]  compare 
 */
template<typename T, typename Comp>
template<class InputIterator>
binomial_heap<T, Comp>::binomial_heap(
    InputIterator start,
    InputIterator stop,
    const Comp& compare
) : compare(compare), min(nullptr) { multi_insert(start, stop); }

/**
 *  @brief 
 *  @param[in]  rhs 
 */
template<typename T,  typename Comp>
binomial_heap<T, Comp>::binomial_heap(const binomial_heap<T, Comp>& rhs) { this->operator=(rhs); }

/**
 *  @brief 
 *  @param[in]  rhs 
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap(binomial_heap<T, Comp>&& rhs) { this->operator=(rhs); }

/**
 *  @brief 
 *  @param[in]  rhs 
 *  @return 
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(const binomial_heap<T, Comp>& rhs) {
    delete_trees();
    trees.clear();
    compare = rhs.compare;
    for(node* tree: rhs.trees) { trees.push_front(new node(*tree)); }
    trees.reverse();
    set_min();
    return *this;
}

/**
 *  @brief 
 *  @param[in]  rhs 
 *  @return 
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(binomial_heap<T, Comp>&& rhs) {
    delete_trees();
    trees.clear();
    compare = std::move(rhs.compare);
    trees = std::move(rhs.trees);
    min = std::move(rhs.min);
    return *this;
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::~binomial_heap() { delete_trees(); }

/**
 *  @brief 
 *  @param[in]  key 
 *  @return 
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::find(T key) {
    for(node* tree: trees) {
        node* found = tree->search(key);
        if(found) return iterator(found);
    }
    throw new std::out_of_range("Key not found");
}

/**
 *  @brief 
 *  @return 
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::front() { return min->key; } //fix for null min

/**
 *  @brief 
 *  @return 
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::extract() {
    //finish
    return min->key;
}

/**
 *  @brief 
 *  @param rhs 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge(binomial_heap<T, Comp>& rhs) {

}

/**
 *  @brief 
 *  @param[in]  key 
 *  @return 
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::insert(T key) {
    node* new_tree = new node(key);
    trees.push_front(new_tree);
    if(!min) {
        min = new_tree;
    }
    else if(compare(new_tree->key, min->key)) { min = new_tree; }
    fast_zip();
    return iterator(new_tree);
}

/**
 *  @brief 
 *  @param[in]  start 
 *  @param[in]  stop 
 *  @return 
 */
template<typename T, typename Comp>
template<class InputIterator>
std::vector<typename binomial_heap<T, Comp>::iterator> binomial_heap<T, Comp>::multi_insert(
    InputIterator start,
    InputIterator stop
) {
    std::vector<iterator> iters;
    while(start != stop) { iters.push_back(insert(*start)); ++start; }
    return iters;
}


//if the key becomes equivalent to another key, iter->data bubbles above, necessary for remove
/**
 *  @brief 
 *  @param[in, out] iter 
 *  @param[in]      new_key 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::decrease_key(
    const binomial_heap<T, Comp>::iterator& iter,
    T new_key
) {

}

/**
 *  @brief 
 *  @param[in]  iter 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::remove(typename binomial_heap<T, Comp>::iterator&& iter) { 
    decrease_key(iter, min->key);
    extract();
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::delete_trees() { for(node* tree: trees) delete tree; trees.clear(); }

/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::set_min() {
    if(!trees.size()) { min = nullptr; return; }
    min = trees.front();
    for(node* tree: trees) if(compare(tree->key, min)) min = tree;
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::zip() {
    for(auto it = trees.begin(); it != trees.end() && (it + 1) != trees.end();) {
        auto next = it + 1;
        if(it->degree == next->degree) {
            *it = *it.promote(*next);
            trees.erase_after(it);
        }
        else it = next;
    }
}

//zips until no duplicate found
/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::fast_zip() {
    for(auto it = trees.begin(); it != trees.end() && (it + 1) != trees.end();) {
        auto next = it + 1;
        if(it->degree == next->degree) {
            *it = *it.promote(*next);
            trees.erase_after(it);
        }
        else return;
    }
}

/**
 *  @brief 
 *  @param[in]  rhs 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge_lists(
    std::forward_list<typename binomial_heap<T, Comp>::node*>&& rhs
) {

}

#endif