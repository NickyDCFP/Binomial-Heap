#include <list>
#include <functional>

template<typename T, typename Comp = std::less<T>>
class binomial_heap {
public:
    binomial_heap();
    binomial_heap(const binomial_heap& rhs);
    binomial_heap(binomial_heap&&) = default;
    binomial_heap& operator=(const binomial_heap& rhs);
    ~binomial_heap();
    void delete_trees();
    /*necessary functionality
        report min
        union
        insert
        extract min
        decrease key --> consider that maybe the binomial_node struct should be public so that
                         decrease keys can be called with pointers 
        delete
    */
private:
    struct node {
        node();
        node(const node& rhs);
        node(node&&) = default;
        node& operator=(const node& rhs);
        ~node();
        void delete_children();
            T key;
            std::list<node*> children;
    };
    std::list<node*> trees;
};