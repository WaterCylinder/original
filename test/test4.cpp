#include <iostream>
#include "vector.h"

int main(){
    auto v1 = original::vector({10, 4, 5, 7, 6, 3, 2});
    std::cout << v1 << std::endl;
    const auto v2 = original::vector({11, 8, 13, 16, 15});
    for (const auto& e: v2) {
        v1.pushEnd(e);
    }
    std::cout << v1 << std::endl; // ???
    return 0;
}