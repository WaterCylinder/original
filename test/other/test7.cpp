#include <iostream>
#include "array.h"
#include "async.h"

original::array<int> matrixAdd(const original::array<int>& a, const original::array<int>& b)
{
    const auto len = original::max(a.size(), b.size());
    original::array<int> result(len);
    for (original::integer i = 0; i < len; i++) {
        if (i < a.size())
            result[i] += a[i];
        if (i < b.size())
            result[i] += b[i];
    }
    return result;
}

int main() {
    auto simple_func = []{
        original::thread::sleep(original::seconds(1));
        return 0;
    };

    auto add_func = [](const int a, const int b){
        original::thread::sleep(original::seconds(1));
        return a + b;
    };

    auto sub_func = [](const int a, const int b) {
        original::thread::sleep(original::seconds(1));
        return a - b;
    };

    const original::async::promise<int, int()> p {simple_func};
    const auto f = p.getFuture();
    std::cout << f.result() << std::endl;

    std::cout << original::async::get(simple_func) << std::endl;
    std::cout << original::async::get(add_func, 1, 5) << std::endl;

    std::cout << original::async::get(matrixAdd, original::array{1, 2, 3, 4}, original::array{2, 4, 5, 7, 12}) << std::endl;

    const auto task = original::async::promise<int, int(int, int)>{sub_func};
    std::cout << task(10, 5) << std::endl;
    return 0;
}
