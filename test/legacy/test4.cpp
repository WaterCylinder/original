#include "forwardChain.h"
#include <iostream>
#include "vector.h"
#include "blocksList.h"

int main(){
    auto f1 = original::forwardChain({10, 4, 5, 7, 6, 3, 2});
    std::cout << f1 << std::endl;
    const auto v1 = original::vector({11, 8, 13, 16, 15});
    for (const auto& e: v1) {
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
    return 0;
}