#include <list>
#include <functional>

template<typename T, typename Comp = std::less<T>>
class binomial_heap {
public:
    binomial_heap();
    binomial_heap(const binomial_heap& rhs);
    binomial_heap(binomial_heap&&) = default;
    binomial_heap& operator=(const binomial_heap& rhs);
private:
    struct binomial_node {
            T key;
            std::list<binomial_node> children;
    };
    std::list<binomial_node> trees;
};