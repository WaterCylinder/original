#include <gtest/gtest.h>
#include "async.h"
#include "zeit.h"

using namespace original;

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
        throw std::runtime_error("failure in thread");
    });

    const auto f = p.getFuture();
    EXPECT_THROW(f.result(), valueError);
}

