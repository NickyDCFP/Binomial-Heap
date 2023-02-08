#ifndef BINOMIAL_HEAP
#define BINOMIAL_HEAP 1
#include <forward_list>
#include <vector>
#include <functional>

template<typename T, typename Comp = std::less<T>>
class binomial_heap {
    struct node;
public:
    class iterator;
    binomial_heap(const Comp& compare = Comp());
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
    std::vector<iterator> multi_insert(InputIterator start, InputIterator end);
    void decrease_key(const iterator& iter, T new_key);
    void remove(iterator&& iter);
    class iterator {
        T operator*();
        node* data;
    };
private:
    void delete_trees();
    void zip();
    void fast_zip();
    void merge_lists(std::forward_list<node*>&& rhs);
    struct node {
        node();
        node(const T& key);
        node(const node& rhs);
        node(node&&) = default;
        node& operator=(const node& rhs);
        ~node();
        node* search(const T& target, const Comp& compare);
        void delete_children();

        T key;
        std::forward_list<node*> children;
    };
    Comp compare;
    std::forward_list<node*> trees;
    node* min;
};

#include <stdexcept>

/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                              binomial_heap::iterator implementation                              *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/
template<typename T, typename Comp>
T binomial_heap<T, Comp>::iterator::operator*() { return data->key; }

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
binomial_heap<T, Comp>::node::node(const T& key) : key(key) {}

template<typename T, typename Comp>
binomial_heap<T, Comp>::node::node(
    const typename binomial_heap<T, Comp>::node& rhs
) { this->operator=(rhs); }

template<typename T, typename Comp>
typename binomial_heap<T, Comp>::node& binomial_heap<T, Comp>::node::operator=(
    const typename binomial_heap<T, Comp>::node& rhs
) {
    return *this;
}

template<typename T, typename Comp>
binomial_heap<T, Comp>::node::~node() { delete_children(); children.clear(); }

template<typename T, typename Comp>
void binomial_heap<T, Comp>::node::delete_children() { for(node* child: children) delete child; }

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

/***************************************************************************************************
*                                                                                                  *
*                                                                                                  *
*                                   binomial_heap implementation                                   *
*                                                                                                  *
*                                                                                                  *
***************************************************************************************************/
template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap(const Comp& compare): compare(compare) {}

template<typename T,  typename Comp>
binomial_heap<T, Comp>::binomial_heap(const binomial_heap<T, Comp>& rhs) { this->operator=(rhs); }

template<typename T, typename Comp>
binomial_heap<T, Comp>::binomial_heap(binomial_heap<T, Comp>&& rhs) { this->operator=(rhs); }

template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(const binomial_heap<T, Comp>& rhs) {
    //only assign each field
    return *this;
}

template<typename T, typename Comp>
binomial_heap<T, Comp>& binomial_heap<T, Comp>::operator=(binomial_heap<T, Comp>&& rhs) {
    //std::move each field
    return *this;
}

template<typename T, typename Comp>
binomial_heap<T, Comp>::~binomial_heap() { delete_trees(); trees.clear(); }

template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::find(T key) {
    for(node* tree: trees) {
        node* found = tree->search(key);
        if(found) return iterator(found);
    }
    throw new std::out_of_range("Key not found");
}

template<typename T, typename Comp>
T binomial_heap<T, Comp>::front() { return min->key; }

template<typename T, typename Comp>
T binomial_heap<T, Comp>::extract() {
    return min->key;
}

template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge(binomial_heap<T, Comp>& rhs) {

}

template<typename T, typename Comp>
typename binomial_heap<T, Comp>::iterator binomial_heap<T, Comp>::insert(T key) {
    node* new_tree = new node(key);
    trees.push_front(new_tree);
    fast_zip();
}

template<typename T, typename Comp>
template<class InputIterator>
std::vector<typename binomial_heap<T, Comp>::iterator> binomial_heap<T, Comp>::multi_insert(
    InputIterator start,
    InputIterator end
) {
    std::vector<iterator> iters;
    while(start != end) iters.push_back(insert(*start));
}

//if the key becomes equivalent to another key, this key bubbles above
template<typename T, typename Comp>
void binomial_heap<T, Comp>::decrease_key(
    const binomial_heap<T, Comp>::iterator& iter,
    T new_key
) {

}

template<typename T, typename Comp>
void binomial_heap<T, Comp>::remove(typename binomial_heap<T, Comp>::iterator&& iter) { 
    decrease_key(iter, min->key);
    extract();
}

template<typename T, typename Comp>
void binomial_heap<T, Comp>::delete_trees() { for(node* tree: trees) delete tree; }

template<typename T, typename Comp>
void binomial_heap<T, Comp>::zip() {

}

//zips until no duplicate found
template<typename T, typename Comp>
void binomial_heap<T, Comp>::fast_zip() {

}

template<typename T, typename Comp>
void binomial_heap<T, Comp>::merge_lists(
    std::forward_list<typename binomial_heap<T, Comp>::node*>&& rhs
) {

}

#endif