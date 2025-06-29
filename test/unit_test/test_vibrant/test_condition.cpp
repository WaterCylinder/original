#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "mutex.h"
#include "condition.h"
#include "thread.h"

using namespace original;
using namespace std::chrono;

class pConditionTest : public testing::Test {
protected:
    pMutex mutex;
    pCondition cond;
    bool ready = false;
};

// 基本通知机制：等待一个条件，然后 notify 唤醒
TEST_F(pConditionTest, BasicNotifyTest) {
    thread t([&] {
        uniqueLock lock(mutex);
        while (!ready) {
            cond.wait(mutex);
        }
    }, thread::AUTO_JOIN);

    std::this_thread::sleep_for(50ms); // 让线程进入 wait 状态

    {
        uniqueLock lock(mutex);
        ready = true;
        cond.notify();
    }

    t.join();
    SUCCEED();
}

// 测试 waitFor 的超时行为
TEST_F(pConditionTest, TimedWaitTimeoutTest) {
    uniqueLock lock(mutex);
    const bool result = cond.waitFor(mutex, time::duration(200, time::unit::MILLISECOND));
    EXPECT_FALSE(result);  // 应超时
}

// 测试 waitFor 在超时前被唤醒
TEST_F(pConditionTest, TimedWaitNotTimeoutTest) {
    thread t([&] {
        std::this_thread::sleep_for(50ms); // 等待 50ms 再唤醒
        uniqueLock lock(mutex);
        ready = true;
        cond.notify();
    }, thread::AUTO_JOIN);

    bool result;
    {
        uniqueLock lock(mutex);
        result = cond.waitFor(mutex, time::duration(200, time::unit::MILLISECOND));
    }

    EXPECT_TRUE(result);
}

// 测试 notifyAll 能唤醒多个等待线程
TEST_F(pConditionTest, NotifyAllWakesAllWaiters) {
    int wake_count = 0;
    ready = false;

    thread t1([&] {
        uniqueLock lock(mutex);
        while (!ready) cond.wait(mutex);
        wake_count++;
    });

    thread t2([&] {
        uniqueLock lock(mutex);
        while (!ready) cond.wait(mutex);
        wake_count++;
    });

    std::this_thread::sleep_for(50ms); // 等待线程进入阻塞状态

    {
        uniqueLock lock(mutex);
        ready = true;
        cond.notifyAll();
    }

    t1.join();
    t2.join();

    EXPECT_EQ(wake_count, 2);
}

