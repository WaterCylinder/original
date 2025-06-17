#include <gtest/gtest.h>
#include <thread>
#include "mutex.h"

using namespace original;

TEST(pMutexTest, LockUnlockDoesNotThrow) {
    pMutex m;
    EXPECT_NO_THROW(m.lock());
    EXPECT_NO_THROW(m.unlock());
}

TEST(pMutexTest, TryLockSuccess) {
    pMutex m;
    EXPECT_NO_THROW(m.tryLock());
    EXPECT_NO_THROW(m.unlock());
}

TEST(pMutexTest, TryLockContested) {
    pMutex m;
    std::thread t(
        [&m]() {
            m.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            m.unlock();
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 确保 t 抢先上锁

    EXPECT_THROW(m.tryLock(), sysError);  // 此时 tryLock 应该失败
    t.join();
}

TEST(pMutexTest, PreventsDataRaceWithMultipleThreads) {
    constexpr int thread_count = 10;
    constexpr int iterations = 10000;
    int counter = 0;
    pMutex m;

    auto increment = [&]() {
        for (int i = 0; i < iterations; ++i) {
            m.lock();
            ++counter;
            m.unlock();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(increment);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter, thread_count * iterations);
}

TEST(MutexTest, LockInConstructorAndUnlockInDestructor) {
    {
        mutex m;  // 默认锁定
        // 这里无法直接验证锁状态，但至少应无异常
    }
    SUCCEED();
}

TEST(MutexTest, TryLockConstructor) {
    {
        mutex m(true);  // 使用 tryLock
        // tryLock 失败会抛出异常
    }
    SUCCEED();
}

TEST(MutexTest, MutexIsNonCopyable) {
    EXPECT_FALSE(std::is_copy_constructible<mutex>::value);
    EXPECT_FALSE(std::is_copy_assignable<mutex>::value);
}

TEST(MutexTest, MutexIsNonMovable) {
    EXPECT_FALSE(std::is_move_constructible<mutex>::value);
    EXPECT_FALSE(std::is_move_assignable<mutex>::value);
}

