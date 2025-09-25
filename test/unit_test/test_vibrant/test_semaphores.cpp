#include <gtest/gtest.h>
#include "thread.h"
#include <vector>
#include "zeit.h"
#include "semaphores.h"

using namespace original;
using namespace original::literals;

TEST(SemaphoreTest, AcquireReleaseBasic) {
    semaphore<3> sem(2);

    EXPECT_TRUE(sem.tryAcquire());  // count=1
    EXPECT_TRUE(sem.tryAcquire());  // count=0
    EXPECT_FALSE(sem.tryAcquire()); // 没有资源了

    sem.release();
    EXPECT_TRUE(sem.tryAcquire());  // 又能获取了
}

TEST(SemaphoreTest, BlockingAcquire) {
    semaphore sem(0);

    const auto start = time::point::now();
    thread t([&] {
        thread::sleep(100_ms);
        sem.release();
    });
    sem.acquire(); // 应该会阻塞直到 release
    const auto end = time::point::now();

    EXPECT_GE(end - start, 100_ms - 5_ms);
    t.join();
}

TEST(SemaphoreTest, AcquireTimeout) {
    semaphore<1> sem(0);

    const auto start = time::point::now();
    const bool ok = sem.acquireFor(200_ms);
    const auto end = time::point::now();

    EXPECT_FALSE(ok); // 超时失败
    EXPECT_GE(end - start, 200_ms);
}

TEST(SemaphoreTest, ReleaseBlocksWhenFull) {
    semaphore<2> sem(2);

    thread t([&] {
        thread::sleep(200_ms);
        sem.acquire(); // 消费一个
    });

    const auto start = time::point::now();
    sem.release(); // 应该会阻塞，直到 acquire 消费一个
    const auto end = time::point::now();

    EXPECT_GE(end - start, 200_ms);
    t.join();
}

TEST(SemaphoreTest, ReleaseTimeout) {
    semaphore<2> sem(2);

    const bool ok = sem.releaseFor(1, 200_ms);
    EXPECT_FALSE(ok); // 已经满了，超时失败
}

TEST(SemaphoreTest, UnboundedSemaphore) {
    semaphore<0> sem(0);

    EXPECT_FALSE(sem.tryAcquire());
    sem.release(5);

    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(sem.tryAcquire());
    }
    EXPECT_FALSE(sem.tryAcquire());
}

TEST(SemaphoreGuardTest, RAII) {
    semaphore<2> sem(1);
    {
        semaphoreGuard<2> guard(sem);
        EXPECT_FALSE(sem.tryAcquire()); // 已经被 guard 拿走了
    }
    EXPECT_TRUE(sem.tryAcquire()); // guard 析构时释放
}
