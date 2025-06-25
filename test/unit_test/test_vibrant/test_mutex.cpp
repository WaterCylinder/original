#include <gtest/gtest.h>
#include <thread>
#include "mutex.h"
#include "thread.h"

using namespace original;

TEST(MutexTest, LockUnlockDoesNotThrow) {
    pMutex m;
    EXPECT_NO_THROW(m.lock());
    EXPECT_NO_THROW(m.unlock());
}

TEST(MutexTest, TryLockSuccess) {
    pMutex m;
    EXPECT_NO_THROW(m.tryLock());
    EXPECT_NO_THROW(m.unlock());
}

TEST(MutexTest, TryLockContested) {
    pMutex m;
    std::thread t(
        [&m]
        {
            m.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            m.unlock();
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 确保 t 抢先上锁

    EXPECT_FALSE(m.tryLock());  // 此时 tryLock 应该失败
    t.join();
}

TEST(MutexTest, PreventsDataRaceWithMultipleThreads) {
    constexpr int thread_count = 10;
    constexpr int iterations = 10000;
    int counter = 0;
    pMutex m;

    auto increment = [&]
    {
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
        pMutex pm;
        const uniqueLock m(pm);  // 默认锁定
        EXPECT_TRUE(m.isLocked());
    }
    SUCCEED();
}

TEST(MutexTest, TryLockConstructor) {
    {
        pMutex pm;
        const uniqueLock m(pm, uniqueLock::TRY_LOCK);  // 使用 tryLock
        EXPECT_TRUE(m.isLocked());
    }
    SUCCEED();
}

TEST(MutexTest, MutexIsNonCopyable) {
    EXPECT_FALSE(std::is_copy_constructible_v<pMutex>);
    EXPECT_FALSE(std::is_copy_assignable_v<pMutex>);
    EXPECT_FALSE(std::is_copy_constructible_v<uniqueLock>);
    EXPECT_FALSE(std::is_copy_assignable_v<uniqueLock>);
}

TEST(MutexTest, MutexIsNonMovable) {
    EXPECT_FALSE(std::is_move_constructible_v<pMutex>);
    EXPECT_FALSE(std::is_move_assignable_v<pMutex>);
    EXPECT_FALSE(std::is_move_constructible_v<uniqueLock>);
    EXPECT_FALSE(std::is_move_assignable_v<uniqueLock>);
}

// TryLock失败时，uniqueLock::isLocked() 应为 false
TEST(MutexTest, TryLockFailsIsLockedFalse) {
    pMutex pm;
    pm.lock();  // 主线程先锁住

    const uniqueLock s(pm, uniqueLock::TRY_LOCK);  // 尝试获取失败
    EXPECT_FALSE(s.isLocked());

    pm.unlock();  // 主线程释放锁
}

// scopeLock析构后，mutex可以再次上锁
TEST(MutexTest, RAIIUnlocksCorrectly) {
    pMutex pm;
    {
        uniqueLock s(pm);
        EXPECT_TRUE(s.isLocked());
    }

    // 如果前面未释放成功，这里会阻塞或抛异常
    EXPECT_NO_THROW(pm.lock());
    pm.unlock();
}

// 多线程验证scopeLock作用域内有效
TEST(MutexTest, ScopeLockProtectsCriticalSection) {
    constexpr int thread_count = 10;
    constexpr int iterations = 10000;
    int counter = 0;
    pMutex m;

    auto increment = [&]
    {
        for (int i = 0; i < iterations; ++i) {
            uniqueLock lock(m);
            ++counter;
        }
    };

    std::vector<thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(increment);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(counter, thread_count * iterations);
}

// TryLock获取失败时不会 unlock
TEST(MutexTest, TryLockFailDoesNotUnlock) {
    pMutex pm;
    pm.lock();

    {
        const uniqueLock s(pm, uniqueLock::TRY_LOCK);
        EXPECT_FALSE(s.isLocked());
    }

    // pm 依旧被主线程持有，解锁后测试结束
    EXPECT_NO_THROW(pm.unlock());
}

// TryLock获取成功后，锁应被释放
TEST(MutexTest, TryLockSuccessUnlocksOnDestruction) {
    pMutex pm;
    {
        if (const uniqueLock s(pm, uniqueLock::TRY_LOCK); s.isLocked()) {
            SUCCEED();  // 成功加锁
        }
    }

    // 若未正确释放，此处将阻塞
    EXPECT_NO_THROW(pm.lock());
    pm.unlock();
}
