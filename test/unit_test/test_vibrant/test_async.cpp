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

    const auto start = time::point::now();
    auto f = p.getFuture();
    runPromiseInThread(p);

    const int result = f.result();
    const auto end = time::point::now();

    const auto duration = end - start;
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
    auto p = async::makePromise([](const int a, const int b) {
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
    // 第二次调用仍然抛异常
    EXPECT_THROW(f.result(), runTimeTestError);
}

// 测试 get 函数的基本功能
TEST(AsyncTest, GetAsyncBasicFunctionality) {
    auto sf = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    EXPECT_TRUE(sf.valid());
    EXPECT_FALSE(sf.ready());  // 任务刚开始，应该还没完成

    // 等待任务完成
    sf.wait();

    EXPECT_TRUE(sf.ready());
    EXPECT_EQ(sf.result(), 42);
}

// 测试 get 函数的异常处理
TEST(AsyncTest, GetAsyncExceptionHandling) {
    auto sf = async::get([]() -> int {
        thread::sleep(milliseconds(50));
        throw runTimeTestError("getAsync error");
        return 42;
    });

    EXPECT_TRUE(sf.valid());

    // 等待任务完成
    sf.wait();

    EXPECT_TRUE(sf.ready());
    EXPECT_THROW(sf.result(), runTimeTestError);
}

// 测试 get 函数带参数
TEST(AsyncTest, GetAsyncWithArguments) {
    auto sf = async::get([](const int a, const int b, const int c) {
        thread::sleep(milliseconds(50));
        return a + b + c;
    }, 10, 20, 12);

    EXPECT_TRUE(sf.valid());

    // 等待任务完成
    sf.wait();

    EXPECT_TRUE(sf.ready());
    EXPECT_EQ(sf.result(), 42);
}

// 测试 get 的 void 返回类型
TEST(AsyncTest, GetAsyncVoidReturnType) {
    std::atomic completed{false};

    auto sf = async::get([&completed] {
        thread::sleep(milliseconds(50));
        completed.store(true);
    });

    EXPECT_TRUE(sf.valid());

    // 等待任务完成
    sf.wait();

    EXPECT_TRUE(sf.ready());
    EXPECT_NO_THROW(sf.result());
    EXPECT_TRUE(completed.load());
}

// 测试多个 get 任务并发执行
TEST(AsyncTest, GetAsyncMultipleConcurrentTasks) {
    constexpr int numTasks = 10;
    std::vector<async::sharedFuture<int>> futures;

    // 启动多个并发任务
    for (int i = 0; i < numTasks; i++) {
        futures.push_back(async::get([i] {
            thread::sleep(milliseconds(20));
            return i * i;
        }).share());
    }

    // 等待所有任务完成
    for (auto& sf : futures) {
        sf.wait();
        EXPECT_TRUE(sf.ready());
    }

    // 验证所有结果
    for (int i = 0; i < numTasks; i++) {
        EXPECT_EQ(futures[i].result(), i * i);
    }
}

// 测试 get 任务的自动分离线程
TEST(AsyncTest, GetAsyncAutoDetachThread) {
    // 这个测试验证线程确实会自动分离，不会造成资源泄漏
    const auto sf = async::get([] {
        thread::sleep(milliseconds(100));
        return 123;
    }).share();

    // 获取结果，应该能正常完成
    EXPECT_EQ(sf.result(), 123);

    // 多次调用应该都能正常工作（因为是 sharedFuture）
    EXPECT_EQ(sf.result(), 123);
    EXPECT_EQ(sf.result(), 123);
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
    const auto sf = f.share();  // 转换为 sharedFuture

    runPromiseInThread(p);

    // 多个 sharedFuture 副本应该都能访问结果
    auto sf2 = sf;  // NOLINT: Copy test
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
    const auto sf = f.share();

    runPromiseInThread(p);

    // 应该能正常调用（无返回值）
    EXPECT_NO_THROW(sf.result());

    // 可以复制
    auto sf2 = sf;  // NOLINT: Copy test
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
    auto sf2 = sf;  // NOLINT: Copy test
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
    const auto exceptionPtr = basePtr->exception();
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

// 测试 strongPointer() 方法
TEST(AsyncTest, SharedFutureStrongPointerMethod) {
    auto p = async::makePromise([] {
        thread::sleep(milliseconds(100));
        return std::vector{1, 2, 3, 4, 5};
    });

    auto f = p.getFuture();
    auto sf = f.share();

    runPromiseInThread(p);
    sf.wait();  // 等待任务完成

    // 使用 strongPointer() 避免拷贝
    const auto result_ptr = sf.strongPointer();
    ASSERT_NE(result_ptr, nullptr);
    EXPECT_EQ(result_ptr->size(), 5);
    EXPECT_EQ((*result_ptr)[0], 1);
    EXPECT_EQ((*result_ptr)[4], 5);

    // 原始数据不应被修改（因为是 const 指针）
    // (*result_ptr)[0] = 99;  // 这行应该编译错误

    // 拷贝后的结果可以随意修改
    auto result = *result_ptr;
    result.push_back(6);
    EXPECT_EQ(result.size(), 6);
}

// 测试 sharedFuture 的哈希功能
TEST(AsyncTest, SharedFutureHashFunctionality) {
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

    runPromiseInThread(p1);
    runPromiseInThread(p2);

    // 相同底层对象的副本应该有相同的哈希值
    EXPECT_EQ(sf1_copy.toHash(), sf1_move.toHash());
    EXPECT_TRUE(sf1_copy.equals(sf1_move));

    // 不同底层对象应该有不同的哈希值（大概率）
    EXPECT_NE(sf1_copy.toHash(), sf2.toHash());
    EXPECT_FALSE(sf1_copy.equals(sf2));

    // 验证哈希值的一致性（多次调用应该返回相同值）
    auto hash1 = sf1_copy.toHash();
    auto hash2 = sf1_copy.toHash();
    EXPECT_EQ(hash1, hash2);

    // 无效的 sharedFuture 应该有合理的哈希值（通常是0）
    async::sharedFuture<int> invalid1, invalid2;
    EXPECT_EQ(invalid1.toHash(), invalid2.toHash());
    EXPECT_TRUE(invalid1.equals(invalid2));

    // 有效的和无效的不应该相等
    EXPECT_FALSE(sf1_copy.equals(invalid1));
    EXPECT_NE(sf1_copy.toHash(), invalid1.toHash());
}

// 测试 void 类型的 sharedFuture 哈希
TEST(AsyncTest, SharedFutureVoidHashFunctionality) {
    auto p1 = async::makePromise([] {
        thread::sleep(milliseconds(50));
    });
    auto p2 = async::makePromise([] {
        thread::sleep(milliseconds(50));
    });

    auto f1 = p1.getFuture();
    auto f2 = p2.getFuture();

    auto sf1 = f1.share();
    auto sf2 = f2.share();
    auto sf1_copy = sf1;

    runPromiseInThread(p1);
    runPromiseInThread(p2);

    // 等待任务完成
    sf1.wait();
    sf2.wait();

    // 相同底层对象应该有相同的哈希值
    EXPECT_EQ(sf1.toHash(), sf1_copy.toHash());
    EXPECT_TRUE(sf1.equals(sf1_copy));

    // 不同底层对象应该有不同的哈希值
    EXPECT_NE(sf1.toHash(), sf2.toHash());
    EXPECT_FALSE(sf1.equals(sf2));

    // 验证哈希一致性
    EXPECT_EQ(sf1.toHash(), sf1.toHash());
}

// 测试 sharedFuture 在 unordered_set 中的使用
TEST(AsyncTest, SharedFutureInUnorderedSet) {
    std::unordered_set<async::sharedFuture<int>> future_set;

    auto p1 = async::makePromise([] { return 1; });
    auto p2 = async::makePromise([] { return 2; });
    auto p3 = async::makePromise([] { return 3; });

    auto sf1 = p1.getFuture().share();
    auto sf2 = p2.getFuture().share();
    auto sf3 = p3.getFuture().share();
    auto sf1_copy = sf1;  // 副本

    runPromiseInThread(p1);
    runPromiseInThread(p2);
    runPromiseInThread(p3);

    // 插入到 unordered_set
    future_set.insert(sf1);
    future_set.insert(sf2);
    future_set.insert(sf3);
    future_set.insert(sf1_copy);  // 应该不会插入，因为与 sf1 相同

    // 应该有3个元素（sf1_copy 是重复的）
    EXPECT_EQ(future_set.size(), 3);

    // 检查元素是否存在
    EXPECT_NE(future_set.find(sf1), future_set.end());
    EXPECT_NE(future_set.find(sf2), future_set.end());
    EXPECT_NE(future_set.find(sf3), future_set.end());
    EXPECT_NE(future_set.find(sf1_copy), future_set.end());  // 应该找到 sf1

    // 检查不存在的元素
    auto p4 = async::makePromise([] { return 4; });
    auto sf4 = p4.getFuture().share();
    EXPECT_EQ(future_set.find(sf4), future_set.end());
}

// 测试 sharedFuture 在 unordered_map 中的使用
TEST(AsyncTest, SharedFutureInUnorderedMap) {
    std::unordered_map<async::sharedFuture<int>, std::string> future_map;

    auto p1 = async::makePromise([] { return 1; });
    auto p2 = async::makePromise([] { return 2; });

    auto sf1 = p1.getFuture().share();
    auto sf2 = p2.getFuture().share();
    auto sf1_copy = sf1;  // 副本

    runPromiseInThread(p1);
    runPromiseInThread(p2);

    // 插入到 unordered_map
    future_map[sf1] = "first";
    future_map[sf2] = "second";
    future_map[sf1_copy] = "first_copy";  // 应该覆盖之前的值

    // 应该有2个元素
    EXPECT_EQ(future_map.size(), 2);

    // 检查值
    EXPECT_EQ(future_map[sf1], "first_copy");
    EXPECT_EQ(future_map[sf2], "second");
    EXPECT_EQ(future_map[sf1_copy], "first_copy");

    // 使用 find 方法
    auto it1 = future_map.find(sf1);
    ASSERT_NE(it1, future_map.end());
    EXPECT_EQ(it1->second, "first_copy");

    auto it2 = future_map.find(sf2);
    ASSERT_NE(it2, future_map.end());
    EXPECT_EQ(it2->second, "second");
}

// 测试哈希值的稳定性（多次运行应该相同）
TEST(AsyncTest, SharedFutureHashStability) {
    auto p = async::makePromise([] {
        return 42;
    });

    auto sf = p.getFuture().share();
    runPromiseInThread(p);

    // 获取哈希值多次，应该都相同
    const auto hash1 = sf.toHash();
    const auto hash2 = sf.toHash();
    const auto hash3 = sf.toHash();

    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash2, hash3);
    EXPECT_EQ(hash1, hash3);

    // 即使等待任务完成，哈希值也应该不变
    sf.wait();
    const auto hash_after_wait = sf.toHash();
    EXPECT_EQ(hash1, hash_after_wait);

    // 获取结果后，哈希值也应该不变
    const int result = sf.result();
    EXPECT_EQ(result, 42);
    const auto hash_after_result = sf.toHash();
    EXPECT_EQ(hash1, hash_after_result);
}

// 测试异常情况下的哈希行为
TEST(AsyncTest, SharedFutureHashWithException) {
    auto p = async::makePromise([]() -> int {
        throw runTimeTestError("hash test error");
    });

    const auto sf = p.getFuture().share();
    runPromiseInThread(p);

    // 即使有异常，哈希值也应该可用
    EXPECT_NO_THROW({
        std::cout << sf.toHash() << std::endl;
    });
    const auto hash = sf.toHash();

    // 多次调用应该返回相同的哈希值
    EXPECT_EQ(sf.toHash(), hash);
    EXPECT_EQ(sf.toHash(), hash);

    // 尝试获取结果应该抛出异常，但哈希值不变
    EXPECT_THROW(sf.result(), runTimeTestError);
    EXPECT_EQ(sf.toHash(), hash);
}

// 测试移动语义对哈希的影响
TEST(AsyncTest, SharedFutureHashMoveSemantics) {
    auto p = async::makePromise([] {
        return 42;
    });

    auto sf_original = p.getFuture().share();
    runPromiseInThread(p);

    const auto original_hash = sf_original.toHash();

    // 移动构造
    async::sharedFuture sf_moved(std::move(sf_original));
    EXPECT_EQ(sf_moved.toHash(), original_hash);

    // 移动后原始对象应该无效
    EXPECT_FALSE(sf_original.valid());
    EXPECT_EQ(sf_original.toHash(), 0);  // 无效对象的哈希值应该是0

    // 移动赋值
    const async::sharedFuture<int> sf_target = std::move(sf_moved);
    EXPECT_EQ(sf_target.toHash(), original_hash);
    EXPECT_FALSE(sf_moved.valid());
    EXPECT_EQ(sf_moved.toHash(), 0);
}


// 测试 future 的 waitFor 方法（成功情况）
TEST(AsyncTest, FutureWaitForSuccess) {
    auto f = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    // 等待足够长的时间，应该成功
    const bool completed = f.waitFor(milliseconds(200));
    EXPECT_TRUE(completed);
    EXPECT_TRUE(f.ready());
    EXPECT_EQ(f.result(), 42);
}

// 测试 future 的 waitFor 方法（超时情况）
TEST(AsyncTest, FutureWaitForTimeout) {
    auto f = async::get([] {
        thread::sleep(milliseconds(200));  // 需要200ms
        return 42;
    });

    // 立即检查，应该还没完成
    EXPECT_FALSE(f.ready());

    // 等待时间不够，应该超时
    const bool completed = f.waitFor(milliseconds(50));
    EXPECT_FALSE(completed);
    EXPECT_FALSE(f.ready());  // 应该还没完成

    // 现在等待足够的时间
    f.wait();
    EXPECT_TRUE(f.ready());
    EXPECT_EQ(f.result(), 42);
}

// 测试 future<void> 的 waitFor 方法
TEST(AsyncTest, FutureVoidWaitFor) {
    std::atomic completed{false};

    auto f = async::get([&completed] {
        thread::sleep(milliseconds(100));
        completed = true;
    });

    // 等待足够长的时间
    const bool success = f.waitFor(milliseconds(150));
    EXPECT_TRUE(success);
    EXPECT_TRUE(f.ready());
    EXPECT_NO_THROW(f.result());
    EXPECT_TRUE(completed);
}

// 测试 sharedFuture 的 waitFor 方法
TEST(AsyncTest, SharedFutureWaitFor) {
    const auto sf = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    }).share();

    // 等待足够长的时间
    const bool completed = sf.waitFor(milliseconds(150));
    EXPECT_TRUE(completed);
    EXPECT_TRUE(sf.ready());
    EXPECT_EQ(sf.result(), 42);

    // 副本也应该有相同的行为
    const auto sf_copy = sf;
    EXPECT_TRUE(sf_copy.ready());
    EXPECT_EQ(sf_copy.result(), 42);
}

// 测试 sharedFuture<void> 的 waitFor 方法
TEST(AsyncTest, SharedFutureVoidWaitFor) {
    std::atomic executed{false};

    const auto sf = async::get([&executed] {
        thread::sleep(milliseconds(100));
        executed = true;
    }).share();

    // 等待足够长的时间
    const bool success = sf.waitFor(milliseconds(150));
    EXPECT_TRUE(success);
    EXPECT_TRUE(sf.ready());
    EXPECT_NO_THROW(sf.result());
    EXPECT_TRUE(executed);
}

// 测试 futureBase 接口的 waitFor 方法
TEST(AsyncTest, FutureBaseWaitFor) {
    auto f = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    async::futureBase* base_ptr = &f;

    // 通过基类接口调用 waitFor
    const bool completed = base_ptr->waitFor(milliseconds(150));
    EXPECT_TRUE(completed);
    EXPECT_TRUE(base_ptr->ready());
    EXPECT_EQ(base_ptr->exception(), nullptr);
}

// 测试 sharedFuture 通过 futureBase 接口的 waitFor
TEST(AsyncTest, SharedFutureBaseWaitFor) {
    const auto sf = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    }).share();

    const async::futureBase* base_ptr = &sf;

    // 通过基类接口调用 waitFor
    const bool completed = base_ptr->waitFor(milliseconds(150));
    EXPECT_TRUE(completed);
    EXPECT_TRUE(base_ptr->ready());
    EXPECT_EQ(base_ptr->exception(), nullptr);
}

// 测试 waitFor 在异常情况下的行为
TEST(AsyncTest, WaitForWithException) {
    auto f = async::get([]() -> int {
        thread::sleep(milliseconds(50));
        throw runTimeTestError("waitFor test error");
    });

    // 等待足够长的时间
    const bool completed = f.waitFor(milliseconds(100));
    EXPECT_TRUE(completed);
    EXPECT_TRUE(f.ready());

    // 应该能检测到异常
    const auto exception_ptr = f.exception();
    EXPECT_NE(exception_ptr, nullptr);
    EXPECT_THROW(std::rethrow_exception(exception_ptr), runTimeTestError);
    EXPECT_THROW(f.result(), runTimeTestError);
}

// 测试 waitFor 的多次调用
TEST(AsyncTest, WaitForMultipleCalls) {
    auto f = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    // 第一次调用，可能还没完成
    f.waitFor(milliseconds(50));

    // 第二次调用，等待更长时间
    const bool second_call = f.waitFor(milliseconds(100));

    // 第三次调用，应该已经完成了
    const bool third_call = f.waitFor(milliseconds(50));

    EXPECT_TRUE(second_call || third_call);  // 至少有一次应该成功
    EXPECT_TRUE(f.ready());
    EXPECT_EQ(f.result(), 42);
}

// 测试 waitFor 零超时的情况
TEST(AsyncTest, WaitForZeroTimeout) {
    auto f = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    });

    // 零超时，应该立即返回
    if (f.waitFor(milliseconds(0))) {
        EXPECT_TRUE(f.ready());
        EXPECT_EQ(f.result(), 42);
    } else {
        EXPECT_FALSE(f.ready());
        // 稍等一会儿再检查
        thread::sleep(milliseconds(150));
        EXPECT_TRUE(f.ready());
        EXPECT_EQ(f.result(), 42);
    }
}

// 测试 waitFor 在任务立即完成的情况
TEST(AsyncTest, WaitForImmediateCompletion) {
    auto f = async::get([] {
        return 42;  // 立即返回
    });

    // 即使是很短的超时，也应该立即完成
    const bool completed = f.waitFor(milliseconds(1));
    EXPECT_TRUE(completed);
    EXPECT_TRUE(f.ready());
    EXPECT_EQ(f.result(), 42);
}

// 测试 waitFor 与 wait 的组合使用
TEST(AsyncTest, WaitForAndWaitCombination) {
    auto f = async::get([] {
        thread::sleep(milliseconds(200));
        return 42;
    });

    // 先用 waitFor 尝试短时间等待
    const bool short_wait = f.waitFor(milliseconds(50));
    EXPECT_FALSE(short_wait);
    EXPECT_FALSE(f.ready());

    // 然后用 wait 无限等待
    f.wait();
    EXPECT_TRUE(f.ready());
    EXPECT_EQ(f.result(), 42);

    // 现在 waitFor 应该立即返回 true
    const bool final_wait = f.waitFor(milliseconds(0));
    EXPECT_TRUE(final_wait);
}

// 测试 waitFor 在多个共享 future 上的行为
TEST(AsyncTest, WaitForMultipleSharedFutures) {
    const auto sf1 = async::get([] {
        thread::sleep(milliseconds(100));
        return 42;
    }).share();

    const auto sf2 = sf1;  // 副本

    // 两个共享 future 都应该能正确响应 waitFor
    const bool completed1 = sf1.waitFor(milliseconds(150));
    const bool completed2 = sf2.waitFor(milliseconds(150));

    EXPECT_TRUE(completed1);
    EXPECT_TRUE(completed2);
    EXPECT_TRUE(sf1.ready());
    EXPECT_TRUE(sf2.ready());
    EXPECT_EQ(sf1.result(), 42);
    EXPECT_EQ(sf2.result(), 42);
}