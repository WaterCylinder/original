

#include <iostream>
#include "chain.h"
#include "vector.h"
#include "algorithms.h"
#include "filter.h"

int main(){
    original::chain<std::string> c1 = {"a", "aa", "bc", "vg", "rtqy", "upn"};
    original::vector<std::string> v1 = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    std::cout << "v1 before1: " << v1 << std::endl;
    original::algorithms::copy(*c1.begins(), *c1.ends(), *v1.begins());
    std::cout << "v1 after1: " << v1 << std::endl;
    original::algorithms::fill(*v1.begins(), *v1.ends());
    std::cout << "v1 before2: " << v1 << std::endl;
    original::algorithms::copy(*c1.begins(), *c1.ends(), *v1.begins(),
                               original::greaterFilter(std::string("bc")));
    std::cout << "v1 after2: " << v1 << std::endl;
    return 0;
}