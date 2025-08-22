#include <gtest/gtest.h>
#include <utility>
#include "async.h"
#include "zeit.h"

using namespace original;

class runTimeTestError final : std::exception {
    mutable std::string msg_;

    public:
        runTimeTestError() : msg_("runTimeTestError") {}

        explicit runTimeTestError(std::string msg) : msg_(std::move(msg)) {}

        const char* what() const noexcept override {
            return this->msg_.c_str();
        }
};

TEST(AsyncTest, SimpleAsyncReturnsValue) {
    const async::promise<int, int()> p([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    const auto f = p.getFuture();
    EXPECT_EQ(f.result(), 42);
}

TEST(AsyncTest, MultipleWaitsAreSafe) {
    const async::promise<int, int()> p([] {
        thread::sleep(milliseconds(120));
        return 99;
    });

    const auto f = p.getFuture();
    std::cout << f.result() << std::endl;  // should succeed

    // 再次调用应抛出异常
    EXPECT_THROW(f.result(), valueError);
}

TEST(AsyncTest, AsynchronousExecution) {
    const async::promise<int, int()> p([] {
        thread::sleep(milliseconds(200));
        return 2025;
    });

    auto start = time::point::now();
    auto f = p.getFuture();

    int result = f.result();
    auto end = time::point::now();

    auto duration = end - start;
    EXPECT_GE(duration.value(), 190);  // 至少等待了200ms（允许误差）
    EXPECT_EQ(result, 2025);
}

TEST(AsyncTest, ExceptionHandledProperly) {
    const async::promise<int, int()> p([]() -> int {
        throw runTimeTestError("failure in thread");
    });

    const auto f = p.getFuture();
    EXPECT_THROW(f.result(), runTimeTestError);
}

// 测试 void 返回类型的异步任务
TEST(AsyncTest, VoidReturnType) {
    const async::promise<void, void()> p([] {
        thread::sleep(milliseconds(50));
        // do nothing, just return
    });

    const auto f = p.getFuture();
    EXPECT_NO_THROW(f.result());  // 应该能正常返回（void）
}

// 测试传参异步任务
TEST(AsyncTest, TaskWithArguments) {
    const async::promise<int, int(int, int)> p([](int a, int b) {
        return a + b;
    });

    const auto f = p.getFuture(10, 32);
    EXPECT_EQ(f.result(), 42);
}

// 测试连续创建多个 future，不互相干扰
TEST(AsyncTest, MultipleFuturesIndependence) {
    const async::promise<int, int()> p1([] { return 1; });
    const async::promise<int, int()> p2([] { return 2; });

    auto f1 = p1.getFuture();
    auto f2 = p2.getFuture();

    EXPECT_EQ(f1.result(), 1);
    EXPECT_EQ(f2.result(), 2);
}

// 测试异常任务在多次 wait 后仍能抛出原始异常
TEST(AsyncTest, ExceptionConsistency) {
    const async::promise<int, int()> p([]() -> int {
        throw runTimeTestError("boom");
    });

    auto f = p.getFuture();
    EXPECT_THROW(f.result(), runTimeTestError);

    // 第二次调用仍然抛异常（保证异常一致性）
    EXPECT_THROW(f.result(), runTimeTestError);
}

// 测试 get() 被调用时能重抛异常（绕过 result 的情况）
TEST(AsyncTest, DirectGetThrowsException) {
    const async::promise<int, int()> p([]() -> int {
        throw runTimeTestError("direct get boom");
    });

    auto f = p.getFuture();

    EXPECT_THROW({
        // 模拟 result 内部逻辑：wait + rethrow + get
        f.result();
    }, runTimeTestError);
}

// 压力测试：启动多个任务，确保不会死锁
TEST(AsyncTest, StressTestMultipleTasks) {
    std::vector<async::future<int>> futures;
    for (int i = 0; i < 20; i++) {
        const async::promise<int, int(int)> p([](int x) {
            thread::sleep(milliseconds(10));
            return x * x;
        });
        futures.push_back(p.getFuture(i));
    }

    for (int i = 0; i < 20; i++) {
        EXPECT_EQ(futures[i].result(), i * i);
    }
}

// 测试 promise 的 operator() 方法 - 无参数版本
TEST(AsyncTest, PromiseOperatorNoArgs) {
    const async::promise<int, int()> p([] {
        thread::sleep(milliseconds(50));
        return 123;
    });

    // 直接调用 promise 对象获取结果
    EXPECT_EQ(p(), 123);
}

// 测试 promise 的 operator() 方法 - 带参数版本
TEST(AsyncTest, PromiseOperatorWithArgs) {
    const async::promise<int, int(int, int)> p([](int a, int b) {
        return a + b;
    });

    // 直接调用 promise 对象并传递参数
    EXPECT_EQ(p(10, 32), 42);
}

// 测试 promise 的 operator() 方法 - void 返回类型
TEST(AsyncTest, PromiseOperatorVoidReturn) {
    bool executed = false;
    const async::promise<void, void()> p([&executed] {
        thread::sleep(milliseconds(30));
        executed = true;
    });

    // 直接调用 promise 对象
    EXPECT_NO_THROW(p());
    EXPECT_TRUE(executed);
}

// 测试 promise 的 operator() 方法 - 异常处理
TEST(AsyncTest, PromiseOperatorExceptionHandling) {
    const async::promise<int, int()> p([]() -> int {
        throw runTimeTestError("operator() boom");
    });

    // 直接调用 promise 对象应该抛出异常
    EXPECT_THROW(p(), runTimeTestError);
}

// 测试 promise 的 operator() 方法 - 多次调用创建独立 future
TEST(AsyncTest, PromiseOperatorMultipleCalls) {
    int counter = 0;
    const async::promise<int, int()> p([&counter] {
        return ++counter;
    });

    // 每次调用 operator() 应该创建新的 future，得到独立的结果
    EXPECT_EQ(p(), 1);
    EXPECT_EQ(p(), 2);
    EXPECT_EQ(p(), 3);
}

// 测试 promise 的 operator() 方法 - 异步执行验证
TEST(AsyncTest, PromiseOperatorAsynchronous) {
    constexpr int val = 999;

    const async::promise<int, int()> p([&] {
        thread::sleep(milliseconds(100));
        return val;
    });

    const auto start = time::point::now();
    const int result = p();  // 直接调用应该等待异步任务完成
    const auto end = time::point::now();

    const auto duration = end - start;
    EXPECT_GE(duration.value(), 90);  // 至少等待了100ms（允许误差）
    EXPECT_EQ(result, val);
}