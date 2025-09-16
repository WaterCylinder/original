#include <gtest/gtest.h>
#include "tasks.h"
#include "thread.h"

using namespace original;

// 基础测试函数
int add_func(const int a, const int b) {
    thread::sleep(seconds(1));
    return a + b;
}

int sub_func(const int a, const int b) {
    thread::sleep(seconds(1));
    return a - b;
}

TEST(TaskDelegatorTest, SubmitNormalTasks) {
    taskDelegator delegator(4);

    auto f1 = delegator.submit(add_func, 2, 3);
    auto f2 = delegator.submit(sub_func, 10, 4);

    EXPECT_EQ(f1.result(), 5);
    EXPECT_EQ(f2.result(), 6);
}

TEST(TaskDelegatorTest, SubmitHighPriority) {
    taskDelegator delegator(2);

    auto f1 = delegator.submit(taskDelegator::priority::NORMAL, add_func, 1, 1);
    auto f2 = delegator.submit(taskDelegator::priority::HIGH, add_func, 2, 2);

    EXPECT_EQ(f1.result(), 2);
    EXPECT_EQ(f2.result(), 4);
}

TEST(TaskDelegatorTest, DeferredRunManually) {
    taskDelegator delegator(2);

    // 提交延迟任务
    auto f = delegator.submit(taskDelegator::priority::DEFERRED, add_func, 7, 8);

    // 还没触发，future 应该还没 ready
    EXPECT_FALSE(f.ready());

    // 手动触发一次
    delegator.runDeferred();

    EXPECT_EQ(f.result(), 15);
}

TEST(TaskDelegatorTest, RunAllDeferred) {
    taskDelegator delegator(2);

    auto f1 = delegator.submit(taskDelegator::priority::DEFERRED, add_func, 1, 1);
    auto f2 = delegator.submit(taskDelegator::priority::DEFERRED, add_func, 2, 2);

    delegator.runAllDeferred();

    EXPECT_EQ(f1.result(), 2);
    EXPECT_EQ(f2.result(), 4);
}

TEST(TaskDelegatorTest, StopPreventsNewSubmits) {
    taskDelegator delegator(2);

    auto f1 = delegator.submit(add_func, 3, 4);
    EXPECT_EQ(f1.result(), 7);

    delegator.stop();

    EXPECT_THROW({
        delegator.submit(add_func, 5, 6);
    }, sysError);
}

TEST(TaskDelegatorTest, StressTestMixedTasks) {
    constexpr int thread_count = 8;
    constexpr int normal_tasks = 50;
    constexpr int high_tasks = 20;
    constexpr int deferred_tasks = 30;

    taskDelegator delegator(thread_count);

    std::atomic normal_sum{0};
    std::atomic high_sum{0};
    std::atomic deferred_sum{0};

    auto normal_func = [&normal_sum](const int val){
        thread::sleep(milliseconds(10));
        normal_sum += val;
        return val;
    };

    auto high_func = [&high_sum](const int val){
        thread::sleep(milliseconds(5));
        high_sum += val;
        return val;
    };

    auto deferred_func = [&deferred_sum](const int val){
        thread::sleep(milliseconds(15));
        deferred_sum += val;
        return val;
    };

    std::vector<async::future<int>> futures;

    // 提交 NORMAL 任务
    for (int i = 1; i <= normal_tasks; ++i) {
        futures.push_back(delegator.submit(taskDelegator::priority::NORMAL, normal_func, i));
    }

    // 提交 HIGH 任务
    for (int i = 1; i <= high_tasks; ++i) {
        futures.push_back(delegator.submit(taskDelegator::priority::HIGH, high_func, i));
    }

    // 提交 DEFERRED 任务
    for (int i = 1; i <= deferred_tasks; ++i) {
        futures.push_back(delegator.submit(taskDelegator::priority::DEFERRED, deferred_func, i));
    }

    // 手动触发部分 DEFERRED 任务
    for (int i = 0; i < deferred_tasks / 2; ++i) {
        delegator.runDeferred();
    }

    // 触发剩余所有 DEFERRED 任务
    delegator.runAllDeferred();

    // 等待所有任务完成
    for (auto &fut : futures) {
        fut.result();
    }

    // 验证计数和求和结果
    constexpr int expected_normal_sum = normal_tasks * (normal_tasks + 1) / 2;
    constexpr int expected_high_sum = high_tasks * (high_tasks + 1) / 2;
    constexpr int expected_deferred_sum = deferred_tasks * (deferred_tasks + 1) / 2;

    EXPECT_EQ(normal_sum.load(), expected_normal_sum);
    EXPECT_EQ(high_sum.load(), expected_high_sum);
    EXPECT_EQ(deferred_sum.load(), expected_deferred_sum);

    // active_threads_ 最终应该为 0
    EXPECT_EQ(delegator.activeThreads(), 0);
}
