#include "stack.h"
#include <iostream>
#include <queue.h>
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
    std::cout << original::iterator<int>(nullptr) << std::endl;
    if (original::iterator<int>(nullptr)){
        std::cout << "state 1" << std::endl;
    } else{
        std::cout << "state 2" << std::endl;
    }
    std::cout << "c1 before1: " << c1 << std::endl;
    original::algorithms::reverse(*c1.begins(), *c1.ends());
    std::cout << "c1 after1: " << c1 << std::endl;
    original::stack<int> s1;
    std::cout << "s1 before1: " << s1 << std::endl;
    for (int i = 0; i < v1.size(); i++) {
        s1.push(i);
    }
    std::cout << "s1 after1: " << s1 << std::endl;
    original::queue<int> q1;
    std::cout << "q1 before1: " << q1 << std::endl;
    for (int i = 0; i < v1.size(); i++) {
        q1.push(i);
    }
    std::cout << "q1 after1: " << q1 << std::endl;
    for (int i = 0; i < v1.size() / 2; i++) {
        q1.pop();
    }
    std::cout << "q1 after2: " << q1 << std::endl;
    const auto q2 = original::queue(q1);
    std::cout << "q2: " << q2 << std::endl;
    auto s2 = original::stack<int, original::vector<int>>();
    std::cout << "s2: " << s2 << std::endl;
    for (int i = 0; i < 50; ++i) {
        s2.push(i);
    }
    std::cout << "s2: " << s2 << std::endl;
    for (int i = 0; i < 18; ++i) {
        s2.pop();
    }
    std::cout << "s2.top(): " << s2.top() << std::endl;
    std::cout << "s2: " << s2 << std::endl;
    return 0;
}