#include "atomic.h"
#include <gtest/gtest.h>
#include <string>
#include <thread>

using namespace original;

// ========== 基础测试 ==========
TEST(AtomicTest, StoreAndLoadInt) {
    auto a = atomic(0);
    EXPECT_EQ(a.load(), 0);

    a.store(42);
    EXPECT_EQ(a.load(), 42);
}

TEST(AtomicTest, ExchangeInt) {
    auto a = atomic(1);
    const int old = a.exchange(99);
    EXPECT_EQ(old, 1);
    EXPECT_EQ(a.load(), 99);
}

TEST(AtomicTest, StoreAndLoadBool) {
    auto a = atomic(false);
    EXPECT_FALSE(a.load());

    a.store(true);
    EXPECT_TRUE(a.load());
}

// ========== 非平凡类型测试（走锁模式） ==========
TEST(AtomicTest, StoreAndLoadString) {
    auto a = atomic(std::string("hello"));
    EXPECT_EQ(a.load(), "hello");

    a.store(std::string("world"));
    EXPECT_EQ(a.load(), "world");
}

TEST(AtomicTest, ExchangeString) {
    auto a = atomic(std::string("first"));
    const auto old = a.exchange(std::string("second"));
    EXPECT_EQ(old, "first");
    EXPECT_EQ(a.load(), "second");
}

// ========== 多线程并发测试 ==========
TEST(AtomicTest, ConcurrentIncrement) {
    auto counter = atomic(0);

    auto task = [&counter]
    {
        for (int i = 0; i < 1000; ++i) {
            const int old = counter.load();
            counter.store(old + 1);
        }
    };

    std::thread t1(task);
    std::thread t2(task);
    t1.join();
    t2.join();

    // 注意：这里用 store+load 而不是 CAS，不保证无锁正确性
    // 但能测试在锁模式/lock-free模式下至少是数据安全的
    EXPECT_EQ(counter.load(), 2000);
}
