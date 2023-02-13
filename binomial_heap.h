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
*   4) Implement emplacing
***************************************************************************************************/

/**
 *  @brief  A binomial heap that supports fast insertion and merging
 *  @tparam T the type of the key that wil be stored in the heap
 *  @tparam Comp the comparison function that will be used for heap-ordering. defaults to std::less
 */
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
    size_t size() const;
    bool empty() const;
    iterator find(T key) const;
    T min() const;
    T extract();
    void merge(binomial_heap& rhs);
    void merge(binomial_heap&& rhs);
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
    node* _min;
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
 *  @param[in]  target the key to be found
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
 *  @param[in]  to_merge the other tree that this tree is to be merged with
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
    _min(nullptr),
    _size(0) {}

/**
 *  @brief      Range constructor for the binomial_heap class
 *  @param[in]  start the beginning of the range to be inserted into the heap
 *  @param[in]  stop the end of the range to be inserted into the heap
 *  @param[in]  compare the comparison functor for heap-ordering, defaults to std::less<T>
 */
template<typename T, typename Comp>
template<class InputIterator>
binomial_heap<T, Comp>::binomial_heap(
    InputIterator start,
    InputIterator stop,
    const Comp& compare
) : compare(compare), _min(nullptr), _size(0) { multi_insert(start, stop); }

/**
 *  @brief      Copy constructor for the binomial_heap class. Performs a deep copy.
 *  @param[in]  rhs the binomial_heap whose contents are to be copied
 */
template<typename T,  typename Comp>
binomial_heap<T, Comp>::binomial_heap(const binomial_heap<T, Comp>& rhs) { this->operator=(rhs); }

/**
 *  @brief      Move constructor for the binomial_heap class
 *  @param[in]  rhs the binomial_heap whose contents are to be moved
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap(binomial_heap<T, Comp>&& rhs) { this->operator=(rhs); }

/**
 *  @brief      Assignment operator for the binomial_heap class. Performs a deep copy.
 *  @param[in]  rhs the binomial_heap to be copied
 *  @return     this binomial_heap by reference for operator chaining
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(const binomial_heap<T, Comp>& rhs) {
    delete_trees();
    compare = rhs.compare;
    _size = rhs._size;
    for(node* tree: rhs.trees) { trees.push_front(new node(*tree)); }
    trees.reverse();
    set_min();
    return *this;
}

/**
 *  @brief      Move assignment operator for the binomial_heap class.
 *  @param[in]  rhs the binomial_heap to be moved
 *  @return     this binomial_heap by reference for operator chaining
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(binomial_heap<T, Comp>&& rhs) {
    delete_trees();
    compare = std::move(rhs.compare);
    trees = std::move(rhs.trees);
    _min = std::move(rhs._min);
    _size = std::move(rhs._size);
    return *this;
}

/**
 *  @brief  Destructor for the binomial_heap class
 */
template<typename T, typename Comp>
binomial_heap<T, Comp>::~binomial_heap() { delete_trees(); }

/**
 *  @brief  Gets the size of the heap
 *  @return the size of the heap
 */
template<typename T, typename Comp>
size_t binomial_heap<T, Comp>::size() const { return _size; }

/**
 *  @brief  Returns whether or not the heap is empty
 *  @return true if the heap has zero elements. Otherwise,
 *          false
 */
template<typename T, typename Comp>
bool binomial_heap<T, Comp>::empty() const { return !_size; }


/**
 *  @brief      Finds the first occurrence of a certain key in the heap by iterating through
 *              all of the values until it finds an element with that key. Requires linear time.
 *  @param[in]  key the key to be searched for in the heap
 *  @return     an iterator containing the element that was searched for in the heap
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::find(T key) const {
    for(node* tree: trees) {
        node* found = tree->search(key);
        if(found) return iterator(found);
    }
    throw new std::out_of_range("Key not found");
}

/**
 *  @brief  Gets the value of the minimum element in the heap
 *  @return the value of the minimum element in the heap.
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::min() const {
    if(_min) return _min->key;
    throw new std::out_of_range("Empty");
}

/**
 *  @brief  Extracts the minimum element from the heap. O(log n) time.
 *  @return the value of the minimum element in the heap.
 */
template<typename T, typename Comp>
T binomial_heap<T, Comp>::extract() {
    T min_val = _min->key;
    trees.remove(_min);
    _min->children.reverse();
    merge_lists(std::move(_min->children));
    _min->children = std::forward_list<node*>();
    delete _min;
    set_min();
    --_size;
    return min_val;
}

/**
 *  @brief          Merges two heaps, emptying the passed heap. O(log n) time.
 *  @param[in, out] rhs the heap to be emptied and merged with this heap
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge(binomial_heap<T, Comp>& rhs) {
    auto comp_copy = rhs.compare;
    merge(std::move(rhs));
    rhs = binomial_heap(comp_copy);
}

/**
 * @brief       Merges two heaps, destroying the passed heap. O(log n) time.
 * @param[in]   rhs the heap to be merged with this heap
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge(binomial_heap<T, Comp>&& rhs) {
    _size += rhs._size;
    if(compare(rhs._min->key, _min->key)) _min = rhs._min;
    merge_lists(rhs.trees);
}

/**
 *  @brief      Inserts a key into the heap. O(1) am. time.
 *  @param[in]  key the key to be inserted into the heap
 *  @return     an iterator containing the node that was just inserted into the heap
 */
template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::insert(T key) {
    ++_size;
    node* new_tree = new node(key);
    trees.push_front(new_tree);
    if(!_min) {
        _min = new_tree;
    }
    else if(compare(new_tree->key, _min->key)) { _min = new_tree; }
    fast_zip();
    return iterator(new_tree);
}

/**
 *  @brief      Inserts a range of elements into the heap
 *  @param[in]  start the beginning of the range to be inserted into the heap
 *  @param[in]  stop the end of the range to be inserted into the heap
 *  @return     a vector containing the respective iterators for each of the elements that were
 *              inserted into the heap.
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


/**
 *  @brief          Decreases the key of the node contained within the passed iterator. If the
 *                  key passed in is equivalent to an existing key within the tree of the 
 *                  referenced node, the referenced node will bubble above the existing equivalent
 *                  node (for remove()). O(log n) time.
 *  @param[in, out] it an iterator containing the node whose key is to be decreased
 *  @param[in]      new_key the value the key is to be decreased to
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::decrease_key(
    const binomial_heap<T, Comp>::iterator& it,
    T new_key
) {
    if(!compare(new_key, it.data->key)) throw new std::invalid_argument("Invalid new key.");
    //finish
}

/**
 *  @brief      Removes the specified element from the heap
 *  @param[in]  it iterator of the element that is to be removed
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::remove(typename binomial_heap<T, Comp>::iterator&& it) {
    decrease_key(it, _min->key);
    extract();
}

/**
 *  @brief  Empties the heap, destroying all elements. Requires linear time.
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::delete_trees() {
    for(node* tree: trees) delete tree;
    trees.clear();
    _size = 0;
}

/**
 *  @brief  Finds the min value of all of the roots. Requires logarithmic time.
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::set_min() {
    if(trees.empty()) { _min = nullptr; return; }
    _min = trees.front();
    for(node* tree: trees) if(compare(tree->key, _min->key)) _min = tree;
}

/**
 *  @brief  Zips up the tree list, merging together duplicate degree trees.
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

/**
 *  @brief  Zips up the tree list until it reaches any two distinct degrees, at which point merging
 *          stops. Meant for insertion.
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
 *  @brief      Merges this tree list with rhs, zipping up the list afterwards.
 *  @param[in]  rhs the list with which this list is to be merged.
 */
template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge_lists(
    std::forward_list<typename binomial_heap<T, Comp>::node*>&& rhs
) { trees.merge(rhs, [] (node* a, node* b) { return a->degree < b->degree; }); zip(); }
#endif