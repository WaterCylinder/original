#include <iostream>
#include "thread.h"
#include "maps.h"

int main()
{
    auto task1 = [](const int a, const std::string& b)
    {
        std::cout << b << a << std::endl;
    };
    original::pThread t1{task1, 1, "show: "};
    t1.join();
    original::pThread t2{task1, 2, "show: "};
    t2.join();
    original::pThread t3{task1, 3, "show: "};
    auto t4 = std::move(t3);
    t4.join();

    original::pThread t5{task1, 4, "show: "};
    original::pThread t6{task1, 5, "show: "};
    if (t5) {
        std::cout << "t5 is valid" << std::endl;
    }
    t6 = std::move(t5);
    if (!t5) {
        std::cout << "t5 is not valid" << std::endl;
    }
    t6.join();

    original::thread t7{task1, 6, "show: "};
    original::thread t8{task1, 7, "show: "};

    original::pThread t9{task1, 8, "show: "};
    original::thread t10{std::move(t9), true};

    original::pThread t11{task1, 9, "show: "};
    original::thread t12{std::move(t11)};

    original::thread t13;

    original::thread t14{task1, 10, "show: "};
    t14.detach();

    auto j1 = original::JMap<int, int>();
    const auto l = {0, 1, 2, 4, 2, 2};
    for (const auto& e: l) {
        if (j1.containsKey(e)){
            j1[e] += 1;
        } else {
            j1.add(e, 1);
        }
    }
    std::cout << "j1: " << j1 << std::endl;


    auto j2 = original::JMap<int, int>();
    for (int i = 0; i < 20; ++i) {
        j2.add(i, 1);
    }
    std::cout << j2 << std::endl;
    for (int i = 19; i >= 0; --i) {
        j2.remove(i);
    }
    std::cout << j2 << std::endl;
    return 0;
}