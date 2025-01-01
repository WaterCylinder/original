#include <forwardChain.h>
#include <iostream>
#include "vector.h"

int main(){
    auto f1 = original::forwardChain({10, 4, 5, 7, 6, 3, 2});
    std::cout << f1 << std::endl;
    const auto v2 = original::vector({11, 8, 13, 16, 15});
    for (const auto& e: v2) {
        f1.pushEnd(e);
    }
    std::cout << f1 << std::endl;
    return 0;
}