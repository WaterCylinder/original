
#include <iostream>

#include "algorithms.h"
#include "chain.h"
#include "couple.h"
#include "vector.h"
#include "maths.h"
#include "comparator.h"
#include "filter.h"
#include "transformStream.h"


int main()
{
    const auto v1 = original::vector({1, 2, 6, 2, 5, 3, 2});
    const auto c1 = original::chain({1, 2, 6, 2, 5, 3, 2});
    std::cout << "distance between v1.begin() and v1.end(): " << original::algorithms::distance(
            *v1.begins(), *v1.ends()) << std::endl;
    std::cout << "find the iterator pointing at 6: " << original::algorithms::find(
        *v1.begins(), *v1.ends(), 6) << std::endl;
    std::cout << "number of 2 in v1: " << original::algorithms::count(
        *v1.begins(), *v1.ends(), 2) << std::endl;
    std::cout << "does sequence of v1 equals itself: " << original::printable::formatCString(original::algorithms::equal(
        *v1.begins(), *v1.ends(), *v1.begins(), *v1.ends())) << std::endl;
    std::cout << "does sequence of v1 equals c1: " << original::printable::formatCString(original::algorithms::equal(
    *v1.begins(), *v1.ends(), *c1.begins(), *c1.ends())) << std::endl;
    const auto c2 = original::chain({3, 1});
    std::cout << "c2 before swap: " << c2 << std::endl;
    original::algorithms::swap(*c2.begins(), *c2.ends());
    std::cout << "c2 after swap: " << c2 << std::endl;
    auto a1 = original::array<original::couple<char, int>>(10);
    for (int i = 0; i < a1.size(); i++)
    {
        a1[i].first() = static_cast<char>('a' + i);
    }
    std::cout << "before: " << a1 << std::endl;
    for (const auto e : v1)
    {
        a1[e].second() += 1;
    }
    std::cout << "after: " << a1 << std::endl;
    auto v2 = original::vector<double>();
    for (int i = 0; i < 10; ++i) {
        v2.pushEnd(double{});
    }
    std::cout << "before: " << v2 << std::endl;
    original::algorithms::fill(*v2.begins(), *v2.ends(), PI);
    std::cout << "after: " << v2 << std::endl;
    std::cout << "number of elements less than 3 in v1: "
    << original::algorithms::count(*v1.begins(), *v1.ends(), [](int e) {
        return e < 3;
    }) << std::endl;
    std::cout << "find the element greater than 4 and less than 6 in c1: "
    << original::algorithms::find(*c1.begins(), *c1.ends(), [](int e) {
        return e > 4 && e < 6;
    }) << std::endl;
    std::cout << "count the elements in c1, range in [1,5]: "
        << original::algorithms::count(
                *c1.begins(), *c1.ends(), original::rangeFilter(1,5)) << std::endl;
    auto c3 = original::chain<int>();
    for (int i = 0; i < 8; ++i) {
        c3.pushEnd(i);
    }
    std::cout << "before: " << c3 << std::endl;
    original::algorithms::fill(*c3.begins(), *c3.ends());
    std::cout << "after1: " << c3 << std::endl;
    original::algorithms::fill(*c3.begins(), 3, 233);
    std::cout << "after2: " << c3 << std::endl;
    original::algorithms::fill(*c3.begins(), *c3.ends());
    c3.forEach(original::addOptTransform(3));
    std::cout << "after3: " << c3 << std::endl;
    original::algorithms::fill(*c3.begins(), *c3.ends());
    c3.forEach(
        original::addOptTransform(3)
                + original::addOptTransform(7)
                + original::addOptTransform(10)
                + original::addOptTransform(10));
    std::cout << "after4: " << c3 << std::endl;
    original::algorithms::fill(*c3.begins(), *c3.ends());
    c3.forEach(
    original::addOptTransform(3)
            + original::addOptTransform(7)
            + original::multiOptTransform(10));
    std::cout << "after5: " << c3 << std::endl;
    auto v3 = original::vector<int>();
    c3.forEach(original::addOptTransform(10)
                + original::copyTransform(v3));
    std::cout << "v3: " << v3 << std::endl;
    auto v4 = original::vector<std::string>();
    for (int i = 0; i < 20; ++i)
    {
        v4.pushEnd(std::to_string(i));
    }
    std::cout << "v4: " << v4 << std::endl;
    v4.forEach(original::addOptTransform(std::string("^-^")));
    std::cout << "v4: " << v4 << std::endl;
    v4.forEach(original::assignOptTransform(std::string("QwQ")));
    std::cout << "v4: " << v4 << std::endl;
    original::algorithms::fill(*c3.begins(), *c3.ends());
    c3.forEach();
    std::cout << "after6: " << c3 << std::endl;
    original::algorithms::fill(*c3.begins(), *c3.ends(), 1);
    c3.forEach(
        original::multiOptTransform(5)
        + (original::addOptTransform(3)
        + original::addOptTransform(7))
        + (original::addOptTransform(7)
        + original::multiOptTransform(3))
    );
    std::cout << "after7: " << c3 << std::endl;
    return 0;
}
