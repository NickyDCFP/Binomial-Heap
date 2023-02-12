/**
 *  @file   binomial_heap.h
 *  @brief  A templated binomial heap that supports constant time amortized insertion and
 *          logarithmic time merging.
 * 
 *  @author Nicky Kriplani
 *  @date   February 12, 2023
*/
#ifndef BINOMIAL_HEAP
#define BINOMIAL_HEAP 1
#include <forward_list>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iterator>
#include <algorithm>
/***************************************************************************************************
* Next steps:                                                                                      *
*   1) Add parent pointers to support decrease_key and extraction                                  *
*   2) Finish `void merge(binomial_heap&)` and add an overload that takes a heap by move           *
*       a) The pass-by-reference version should empty the passed heap                              *
*   3) Child lists are stored in reverse order relative to the tree list, which means they need to *
*      be reversed every merge. Two possible solutions.                                            *
*       a) Refactor the code to use std::list instead of std::forward_list, which would probably   *
*          also have other slight heuristic time advantages.                                       *
*       b) Store an iterator permanently sitting at the end of the list so that insertion to the   *
*          end will be instant, meaning the lists will be in the same order. Saves space but is    *
*          more complex.                                                                           *
*       c) Will spend some time looking into the heuristic advantages of swapping to list and then *
*          make a decision based on how tangible they are.                                         *
***************************************************************************************************/

template<typename T, typename Comp = std::less<T>>
class binomial_heap {
    struct node;
public:
    class iterator;
    explicit binomial_heap(const Comp& compare = Comp());
    template<class InputIterator>
    binomial_heap(InputIterator start, InputIterator stop, const Comp& compare = Comp());
    binomial_heap(const binomial_heap& rhs);
    binomial_heap(binomial_heap&& rhs);
    binomial_heap& operator=(const binomial_heap& rhs);
    binomial_heap& operator=(binomial_heap&& rhs);
    ~binomial_heap();
    size_t size();
    bool empty();
    iterator find(T key);
    T front();
    T extract();
    void merge(binomial_heap& rhs); //one by reference and one by move (fix)
    iterator insert(T key);
    template<class InputIterator>
    std::vector<iterator> multi_insert(InputIterator start, InputIterator stop);
    void decrease_key(const iterator& it, T new_key);
    void remove(iterator&& it);
    class iterator {
    public:
        explicit iterator(node* data);
        T operator*();
    private:
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
        node* promote(node* to_merge, const Comp& compare);

        T key;
        std::forward_list<node*> children;
        uint64_t degree;
    };
    Comp compare;
    std::forward_list<node*> trees;
    node* min;
    size_t _size;
};


/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                              binomial_heap::iterator implementation                              *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/

/**
 *  @brief  Dereference operator for the iterator class
 *  @return the key of the node the iterator is holding
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::iterator::operator*() { return data->key; }

/**
 *  @brief  Constructor for the iterator class
*/
template<typename T, typename Comp>
binomial_heap<T, Comp>::iterator::iterator(binomial_heap<T, Comp>::node* data) : data(data) {}
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
 *  @brief      Searches for a node in this tree with a particular node. Time complexity is linear
 *              with the number of nodes in the tree.
 *  @param[in]  target  the key to be found
 *  @param[in]  compare the comparison function to test if the key has been found
 *  @return     if a node with the valid key is found, the pointer to the first found occurrence of
 *              that value
 *              otherwise, nullptr
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
 *  @brief      Merges two trees in constant time, making the smaller of the two roots the new root
 *  @param[in]  to_merge    the other tree that this tree is to be merged with
 *  @return     The new root to the tree to be replaced in the list
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::node* binomial_heap<T, Comp>::node::promote(
    binomial_heap<T, Comp>::node* to_merge,
    const Comp& compare
) {
    if(compare(key, to_merge->key)) {
        children.push_front(to_merge);
        ++degree;
        return this;
    }
    to_merge->children.push_front(this);
    ++to_merge->degree;
    return to_merge;
}

/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                                   binomial_heap implementation                                   *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/
/**
 *  @brief      Default constructor for the binomial_heap class
 *  @param[in]  compare the comparison functor for heap-ordering, defaults to std::less<T>
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap(const Comp& compare) : 
    compare(compare),
    min(nullptr),
    _size(0) {}

/**
 *  @brief      Range constructor for the binomial_heap class
 *  @param[in]  start   the beginning of the range to be inserted into the heap
 *  @param[in]  stop    the end of the range to be inserted into the heap
 *  @param[in]  compare the comparison functor for heap-ordering, defaults to std::less<T>
 */
template<typename T, typename Comp>
template<class InputIterator>
binomial_heap<T, Comp>::binomial_heap(
    InputIterator start,
    InputIterator stop,
    const Comp& compare
) : compare(compare), min(nullptr), _size(0) { multi_insert(start, stop); }

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
    _size = rhs._size;
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
    _size = std::move(rhs._size);
    return *this;
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::~binomial_heap() { delete_trees(); }

template<typename T, typename Comp>
size_t binomial_heap<T, Comp>::size() { return _size; }

template<typename T, typename Comp>
bool binomial_heap<T, Comp>::empty() { return !_size; }


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
T binomial_heap<T, Comp>::front() { if(min) return min->key; throw new std::out_of_range("Empty"); }

/**
 *  @brief 
 *  @return 
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::extract() {
    T min_val = min->key;
    trees.remove(min);
    min->children.reverse(); //VERY VERY BAD NEED TO FIXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    merge_lists(std::move(min->children));
    min->children = std::forward_list<node*>();
    delete min;
    set_min();
    --_size;
    return min_val;
}

/**
 *  @brief 
 *  @param rhs 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge(binomial_heap<T, Comp>& rhs) {
    _size += rhs._size;
    if(compare(rhs.min->key, min->key)) min = rhs.min;
    //unfinished
}

/**
 *  @brief 
 *  @param[in]  key 
 *  @return 
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::insert(T key) {
    ++_size;
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
    _size += stop - start;
    return iters;
}


//if the key becomes equivalent to another key, it->data bubbles above, necessary for remove
/**
 *  @brief 
 *  @param[in, out] it 
 *  @param[in]      new_key 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::decrease_key(
    const binomial_heap<T, Comp>::iterator& it,
    T new_key
) {
//in order to deal with decrease key, might need parent pointers (fix)
}

/**
 *  @brief 
 *  @param[in]  it 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::remove(typename binomial_heap<T, Comp>::iterator&& it) {
    decrease_key(it, min->key);
    extract();
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::delete_trees() {
    for(node* tree: trees) delete tree;
    trees.clear();
    _size = 0;
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::set_min() {
    if(trees.empty()) { min = nullptr; return; }
    min = trees.front();
    for(node* tree: trees) if(compare(tree->key, min->key)) min = tree;
}

/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::zip() {
    for(auto it = trees.begin(); it != trees.end() && std::next(it, 1) != trees.end();) {
        auto next = std::next(it, 1);
        if((*it)->degree > (*next)->degree) std::iter_swap(it, next);
        if((*it)->degree == (*next)->degree) {
            *it = (*it)->promote(*next, compare);
            trees.erase_after(it);
        }
        else it = next;
    }
}

//zips until no duplicate found, meant for insertion
/**
 *  @brief 
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::fast_zip() {
    for(auto it = trees.begin(); it != trees.end() && (std::next(it, 1)) != trees.end();) {
        auto next = std::next(it, 1);
        if((*it)->degree == (*next)->degree) {
            *it = (*it)->promote(*next, compare);
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
) { trees.merge(rhs, [] (node* a, node* b) { return a->degree < b->degree; }); zip(); }

#endif