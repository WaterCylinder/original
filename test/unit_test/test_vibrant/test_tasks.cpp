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
    for (int i = 0; i < 5; ++i) {
        constexpr int thread_count = 8;
        constexpr int normal_tasks = 50;
        constexpr int high_tasks = 30;
        constexpr int low_tasks = 15;

        taskDelegator delegator(thread_count);

        std::atomic normal_sum{0};
        std::atomic high_sum{0};
        std::atomic low_sum{0};

        auto low_func = [&low_sum](const int val){
            thread::sleep(milliseconds(10));
            low_sum += val;
            return val;
        };

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

        std::vector<async::future<int>> futures;

        // 提交 LOW 任务
        for (int j = 1; j <= low_tasks; ++j) {
            futures.push_back(delegator.submit(taskDelegator::priority::LOW, low_func, j));
        }

        // 提交 NORMAL 任务
        for (int j = 1; j <= normal_tasks; ++j) {
            futures.push_back(delegator.submit(taskDelegator::priority::NORMAL, normal_func, j));
        }

        // 提交 HIGH 任务
        for (int j = 1; j <= high_tasks; ++j) {
            futures.push_back(delegator.submit(taskDelegator::priority::HIGH, high_func, j));
        }

        // 等待所有任务完成
        for (auto &fut : futures) {
            fut.wait();
        }

        for (auto &fut : futures) {
            fut.result();
        }

        // 验证计数和求和结果
        constexpr int expected_normal_sum = normal_tasks * (normal_tasks + 1) / 2;
        constexpr int expected_high_sum = high_tasks * (high_tasks + 1) / 2;
        constexpr int expected_low_sum = low_tasks * (low_tasks + 1) / 2;

        EXPECT_EQ(normal_sum.load(), expected_normal_sum);
        EXPECT_EQ(high_sum.load(), expected_high_sum);
        EXPECT_EQ(low_sum.load(), expected_low_sum);
    }
}
