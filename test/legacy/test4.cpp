#include "forwardChain.h"
#include <iostream>

#include "vector.h"
#include "blocksList.h"
#include "prique.h"
#include "chain.h"
#include "tuple.h"
#include "stack.h"
#include "bitSet.h"

int main(){
    auto f1 = original::forwardChain({10, 4, 5, 7, 6, 3, 2});
    std::cout << f1 << std::endl;
    for (const auto v1 = original::vector({11, 8, 13, 16, 15}); const auto& e: v1) {
        f1.pushEnd(e);
    }
    std::cout << f1 << std::endl;
    auto v2 = original::vector({0, 1, 3});
    v2.push(2, 2);
    std::cout << "v2: " << v2 << std::endl;
    auto bl1 = original::blocksList({10, 4, 5, 7, 6, 3, 2, 11, 8, 13, 16, 15});
    std::cout << "bl1: " << bl1 << std::endl;
    std::cout << "bl1[3] = " << bl1[3] << std::endl;
    for (int i = 15; i < 30; ++i) {
        bl1.pushEnd(i);
    }
    std::cout << "bl1: " << bl1 << std::endl;
    while (!bl1.empty()){
        bl1.pop(bl1.size() / 2);
        std::cout << "bl1: " << bl1 << std::endl;
    }
    for (int i = 0; i < 20; i++){
        bl1.push(bl1.size() / 2, i);
        std::cout << "bl1: " << bl1 << std::endl;
    }
    original::prique<int, original::increaseComparator, original::chain> pq = {40, 20, 10, 30, 50, 70, 60, 20, 90, 80, 80, 40};
    pq.push(10);
    while (!pq.empty()){
        std::cout << pq << std::endl;
        pq.pop();
    }
    original::couple<const int, const int> couple = {1, 1};
    std::cout << couple << std::endl;
    #define test4_lst1 {1, 5, 8, 10, 25, 70, 64, 3, 9, 2, 11, 14, 26, 39, 42, 50}
    original::prique pq2 = test4_lst1;
    original::array arr = test4_lst1;
    original::algorithms::sort(arr.first(), arr.last(), original::increaseComparator<int>());
    int i = 0;
    while (!pq2.empty()){
        if (pq2.pop() != arr[i])
            std::cout << "Not equal at index " << i << std::endl;
        i += 1;
    }
    auto t1 = original::tuple{1, 1, 1};
    auto t2 = original::tuple{1, 1, 2};
    std::cout << original::printable::formatString(t1 < t2) << std::endl;
    std::cout << t1 << std::endl;
    auto t3 = original::tuple{original::array({1, 2}), original::vector({3, 4}), original::chain({5, 6})};
    std::cout << t3 << std::endl;
    auto t4 = original::tuple{original::array({1, 2, 3}), original::couple{1, 0.5}, original::blocksList{true, false}, original::vector{3, 2, 9, 5, 8, 6, 1}};
    std::cout << t4 << std::endl;
    std::cout << t4.get<2>() << std::endl;
    auto cp = t4.get<1>();
    cp.set<1>(0.6);
    t4.set<1>(cp);
    std::cout << cp << std::endl;
    std::cout << t4 << std::endl;
    auto t5 = original::tuple{original::stack{1, 2, 1}};
    std::cout << t5 << std::endl;
    auto t6 = original::tuple{original::prique{3, 9, 5, 4, 6, 1, 8}, original::bitSet{true, false, true}};
    std::cout << t6 << std::endl;
    auto t7 = t4.slice<0, 2>();
    std::cout << t7 << std::endl;
    std::cout << t7.size() << std::endl;
    auto t8 = t4 + t4.slice<2, 1>() + t4.slice<1, 3>() + original::makeTuple(t4.get<1>());
    std::cout << t8 << std::endl;
    return 0;
}