#include "singleton.h"
#include <gtest/gtest.h>
#include <string>

using namespace original;

// 一个简单测试类
struct TestConfig {
    TestConfig() : value(0) {}
    explicit TestConfig(const int v) : value(v) {}
    int value;
};

// 测试 init 正常初始化
TEST(SingletonTest, InitAndInstance) {
    // 先清理，保证干净环境
    singleton<TestConfig>::clear();
    EXPECT_FALSE(singleton<TestConfig>::exist());

    // init 带参数初始化
    singleton<TestConfig>::init(42);
    EXPECT_TRUE(singleton<TestConfig>::exist());

    // 获取实例
    const auto& instance = singleton<TestConfig>::instance();
    EXPECT_EQ(instance.value, 42);
}

// 测试 init 重复调用抛异常
TEST(SingletonTest, InitTwiceThrows) {
    singleton<TestConfig>::clear();
    EXPECT_FALSE(singleton<TestConfig>::exist());
    singleton<TestConfig>::init(1);
    EXPECT_TRUE(singleton<TestConfig>::exist());

    EXPECT_THROW(singleton<TestConfig>::init(2), valueError);

    const auto& inst = singleton<TestConfig>::instance();
    EXPECT_EQ(inst.value, 1);  // 保证旧实例未被覆盖
}

// 测试 reset 可以重建实例
TEST(SingletonTest, ResetRebuildsInstance) {
    singleton<TestConfig>::clear();
    EXPECT_FALSE(singleton<TestConfig>::exist());
    singleton<TestConfig>::init(10);
    EXPECT_TRUE(singleton<TestConfig>::exist());

    // reset
    singleton<TestConfig>::reset(20);
    EXPECT_TRUE(singleton<TestConfig>::exist());

    const auto& inst = singleton<TestConfig>::instance();
    EXPECT_EQ(inst.value, 20);
}

// 测试 clear 可以释放实例
TEST(SingletonTest, ClearReleasesInstance) {
    singleton<TestConfig>::clear();
    EXPECT_FALSE(singleton<TestConfig>::exist());
    singleton<TestConfig>::init(5);
    EXPECT_TRUE(singleton<TestConfig>::exist());

    singleton<TestConfig>::clear();
    EXPECT_FALSE(singleton<TestConfig>::exist());

    // 再次访问 instance 应该抛异常
    EXPECT_THROW(singleton<TestConfig>::instance(), nullPointerError);
}

// 测试默认构造
struct DefaultCtorTest {
    int x = 0;
};

TEST(SingletonTest, DefaultConstructor) {
    singleton<DefaultCtorTest>::clear();
    EXPECT_FALSE(singleton<DefaultCtorTest>::exist());
    singleton<DefaultCtorTest>::init(); // 默认构造
    EXPECT_TRUE(singleton<DefaultCtorTest>::exist());
    const auto& [x] = singleton<DefaultCtorTest>::instance();
    EXPECT_EQ(x, 0);
}

// 多次获取实例是同一个对象
TEST(SingletonTest, SameInstanceMultipleAccess) {
    singleton<TestConfig>::clear();
    EXPECT_FALSE(singleton<TestConfig>::exist());
    singleton<TestConfig>::init(99);
    EXPECT_TRUE(singleton<TestConfig>::exist());

    const auto& a = singleton<TestConfig>::instance();
    const auto& b = singleton<TestConfig>::instance();

    EXPECT_EQ(&a, &b);
}
