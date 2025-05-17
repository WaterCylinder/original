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
    return 0;
}