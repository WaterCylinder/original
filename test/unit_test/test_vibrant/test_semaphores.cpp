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

    EXPECT_GE(end - start, 100_ms - 1_ms);
    t.join();
}

TEST(SemaphoreTest, AcquireTimeout) {
    semaphore sem(0);

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
        semaphoreGuard guard(sem);
        EXPECT_FALSE(sem.tryAcquire()); // 已经被 guard 拿走了
    }
    EXPECT_TRUE(sem.tryAcquire()); // guard 析构时释放
}

TEST(SemaphoreStressTest, ProducerConsumerBounded) {
    constexpr int PRODUCERS = 4;
    constexpr int CONSUMERS = 4;
    constexpr int ITEMS_PER_PRODUCER = 200;
    constexpr int BUFFER_SIZE = 10;

    semaphore<BUFFER_SIZE> empty_slots; // 控制空位
    semaphore<BUFFER_SIZE> full_slots(0);            // 控制已有元素
    std::mutex buffer_mutex;
    std::vector<int> buffer;

    std::atomic produced{0};
    std::atomic consumed{0};

    auto producer = [&](const int id) {
        for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
            // 等待空位
            if (i % 3 == 0) {
                empty_slots.acquire();
            } else if (i % 3 == 1) {
                ASSERT_TRUE(empty_slots.acquireFor(100_ms));
            } else {
                ASSERT_TRUE(empty_slots.tryAcquire() || empty_slots.acquireFor(50_ms));
            }

            {
                std::lock_guard lock(buffer_mutex);
                buffer.push_back(id * 1000 + i); // 模拟数据
            }

            ++produced;
            // 通知有元素
            if (i % 2 == 0) {
                full_slots.release();
            } else {
                ASSERT_TRUE(full_slots.tryRelease() || full_slots.releaseFor(50_ms));
            }
        }
    };

    auto consumer = [&] {
        while (consumed < PRODUCERS * ITEMS_PER_PRODUCER) {
            bool ok = false;
            if (consumed % 3 == 0) {
                ok = full_slots.acquireFor(100_ms);
            } else {
                ok = full_slots.tryAcquire() || full_slots.acquireFor(50_ms);
            }
            if (!ok) continue; // 超时没拿到，继续尝试

            int value = -1;
            {
                std::lock_guard lock(buffer_mutex);
                if (!buffer.empty()) {
                    value = buffer.back();
                    buffer.pop_back();
                }
            }
            if (value != -1) {
                ++consumed;
                empty_slots.release();
            }
        }
    };

    std::vector<thread> threads;
    for (int i = 0; i < PRODUCERS; i++) {
        threads.emplace_back(producer, i);
    }
    for (int i = 0; i < CONSUMERS; i++) {
        threads.emplace_back(consumer);
    }

    for (auto& t : threads) t.join();

    EXPECT_EQ(produced, PRODUCERS * ITEMS_PER_PRODUCER);
    EXPECT_EQ(consumed, PRODUCERS * ITEMS_PER_PRODUCER);
}

TEST(SemaphoreStressTest, ProducerConsumerUnbounded) {
    constexpr int PRODUCERS = 2;
    constexpr int CONSUMERS = 2;
    constexpr int ITEMS_PER_PRODUCER = 500;

    semaphore<0> full_slots(0); // 无界，只用来控制元素数量
    std::mutex buffer_mutex;
    std::vector<int> buffer;

    std::atomic produced{0};
    std::atomic consumed{0};

    auto producer = [&](const int id) {
        for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
            {
                std::lock_guard lock(buffer_mutex);
                buffer.push_back(id * 1000 + i);
            }
            ++produced;
            full_slots.release();
        }
    };

    auto consumer = [&] {
        while (consumed < PRODUCERS * ITEMS_PER_PRODUCER) {
            if (!full_slots.acquireFor(100_ms)) continue;

            int value = -1;
            {
                std::lock_guard lock(buffer_mutex);
                if (!buffer.empty()) {
                    value = buffer.back();
                    buffer.pop_back();
                }
            }
            if (value != -1) {
                ++consumed;
            }
        }
    };

    std::vector<thread> threads;
    for (int i = 0; i < PRODUCERS; i++) {
        threads.emplace_back(producer, i);
    }
    for (int i = 0; i < CONSUMERS; i++) {
        threads.emplace_back(consumer);
    }

    for (auto& t : threads) t.join();

    EXPECT_EQ(produced, PRODUCERS * ITEMS_PER_PRODUCER);
    EXPECT_EQ(consumed, PRODUCERS * ITEMS_PER_PRODUCER);
}