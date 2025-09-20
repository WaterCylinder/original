#include <unordered_set>
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

// 测试 sharedFuture 的基本功能（复制和多次访问）
TEST(AsyncTest, SharedFutureBasicFunctionality) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    auto f = p.getFuture();
    auto sf = f.share();  // 转换为 sharedFuture

    runPromiseInThread(p);

    // 多个 sharedFuture 副本应该都能访问结果
    auto sf2 = sf;
    EXPECT_EQ(sf.result(), 42);
    EXPECT_EQ(sf2.result(), 42);

    // 原始 future 应该失效
    EXPECT_FALSE(f.valid());
}

// 测试 void 类型的 sharedFuture
TEST(AsyncTest, SharedFutureVoidType) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(50));
    });

    auto f = p.getFuture();
    auto sf = f.share();

    runPromiseInThread(p);

    // 应该能正常调用（无返回值）
    EXPECT_NO_THROW(sf.result());

    // 可以复制
    auto sf2 = sf;
    EXPECT_NO_THROW(sf2.result());
}

// 测试 sharedFuture 的异常处理
TEST(AsyncTest, SharedFutureExceptionHandling) {
    auto p = async::makePromise([]() -> int {
        throw runTimeTestError("shared future error");
    });

    auto f = p.getFuture();
    auto sf = f.share();

    runPromiseInThread(p);

    // 应该抛出相同的异常
    EXPECT_THROW(sf.result(), runTimeTestError);

    // 副本也应该抛出相同的异常
    auto sf2 = sf;
    EXPECT_THROW(sf2.result(), runTimeTestError);
}

// 测试多线程同时访问 sharedFuture
TEST(AsyncTest, SharedFutureMultithreadedAccess) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(200));
        return 100;
    });

    auto f = p.getFuture();
    auto sf = f.share();

    runPromiseInThread(p);

    std::vector<thread> threads;
    std::atomic successCount{0};

    // 启动多个线程同时访问 sharedFuture
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([&sf, &successCount] {
            try {
                EXPECT_EQ(sf.result(), 100);
                ++successCount;
            } catch (...) {
                // 不应该有异常
                FAIL() << "Unexpected exception in worker thread";
            }
        });
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(successCount.load(), 5);
}

// 测试 futureBase 多态接口
TEST(AsyncTest, FutureBasePolymorphicInterface) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(50));
        return 42;
    });

    auto f = p.getFuture();

    // 通过基类指针操作
    async::futureBase* basePtr = &f;

    runPromiseInThread(p);

    EXPECT_TRUE(basePtr->valid());
    EXPECT_NO_THROW(basePtr->wait());
    EXPECT_TRUE(basePtr->ready());

    // 异常应该为nullptr（无异常）
    EXPECT_EQ(basePtr->exception(), nullptr);
}

// 测试 sharedFuture 通过 futureBase 接口操作
TEST(AsyncTest, SharedFutureBaseInterface) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(50));
        return 42;
    });

    auto f = p.getFuture();
    auto sf = f.share();

    // 通过基类指针操作 sharedFuture
    async::futureBase* basePtr = &sf;

    runPromiseInThread(p);

    EXPECT_TRUE(basePtr->valid());
    EXPECT_NO_THROW(basePtr->wait());
    EXPECT_TRUE(basePtr->ready());
    EXPECT_EQ(basePtr->exception(), nullptr);
}

// 测试异常情况下的 futureBase 接口
TEST(AsyncTest, FutureBaseExceptionCase) {
    auto p = async::makePromise([]() -> int {
        throw runTimeTestError("base interface error");
    });

    auto f = p.getFuture();
    async::futureBase* basePtr = &f;

    runPromiseInThread(p);

    EXPECT_TRUE(basePtr->valid());

    // 应该能检测到异常
    auto exceptionPtr = basePtr->exception();
    EXPECT_NE(exceptionPtr, nullptr);

    // 通过异常指针重新抛出
    EXPECT_THROW(std::rethrow_exception(exceptionPtr), runTimeTestError);
}

// 测试无效的 future 通过 base 接口操作
TEST(AsyncTest, InvalidFutureBaseAccess) {
    async::future<int> f;  // 默认构造，无效的future

    async::futureBase* basePtr = &f;

    EXPECT_FALSE(basePtr->valid());
    EXPECT_THROW(basePtr->wait(), original::error);

    // 无效的future应该返回空指针
    auto exception_ptr = basePtr->exception();
    EXPECT_EQ(exception_ptr, nullptr);

    strongPtr<async::sharedFuture<void>> f2 = makeStrongPtr<async::sharedFuture<void>>();

    EXPECT_FALSE(f2->valid());
    EXPECT_THROW(f2->wait(), original::error);

    auto exception_ptr2 = f2->exception();
    EXPECT_EQ(exception_ptr2, nullptr);

    auto f3 = f2.dynamicCastTo<async::futureBase>();
    EXPECT_FALSE(f3->valid());
    EXPECT_THROW(f3->wait(), original::error);

    auto exception_ptr3 = f3->exception();
    EXPECT_EQ(exception_ptr3, nullptr);
}

// 测试 sharedFuture 的 ready() 方法
TEST(AsyncTest, SharedFutureReadyMethod) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    auto f = p.getFuture();
    auto sf = f.share();

    EXPECT_FALSE(sf.ready());  // 任务还没开始

    runPromiseInThread(p);
    thread::sleep(milliseconds(150));  // 等待任务完成

    EXPECT_TRUE(sf.ready());  // 现在应该准备好了
    EXPECT_EQ(sf.result(), 42);
}

// 测试 sharedFuture 的 wait() 方法
TEST(AsyncTest, SharedFutureWaitMethod) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    auto f = p.getFuture();
    auto sf = f.share();

    auto start = time::point::now();

    runPromiseInThread(p);
    sf.wait();  // 等待任务完成

    auto end = time::point::now();
    auto duration = end - start;

    EXPECT_GE(duration.value(), 90);  // 应该等待了至少100ms
    EXPECT_TRUE(sf.ready());
    EXPECT_EQ(sf.result(), 42);
}

// 测试 sharedFuture 的比较运算符
TEST(AsyncTest, SharedFutureComparisonOperators) {
    auto p1 = async::makePromise([] {
        return 42;
    });
    auto p2 = async::makePromise([] {
        return 100;
    });

    auto f1 = p1.getFuture();
    auto f2 = p2.getFuture();

    auto sf1 = f1.share();
    auto sf2 = f2.share();
    auto sf1_copy = sf1;  // 复制构造
    auto sf1_move = std::move(sf1);  // 移动构造

    // 相同底层对象的副本应该相等
    EXPECT_TRUE(sf1_copy == sf1_move);
    EXPECT_FALSE(sf1_copy != sf1_move);

    // 不同底层对象应该不相等
    EXPECT_FALSE(sf1_copy == sf2);
    EXPECT_TRUE(sf1_copy != sf2);

    // 无效的 sharedFuture 应该相等（都是nullptr）
    async::sharedFuture<int> invalid1, invalid2;
    EXPECT_TRUE(invalid1 == invalid2);
    EXPECT_FALSE(invalid1 != invalid2);

    // 有效的和无效的不相等
    EXPECT_FALSE(sf1_copy == invalid1);
    EXPECT_TRUE(sf1_copy != invalid1);

    runPromiseInThread(p1);
    runPromiseInThread(p2);

    // 比较结果不应该影响值访问
    EXPECT_EQ(sf1_copy.result(), 42);
    EXPECT_EQ(sf1_move.result(), 42);
    EXPECT_EQ(sf2.result(), 100);
}
