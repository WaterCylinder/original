#include <gtest/gtest.h>
#include <utility>
#include "async.h"
#include "zeit.h"

using namespace original;

class runTimeTestError final : std::exception {
    mutable std::string msg_{"runTimeTestError"};

public:
    runTimeTestError() = default;

    explicit runTimeTestError(std::string msg) : msg_(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return this->msg_.c_str();
    }
};

// 辅助函数：运行 promise 在单独线程中
template<typename Promise>
void runPromiseInThread(Promise& p) {
    thread t([p = std::move(p)]() mutable {
        p.run();
    });
}

TEST(AsyncTest, SimpleAsyncReturnsValue) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    auto f = p.getFuture();
    runPromiseInThread(p);
    EXPECT_EQ(f.result(), 42);
}

TEST(AsyncTest, MultipleWaitsAreSafe) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(120));
        return 99;
    });

    auto f = p.getFuture();
    runPromiseInThread(p);

    std::cout << f.result() << std::endl;  // 第一次应该成功

    // 再次调用应该抛出异常
    EXPECT_THROW(f.result(), original::error);
}

TEST(AsyncTest, AsynchronousExecution) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(200));
        return 2025;
    });

    auto start = time::point::now();
    auto f = p.getFuture();
    runPromiseInThread(p);

    int result = f.result();
    auto end = time::point::now();

    auto duration = end - start;
    EXPECT_GE(duration.value(), 190);  // 至少等待了200ms（允许误差）
    EXPECT_EQ(result, 2025);
}

TEST(AsyncTest, ExceptionHandledProperly) {
    auto p = async::makePromise([]() -> int {
        throw runTimeTestError("failure in thread");
    });

    auto f = p.getFuture();
    runPromiseInThread(p);
    EXPECT_THROW(f.result(), runTimeTestError);
}

// 测试 void 返回类型的异步任务
TEST(AsyncTest, VoidReturnType) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(50));
        // do nothing, just return
    });

    auto f = p.getFuture();
    runPromiseInThread(p);
    EXPECT_NO_THROW(f.result());  // 应该能正常返回（void）
}

// 测试传参异步任务
TEST(AsyncTest, TaskWithArguments) {
    auto p = async::makePromise([](int a, int b) {
        return a + b;
    }, 10, 32);

    auto f = p.getFuture();
    runPromiseInThread(p);
    EXPECT_EQ(f.result(), 42);
}

// 测试连续创建多个 future，不互相干扰
TEST(AsyncTest, MultipleFuturesIndependence) {
    auto p1 = async::makePromise([] { return 1; });
    auto p2 = async::makePromise([] { return 2; });

    auto f1 = p1.getFuture();
    auto f2 = p2.getFuture();

    runPromiseInThread(p1);
    runPromiseInThread(p2);

    EXPECT_EQ(f1.result(), 1);
    EXPECT_EQ(f2.result(), 2);
}

// 测试异常任务在多次调用后仍能抛出原始异常
TEST(AsyncTest, ExceptionConsistency) {
    auto p = async::makePromise([]() -> int {
        throw runTimeTestError("boom");
    });

    auto f = p.getFuture();
    runPromiseInThread(p);

    EXPECT_THROW(f.result(), runTimeTestError);
    // 第二次调用仍然抛异常（根据你的实现）
    EXPECT_THROW(f.result(), runTimeTestError);
}

// 测试 get() 函数（便捷函数）
TEST(AsyncTest, GetFunctionWorks) {
    auto result = async::get([] {
        thread::sleep(milliseconds(50));
        return 123;
    });
    EXPECT_EQ(result, 123);
}

// 测试带参数的 get() 函数
TEST(AsyncTest, GetFunctionWithArgs) {
    auto result = async::get([](int a, int b) {
        return a + b;
    }, 20, 22);
    EXPECT_EQ(result, 42);
}

// 压力测试：启动多个任务，确保不会死锁
TEST(AsyncTest, StressTestMultipleTasks) {
    std::vector<async::future<int>> futures;

    for (int i = 0; i < 20; i++) {
        // 直接使用 async::get 简化测试
        auto result = async::get([](int x) {
            thread::sleep(milliseconds(10));
            return x * x;
        }, i);
        EXPECT_EQ(result, i * i);
    }
}

// 测试 ready() 方法
TEST(AsyncTest, ReadyMethodWorks) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    auto f = p.getFuture();
    EXPECT_FALSE(f.ready());  // 任务还没开始，应该没准备好

    runPromiseInThread(p);
    thread::sleep(milliseconds(150));  // 等待任务完成

    EXPECT_TRUE(f.ready());  // 现在应该准备好了
    EXPECT_EQ(f.result(), 42);
}

// 测试 wait() 方法
TEST(AsyncTest, WaitMethodWorks) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    auto f = p.getFuture();
    auto start = time::point::now();

    runPromiseInThread(p);
    f.wait();  // 等待任务完成

    const auto end = time::point::now();
    const auto duration = end - start;

    EXPECT_GE(duration.value(), 90);  // 应该等待了至少100ms
    EXPECT_TRUE(f.ready());
    EXPECT_EQ(f.result(), 42);
}
