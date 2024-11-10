
#include <iostream>

#include "algorithms.h"
#include "chain.h"
#include "vector.h"

int main()
{
    const auto v1 = original::vector({1, 2, 6, 2, 5, 3, 2});
    const auto c1 = original::chain({1, 2, 6, 2, 5, 3, 2});
    std::cout << "distance between v1.begin() and v1.end(): " << original::algorithms::distance(
            *v1.begins(), *v1.ends()) << std::endl;
    std::cout << "find the iterator pointing 6: " << original::algorithms::find(
        *v1.begins(), *v1.ends(), 6) << std::endl;
    std::cout << "number of 2 in v1: " << original::algorithms::count(
        *v1.begins(), *v1.ends(), 2) << std::endl;
    std::cout << "does sequence of v1 equals itself: " << original::printable::boolean(original::algorithms::equal(
        *v1.begins(), *v1.ends(), *v1.begins(), *v1.ends())) << std::endl;
    std::cout << "does sequence of v1 equals c1: " << original::printable::boolean(original::algorithms::equal(
    *v1.begins(), *v1.ends(), *c1.begins(), *c1.ends())) << std::endl;
    const auto c2 = original::chain({3, 1});
    std::cout << "c2 before swap: " << c2 << std::endl;
    original::algorithms::swap(*c2.begins(), *c2.ends());
    std::cout << "c2 after swap: " << c2 << std::endl;
    return 0;
}
