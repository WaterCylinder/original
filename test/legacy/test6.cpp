#include <iostream>
#include "thread.h"
#include <thread>
#include "maps.h"
#include "mutex.h"
#include "zeit.h"


using namespace original::literals;

int main()
{
    original::pMutex mutex;
    auto task1 = [&](const int a, const std::string& b)
    {
        original::uniqueLock lock{mutex};
        std::cout << b << a << std::endl;
    };
    original::pThread t1{task1, 1, "show: "};
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "id t1: " << t1.id() << std::endl;
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
    original::thread t10{std::move(t9), original::thread::AUTO_JOIN};
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "t10 id: " << t10.id() << std::endl;
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

    std::cout << "On win64: " << original::printable::formatString(original::ON_WIN64()) << std::endl;
    std::cout << "Using GCC: " << original::printable::formatString(original::USING_GCC()) << std::endl;

    struct testClass {
        int a;

        explicit testClass(int a) : a(a) {}
        void print(const std::string& b) const{
            std::cout << b << this->a << std::endl;
        }
    };

    testClass tc{1};
    original::thread t15{&testClass::print, &tc, "print(): "};

    original::pMutex m1;
    original::pMutex m2;
    original::multiLock ml{m1, m2};

    auto d1 = original::time::duration(100, original::time::MILLISECOND);
    std::cout << d1 << std::endl;

    const auto now_utc = original::time::UTCTime::now();
    std::cout << "London now: " << now_utc << std::endl;
    std::cout << "Beijing now: " << now_utc + 8_h << std::endl;
    std::cout << "test convert: " << original::time::UTCTime{static_cast<original::time::point>(now_utc)} << std::endl;
    std::cout << static_cast<original::integer>(now_utc.weekday()) << std::endl;

    constexpr original::integer sec = 5;
    std::cout << "Sleep before: " << original::time::UTCTime::localNow() << std::endl;
    std::cout << "Sleep for: " << sec << " second(s)." << std::endl;
    original::thread::sleep(original::time::duration{sec, original::time::SECOND});
    std::cout << "Sleep after: " << original::time::UTCTime::localNow() << std::endl;
    return 0;
}