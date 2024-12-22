#include "stack.h"
#include <iostream>
#include <queue.h>
#include "chain.h"
#include "vector.h"
#include "algorithms.h"
#include "filter.h"
#include "blocksList.h"
#include "transformStream.h"

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
    auto s2 = original::stack<int, original::vector>();
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
    original::algorithms::forEach(*s2.begins(), *s2.ends(), original::addOptTransform(1));
    std::cout << "s2: " << s2 << std::endl;
    original::algorithms::forEach(*s2.begins(), 0, original::addOptTransform(-1));
    std::cout << "s2: " << s2 << std::endl;
    original::algorithms::forEach(*s2.begins(), s2.size(), original::addOptTransform(1) + original::multiOptTransform(2));
    std::cout << "s2: " << s2 << std::endl;
    original::algorithms::fill(*s2.begins(), *s2.begins());
    std::cout << "s2: " << s2 << std::endl;
    auto c2 = original::chain({1, 3, 6, 9, 8, 4, 2});
    auto c3 = original::chain({7, 5});
    std::cout << c2 << " size: " << c2.size() << std::endl;
    std::cout << c3 << " size: " << c3.size() << std::endl;
    c2 += c3;
    std::cout << c2 << " size: " << c2.size() << std::endl;
    std::cout << c3 << " size: " << c3.size() << std::endl;
    original::stack<original::stack<int>> s3;
    for (int i = 0; i < 10; ++i) {
        auto ss = original::stack<int>();
        ss.push(i);
        s3.push(ss);
    }
    std::cout << "s3 before: " << s3 << std::endl;
    s3.pop();
    s3.pop();
    std::cout << "s3 after: " << s3 << std::endl;
    original::queue<original::queue<int>, original::vector> q3;
    for (int i = 0; i < 10; ++i) {
        auto qq = original::queue<int>();
        qq.push(i);
        q3.push(qq);
    }
    std::cout << "q3 before: " << q3 << std::endl;
    q3.pop();
    q3.pop();
    std::cout << "q3 after: " << q3 << std::endl;
    original::stack<original::queue<int>> s4;
    original::stack<int, original::array> s5; // 这种情况不应该通过编译
    return 0;
}