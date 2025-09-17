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

// 测试立即任务在没有空闲线程时提交会抛出异常
TEST(TaskDelegatorTest, SubmitImmediateWithoutIdleThreadThrows) {
    taskDelegator delegator(1);

    // 提交一个长时间运行的任务，使线程繁忙
    auto long_task = delegator.submit([]{
        thread::sleep(seconds(2));
        return 42;
    });

    // 尝试提交立即任务，应抛出异常
    EXPECT_THROW({
        delegator.submit(taskDelegator::priority::IMMEDIATE, []{ return 0; });
    }, sysError);
}

// 测试未知优先级抛出异常
TEST(TaskDelegatorTest, SubmitWithUnknownPriorityThrows) {
    taskDelegator delegator(2);

    // 使用无效的优先级值
    auto invalid_priority = static_cast<taskDelegator::priority>(999);

    EXPECT_THROW({
        delegator.submit(invalid_priority, []{ return 0; });
    }, sysError);
}

// 测试析构函数自动停止
TEST(TaskDelegatorTest, DestructorAutoStops) {
    // 在内部作用域创建delegator，验证析构时自动停止
    {
        taskDelegator delegator(2);
        auto f = delegator.submit([]{ return 1; });
        EXPECT_EQ(f.result(), 1);
        // 不手动调用stop，让析构函数处理
    }
    // 如果析构函数正常工作，此处不应有问题
    SUCCEED();
}

// 测试活动线程和空闲线程计数
TEST(TaskDelegatorTest, ActiveAndIdleThreadCounts) {
    taskDelegator delegator(2);

    // 初始状态，所有线程空闲
    EXPECT_EQ(delegator.activeThreads(), 0);

    // 提交一个任务，应有一个活动线程
    auto f1 = delegator.submit([]{
        thread::sleep(milliseconds(100));
        return 1;
    });

    // 短暂等待让任务开始执行
    thread::sleep(milliseconds(10));

    EXPECT_EQ(delegator.activeThreads(), 1);
    EXPECT_EQ(delegator.idleThreads(), 1);

    // 提交第二个任务
    auto f2 = delegator.submit([]{
        thread::sleep(milliseconds(100));
        return 2;
    });

    thread::sleep(milliseconds(10));

    EXPECT_EQ(delegator.activeThreads(), 2);
    EXPECT_EQ(delegator.idleThreads(), 0);

    // 等待任务完成
    f1.result();
    f2.result();

    delegator.stop();

    // 任务完成后，线程应回到空闲状态
    EXPECT_EQ(delegator.activeThreads(), 0);
    EXPECT_EQ(delegator.idleThreads(), 0);
}

// 测试逐个运行延迟任务
TEST(TaskDelegatorTest, RunDeferredOneByOne) {
    taskDelegator delegator(2);

    std::atomic<int> counter{0};

    // 提交多个延迟任务
    for (int i = 0; i < 3; ++i) {
        delegator.submit(taskDelegator::priority::DEFERRED, [&counter]{
            ++counter;
            return counter.load();
        });
    }

    // 初始时延迟任务不应执行
    EXPECT_EQ(counter.load(), 0);

    // 逐个运行延迟任务
    for (int i = 0; i < 3; ++i) {
        delegator.runDeferred();
        // 等待任务执行
        thread::sleep(milliseconds(50));
    }

    // 所有延迟任务应已执行
    EXPECT_EQ(counter.load(), 3);
}

// 测试task的移动语义
TEST(TaskTest, MoveSemantics) {
    // 移动构造
    auto task1 = original::task<int>([]{ return 1; });
    auto task2 = std::move(task1);

    // 移动赋值
    task<int> task3 = std::move(task2);

    // 验证移动后的task能正常工作
    taskDelegator delegator(1);
    auto t = makeStrongPtr<task<int>>(std::move(task3));
    auto f = delegator.submit(t);

    EXPECT_EQ(f.result(), 1);
}

// 测试空任务提交
TEST(TaskDelegatorTest, SubmitEmptyTask) {
    taskDelegator delegator(1);

    // 提交不执行任何操作的任务
    auto f = delegator.submit([]{});

    // 应正常完成而不抛出异常
    EXPECT_NO_THROW(f.result());
}

// 测试异常传播
TEST(TaskDelegatorTest, ExceptionPropagation) {
    taskDelegator delegator(1);

    auto f = delegator.submit([]{
        throw std::runtime_error("Test exception");
    });

    EXPECT_THROW(f.result(), std::runtime_error);
}

// 测试停止后无法提交任何优先级任务
TEST(TaskDelegatorTest, StopPreventsAllPrioritySubmits) {
    taskDelegator delegator(1);
    delegator.stop();

    // 所有优先级都应抛出异常
    EXPECT_THROW({
        delegator.submit(taskDelegator::priority::IMMEDIATE, []{});
    }, sysError);

    EXPECT_THROW({
        delegator.submit(taskDelegator::priority::HIGH, []{});
    }, sysError);

    EXPECT_THROW({
        delegator.submit(taskDelegator::priority::NORMAL, []{});
    }, sysError);

    EXPECT_THROW({
        delegator.submit(taskDelegator::priority::LOW, []{});
    }, sysError);

    EXPECT_THROW({
        delegator.submit(taskDelegator::priority::DEFERRED, []{});
    }, sysError);
}

TEST(TaskDelegatorTest, StressTestMixedTasks) {
    constexpr int thread_count = 8;
    constexpr int normal_tasks = 50;
    constexpr int high_tasks = 30;
    constexpr int low_tasks = 15;
    constexpr int deferred_tasks = 25;

    bool immediate_task_submitted = false;

    taskDelegator delegator(thread_count);

    std::atomic normal_sum{0};
    std::atomic high_sum{0};
    std::atomic low_sum{0};
    std::atomic deferred_sum{0};
    std::atomic immediate_sum{0};

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
        thread::sleep(milliseconds(10));
        high_sum += val;
        return val;
    };

    auto deferred_func = [&deferred_sum](const int val) {
        thread::sleep(milliseconds(10));
        deferred_sum += val;
        return val;
    };

    auto immediate_func = [&immediate_sum](const int val) {
        thread::sleep(milliseconds(10));
        immediate_sum += val;
        return val;
    };

    std::vector<async::future<int>> futures;

    // 提交 IMMEDIATE 任务
    if (delegator.idleThreads() > 0)
    {
        constexpr int immediate_task = 1;
        futures.push_back(delegator.submit(taskDelegator::priority::IMMEDIATE, immediate_func, immediate_task));
        immediate_task_submitted = true;
    }

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

    // 提交 DEFERRED 任务
    for (int j = 1; j <= deferred_tasks; ++j) {
        futures.push_back(delegator.submit(taskDelegator::priority::DEFERRED, deferred_func, j));
    }

    delegator.runAllDeferred();

    for (auto &fut : futures) {
        fut.result();
    }

    // 验证计数和求和结果
    constexpr int expected_normal_sum = normal_tasks * (normal_tasks + 1) / 2;
    constexpr int expected_high_sum = high_tasks * (high_tasks + 1) / 2;
    constexpr int expected_low_sum = low_tasks * (low_tasks + 1) / 2;
    constexpr int expected_deferred_sum = deferred_tasks * (deferred_tasks + 1) / 2;
    constexpr int expected_immediate_sum = 1;

    EXPECT_EQ(normal_sum.load(), expected_normal_sum);
    EXPECT_EQ(high_sum.load(), expected_high_sum);
    EXPECT_EQ(low_sum.load(), expected_low_sum);
    EXPECT_EQ(deferred_sum.load(), expected_deferred_sum);
    EXPECT_EQ(immediate_sum.load(), immediate_task_submitted ? expected_immediate_sum : 0);
}
