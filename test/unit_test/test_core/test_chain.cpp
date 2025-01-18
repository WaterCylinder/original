#include <gtest/gtest.h>
#include "chain.h"

namespace original {
    // 测试创建一个空链表
    TEST(ChainTest, DefaultConstructor) {
        chain<int> c;
        EXPECT_EQ(c.size(), 0);
    }

    // 测试初始化列表构造函数
    TEST(ChainTest, InitializerListConstructor) {
        chain<int> c = {1, 2, 3, 4, 5};
        EXPECT_EQ(c.size(), 5);
        EXPECT_EQ(c.get(0), 1);
        EXPECT_EQ(c.get(4), 5);
    }

    // 测试通过数组构造链表
    TEST(ChainTest, ArrayConstructor) {
        array<int> arr = {10, 20, 30, 40};
        chain<int> c(arr);
        EXPECT_EQ(c.size(), 4);
        EXPECT_EQ(c.get(0), 10);
        EXPECT_EQ(c.get(3), 40);
    }

    // 测试 pushBegin 和 popBegin
    TEST(ChainTest, PushPopBegin) {
        chain<int> c;
        c.pushBegin(10);
        c.pushBegin(20);
        EXPECT_EQ(c.size(), 2);
        EXPECT_EQ(c.popBegin(), 20);
        EXPECT_EQ(c.popBegin(), 10);
        EXPECT_EQ(c.size(), 0);
    }

    // 测试 pushEnd 和 popEnd
    TEST(ChainTest, PushPopEnd) {
        chain<int> c;
        c.pushEnd(10);
        c.pushEnd(20);
        EXPECT_EQ(c.size(), 2);
        EXPECT_EQ(c.popEnd(), 20);
        EXPECT_EQ(c.popEnd(), 10);
        EXPECT_EQ(c.size(), 0);
    }

    // 测试在特定位置插入元素
    TEST(ChainTest, PushPopAtIndex) {
        chain<int> c = {1, 2, 3};
        c.push(1, 10);  // 在位置1插入10
        EXPECT_EQ(c.size(), 4);
        EXPECT_EQ(c.get(1), 10);
        EXPECT_EQ(c.pop(1), 10);
        EXPECT_EQ(c.size(), 3);
    }

    // 测试索引操作符 []
    TEST(ChainTest, IndexOperator) {
        chain<int> c = {1, 2, 3};
        EXPECT_EQ(c[0], 1);
        EXPECT_EQ(c[1], 2);
        EXPECT_EQ(c[2], 3);
        c[1] = 20;
        EXPECT_EQ(c[1], 20);
    }

    // 测试复制构造函数
    TEST(ChainTest, CopyConstructor) {
        chain<int> c1 = {1, 2, 3};
        chain<int> c2 = c1;
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c2.get(2), 3);
    }

    // 测试移动构造函数
    TEST(ChainTest, MoveConstructor) {
        chain<int> c1 = {1, 2, 3};
        chain<int> c2 = std::move(c1);
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c1.size(), 0);  // c1 应该为空
    }

    // 测试复制赋值操作符
    TEST(ChainTest, CopyAssignmentOperator) {
        chain<int> c1 = {1, 2, 3};
        chain<int> c2;
        c2 = c1;
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c2.get(2), 3);
    }

    // 测试移动赋值操作符
    TEST(ChainTest, MoveAssignmentOperator) {
        chain<int> c1 = {1, 2, 3};
        chain<int> c2;
        c2 = std::move(c1);
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c1.size(), 0);  // c1 应该为空
    }

    // 测试连接链表
    TEST(ChainTest, AddAnotherChain) {
        chain<int> c1 = {1, 2, 3};
        chain<int> c2 = {4, 5, 6};
        c1 += c2;  // 连接两个链表
        EXPECT_EQ(c1.size(), 6);
        EXPECT_EQ(c1.get(3), 4);
        EXPECT_EQ(c1.get(5), 6);
        EXPECT_EQ(c2.size(), 0);  // c2 应该已经清空
    }

    // 测试是否等于操作符
    TEST(ChainTest, EqualityOperator) {
        chain<int> c1 = {1, 2, 3};
        chain<int> c2 = {1, 2, 3};
        EXPECT_TRUE(c1 == c2);
        c2.pushEnd(4);
        EXPECT_FALSE(c1 == c2);
    }
}
