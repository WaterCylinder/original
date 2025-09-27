#include <gtest/gtest.h>
#include "syncPoint.h"
#include "thread.h"
#include "zeit.h"
#include <vector>
#include <atomic>

using namespace original;
using namespace original::literals;

TEST(SyncPointTest, BasicSync) {
    constexpr int THREAD_COUNT = 3;
    std::atomic arrived_count{0};
    std::atomic complete_called{false};

    syncPoint sp(THREAD_COUNT, [&complete_called]
    {
        complete_called = true;
    });

    std::vector<thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&arrived_count, &sp]
        {
            ++arrived_count;
            sp.arrive();
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(arrived_count, THREAD_COUNT);
    EXPECT_TRUE(complete_called);
}

TEST(SyncPointTest, MultipleRounds) {
    constexpr int THREAD_COUNT = 2;
    constexpr int ROUNDS = 3;
    std::atomic round_complete_count{0};

    syncPoint sp(THREAD_COUNT, [&round_complete_count]
    {
        ++round_complete_count;
    });

    auto worker = [&sp]
    {
        for (int i = 0; i < ROUNDS; ++i) {
            sp.arrive();
        }
    };

    std::vector<thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(round_complete_count, ROUNDS);
}

TEST(SyncPointTest, TimeoutBehavior) {
    constexpr int THREAD_COUNT = 2;
    syncPoint sp(THREAD_COUNT);

    std::atomic thread1_arrived{false};
    std::atomic thread2_arrived{false};

    thread t1([&]
    {
        sp.arrive();
        thread1_arrived = true;
    });

    // 线程2延迟启动，测试线程1是否会阻塞
    thread t2([&]
    {
        thread::sleep(100_ms);
        sp.arrive();
        thread2_arrived = true;
    });

    t1.join();
    t2.join();

    EXPECT_TRUE(thread1_arrived);
    EXPECT_TRUE(thread2_arrived);
}

TEST(SyncPointTest, SingleThreadNoWait) {
    syncPoint sp(1, []
    {
        // 完成函数
    });

    const auto start = time::point::now();
    sp.arrive(); // 单线程不应阻塞
    const auto end = time::point::now();

    EXPECT_LT(end - start, 50_ms); // 应该很快返回
}

TEST(SyncPointTest, NoFunctionProvided) {
    constexpr int THREAD_COUNT = 2;
    syncPoint sp(THREAD_COUNT); // 无完成函数

    std::atomic arrived{0};

    auto worker = [&]
    {
        ++arrived;
        sp.arrive();
    };

    thread t1(worker);
    thread t2(worker);

    t1.join();
    t2.join();

    EXPECT_EQ(arrived, THREAD_COUNT);
    // 无完成函数也应正常同步
}

TEST(SyncPointTest, ManyThreads) {
    constexpr int THREAD_COUNT = 10;
    constexpr int ROUNDS = 10;
    std::atomic arrived_count{0};
    std::atomic complete_called{false};

    syncPoint sp(THREAD_COUNT, [&complete_called] {
        complete_called = true;
    });

    std::vector<thread> threads;
    for (int i = 0; i < THREAD_COUNT * ROUNDS; ++i) {
        threads.emplace_back([&arrived_count, &sp] {
            ++arrived_count;
            sp.arrive();
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(arrived_count, THREAD_COUNT * ROUNDS);
    EXPECT_TRUE(complete_called);
}

TEST(SyncPointTest, SequentialArrival) {
    constexpr int THREAD_COUNT = 3;
    syncPoint sp(THREAD_COUNT);
    std::atomic completion_count{0};

    // 重新设置完成函数
    syncPoint sp_with_func(THREAD_COUNT, [&completion_count] {
        ++completion_count;
    });

    std::vector<thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&sp_with_func, i] {
            thread::sleep(i * 10_ms); // 顺序启动
            sp_with_func.arrive();
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(completion_count, 1);
}

TEST(SyncPointTest, DataConsistency) {
    constexpr int THREAD_COUNT = 4;
    constexpr int ITERATIONS = 100;
    std::atomic counter{0};
    std::atomic sync_count{0};

    syncPoint sp(THREAD_COUNT, [&sync_count] {
        ++sync_count;
    });

    auto worker = [&]() {
        for (int i = 0; i < ITERATIONS; ++i) {
            // 做一些工作
            ++counter;
            sp.arrive();
        }
    };

    std::vector<thread> threads;
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter, THREAD_COUNT * ITERATIONS);
    EXPECT_EQ(sync_count, ITERATIONS);
}

TEST(SyncPointTest, ZeroMaxArrived) {
    syncPoint sp(0);

    auto start = time::point::now();
    sp.arrive();
    auto end = time::point::now();

    EXPECT_LT(end - start, 50_ms);

    std::atomic call_count{0};
    std::vector<thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&sp, &call_count] {
            sp.arrive();
            ++call_count;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(call_count, 5);
}

TEST(SyncPointTest, ExceptionInCompleteFunction) {
    constexpr int THREAD_COUNT = 2;
    std::atomic exception_caught{false};
    std::atomic threads_continued{false};

    syncPoint sp(THREAD_COUNT, [] {
        throw std::runtime_error("Test exception");
    });

    auto worker = [&] {
        try {
            sp.arrive();
            threads_continued = true;
        } catch (...) {
            exception_caught = true;
        }
    };

    thread t1(worker);
    thread t2(worker);

    t1.join();
    t2.join();

    // 异常应该被捕获，线程应该继续执行
    EXPECT_TRUE(exception_caught);
    EXPECT_TRUE(threads_continued);
}