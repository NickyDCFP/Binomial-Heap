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

#endif