#include <iostream>
#include "array.h"
#include "async.h"
#include "singleton.h"
#include "tasks.h"

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
    original::thread::sleep(original::seconds(1));
    return result;
}

int main() {
    auto simple_func = []{
        original::thread::sleep(original::seconds(1));
        return 0;
    };
    auto simple_func2 = [](const int a){
        original::thread::sleep(original::seconds(1));
        std::cout << "res = " << a << std::endl;
        return a;
    };

    auto add_func = [](const int a, const int b){
        original::thread::sleep(original::seconds(1));
        return a + b;
    };

    auto sub_func = [](const int a, const int b) {
        original::thread::sleep(original::seconds(1));
        return a - b;
    };

    auto pp = original::async::makePromise(simple_func2, 5);
    auto ff = pp.getFuture();
    original::thread  t{
        [pp = original::makeStrongPtr<decltype(pp)>(pp)] mutable
        {
            pp->run();
        }
    };
    std::cout << ff.result() << std::endl;

    std::cout << original::async::get(simple_func) << std::endl;
    std::cout << original::async::get(add_func, 1, 5) << std::endl;
    std::cout << original::async::get(matrixAdd, original::array{1, 2, 3, 4}, original::array{2, 4, 5, 7, 12}) << std::endl;
    std::cout << original::async::get(sub_func, 5, 1) << std::endl;

    original::singleton<original::taskDelegator>::init();
    auto& delegator = original::singleton<original::taskDelegator>::instance();
    auto futures = original::vector<original::async::future<int>>{};
    for (original::integer i = 0; i < 12; i++) {
        futures.pushEnd(delegator.submit(simple_func2, i));
    }
    for (auto& future : futures) {
        std::cout << future.result() << std::endl;
    }
    return 0;
}
