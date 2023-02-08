#include <list>

class binomial_heap {
public:
private:
    struct binomial_node {
            std::list<binomial_node> children;
    };
    std::list<binomial_node> trees;
};