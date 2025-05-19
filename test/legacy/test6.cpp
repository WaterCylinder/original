#include <iostream>
#include "thread.h"

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
    return 0;
}