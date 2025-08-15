#include <iostream>
#include "async.h"


int main() {
    auto simple_func = []{
        original::thread::sleep(original::seconds(1));
        return 0;
    };

    auto add_func = [](const int a, const int b){
        original::thread::sleep(original::seconds(1));
        return a + b;
    };

    const original::async::promise<int, int()> p {simple_func};
    const auto f = p.getFuture();
    std::cout << f.result() << std::endl;

    std::cout << original::async::get(simple_func) << std::endl;
    std::cout << original::async::get(add_func, 1, 5) << std::endl;
    return 0;
}
