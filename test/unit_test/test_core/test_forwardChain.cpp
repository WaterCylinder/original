#include <random>
#include <gtest/gtest.h>
#include "forwardChain.h"
#include "list"

namespace original {
    template <typename T>
    bool compareChainsAndLists(const forwardChain<T>& c, const std::list<T>& l) {
        if (c.size() != l.size()) {
            return false;
        }

        auto cIt = c.begin();
        auto lIt = l.begin();

        while (cIt.isValid() && lIt != l.end()) {
            if (*cIt != *lIt) {
                return false;
            }
            ++cIt;
            ++lIt;
        }

        return true;
    }

    // 测试创建一个空链表
    TEST(forwardChainTest, DefaultConstructor) {
        const forwardChain<int> c;
        const std::list<int> l;
        EXPECT_EQ(c.size(), 0);
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    // 测试初始化列表构造函数
    TEST(forwardChainTest, InitializerListConstructor) {
        const forwardChain c = {1, 2, 3, 4, 5};
        const std::list l = {1, 2, 3, 4, 5};
        EXPECT_EQ(c.size(), 5);
        EXPECT_EQ(c.get(0), 1);
        EXPECT_EQ(c.get(4), 5);
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

        // 测试通过数组构造链表
    TEST(forwardChainTest, ArrayConstructor) {
        const auto i = std::initializer_list{1, 2, 3, 4, 5};
        const auto arr = array(i);
        const forwardChain c(arr);
        const std::list l = i;
        EXPECT_EQ(c.size(), 5);
        EXPECT_EQ(c.get(0), 1);
        EXPECT_EQ(c.get(3), 4);
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    // 测试 pushBegin 和 popBegin
    TEST(forwardChainTest, PushPopBegin) {
        forwardChain<int> c;
        std::list<int> l;
        c.pushBegin(10);
        c.pushBegin(20);
        l.push_front(10);
        l.push_front(20);
        EXPECT_EQ(c.size(), 2);
        EXPECT_TRUE(compareChainsAndLists(c, l));
        EXPECT_EQ(c.popBegin(), 20);
        EXPECT_EQ(c.popBegin(), 10);
        l.pop_front();
        l.pop_front();
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    // 测试 pushEnd 和 popEnd
    TEST(forwardChainTest, PushPopEnd) {
        forwardChain<int> c;
        std::list<int> l;
        c.pushEnd(10);
        c.pushEnd(20);
        l.push_back(10);
        l.push_back(20);
        EXPECT_TRUE(compareChainsAndLists(c, l));
        EXPECT_EQ(c.size(), 2);
        EXPECT_EQ(c.popEnd(), 20);
        EXPECT_EQ(c.popEnd(), 10);
        l.pop_back();
        l.pop_back();
        EXPECT_EQ(c.size(), 0);
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    // 测试在特定位置插入元素
    TEST(forwardChainTest, PushPopAtIndex) {
        forwardChain c = {1, 2, 3};
        std::list l = {1, 2, 3};
        EXPECT_TRUE(compareChainsAndLists(c, l));
        c.push(1, 10);
        auto li = l.begin();
        ++ li;
        l.insert(li, 10);
        EXPECT_TRUE(compareChainsAndLists(c, l));
        EXPECT_EQ(c.size(), 4);
        EXPECT_EQ(c.get(1), 10);
        EXPECT_EQ(c.pop(1), 10);
        li = l.begin();
        ++ li;
        l.erase(li);
        EXPECT_EQ(c.size(), 3);
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    // 测试索引操作符 []
    TEST(forwardChainTest, IndexOperator) {
        forwardChain c = {1, 2, 3};
        std::list l = {1, 2, 3};
        auto li = l.begin();
        EXPECT_EQ(c[0], *li);
        ++ li;
        EXPECT_EQ(c[1], *li);
        ++ li;
        EXPECT_EQ(c[2], *li);
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    // 测试复制构造函数
    TEST(forwardChainTest, CopyConstructor) {
        const forwardChain c1 = {1, 2, 3};
        auto c2 = c1;
        const std::list l = {1, 2, 3};
        EXPECT_EQ(c2.size(), 3);
        EXPECT_TRUE(compareChainsAndLists(c2, l));
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c2.get(2), 3);
    }

    // 测试移动构造函数
    TEST(forwardChainTest, MoveConstructor) {
        forwardChain c1 = {1, 2, 3};
        const forwardChain<int> c2 = std::move(c1);
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c1.size(), 0);
        EXPECT_TRUE(compareChainsAndLists(c1, std::list<int>{}));
        EXPECT_TRUE(compareChainsAndLists(c2, std::list{1, 2, 3}));
    }

    // 测试复制赋值操作符
    TEST(forwardChainTest, CopyAssignmentOperator) {
        const forwardChain c1 = {1, 2, 3};
        forwardChain<int> c2;
        EXPECT_TRUE(compareChainsAndLists(c2, std::list<int>{}));
        c2 = c1;
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c2.get(2), 3);
        EXPECT_TRUE(compareChainsAndLists(c2, std::list{1, 2, 3}));
    }

    // 测试移动赋值操作符
    TEST(forwardChainTest, MoveAssignmentOperator) {
        forwardChain c1 = {1, 2, 3};
        forwardChain<int> c2;
        EXPECT_TRUE(compareChainsAndLists(c2, std::list<int>{}));
        c2 = std::move(c1);
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(c2.get(0), 1);
        EXPECT_EQ(c1.size(), 0);  // c1 应该为空
        EXPECT_TRUE(compareChainsAndLists(c1, std::list<int>{}));
        EXPECT_TRUE(compareChainsAndLists(c2, std::list{1, 2, 3}));
    }

    // 测试是否等于操作符
    TEST(forwardChainTest, EqualityOperator) {
        forwardChain c1 = {1, 2, 3};
        forwardChain c2 = {1, 2, 3};
        const forwardChain c3 = c2;
        EXPECT_TRUE(c1 == c2);
        EXPECT_TRUE(c1 == c3);
        c2.pushEnd(4);
        EXPECT_FALSE(c1 == c2);
        c1.pushEnd(5);
        EXPECT_FALSE(c1 == c3);
    }

    TEST(forwardChainTest, OutOfBoundsAccess) {
        const forwardChain c = {1, 2, 3};
        EXPECT_THROW(c.get(5), outOfBoundError);  // 期望抛出异常
        EXPECT_NO_THROW(c.get(-1));  // 负索引访问
    }

    TEST(forwardChainTest, pushBeginTest) {
        constexpr size_t dataSize = 100000;  // 大数据量测试
        forwardChain<size_t> c;
        std::list<size_t> l;
        for (size_t i = dataSize; i > 0; --i) {
            c.pushBegin(i);
            l.push_front(i);
        }
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    TEST(forwardChainTest, pushEndTest) {
        constexpr size_t dataSize = 100000;  // 大数据量测试
        forwardChain<size_t> c;
        std::list<size_t> l;
        for (size_t i = 0; i < dataSize; ++i) {
            c.pushEnd(i);
            l.push_back(i);
        }
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    TEST(forwardChainTest, pushTest) {
        constexpr int64_t dataSize = 100000;  // 大数据量测试
        std::random_device rd;
        std::mt19937 gen(rd());

        forwardChain<size_t> c;
        std::list<size_t> l;
        for (int i = 0; i < dataSize; ++i) {
            std::uniform_int_distribution dist(0, i);
            const int64_t rand = dist(gen);
            auto li = l.begin();
            for (size_t j = 0; j < rand; ++j) {
                ++li;
            }
            c.push(rand, i);
            l.insert(li, i);
        }
        EXPECT_TRUE(compareChainsAndLists(c, l));
    }

    TEST(forwardChainTest, popBeginTest) {
        constexpr size_t dataSize = 100000;  // 大数据量测试
        forwardChain<size_t> c;
        std::list<size_t> l;
        for (size_t i = dataSize; i > 0; --i) {
            c.pushBegin(i);
            l.push_front(i);
        }
        while (!c.empty()) {
            c.popBegin();
            l.pop_front();
            EXPECT_TRUE(compareChainsAndLists(c, l));
        }
    }

    TEST(forwardChainTest, popEndTest) {
        constexpr size_t dataSize = 100000;  // 大数据量测试
        forwardChain<size_t> c;
        std::list<size_t> l;
        for (size_t i = 0; i < dataSize; ++i) {
            c.pushEnd(i);
            l.push_back(i);
        }
        while (!c.empty()) {
            c.popEnd();
            l.pop_back();
            EXPECT_TRUE(compareChainsAndLists(c, l));
        }
    }

    TEST(forwardChainTest, popTest) {
        constexpr int64_t dataSize = 100000;  // 大数据量测试
        std::random_device rd;
        std::mt19937 gen(rd());

        forwardChain<size_t> c;
        std::list<size_t> l;
        for (int i = 0; i < dataSize; ++i) {
            std::uniform_int_distribution dist(0, i);
            const int64_t rand = dist(gen);
            auto li = l.begin();
            for (size_t j = 0; j < rand; ++j) {
                ++li;
            }
            c.push(rand, i);
            l.insert(li, i);
        }
        while (!c.empty()) {
            std::uniform_int_distribution dist(0, static_cast<int>(c.size() - 1));
            const int64_t rand = dist(gen);
            auto li = l.begin();
            for (size_t j = 0; j < rand; ++j) {
                ++li;
            }
            c.pop(rand);
            l.erase(li);
            EXPECT_TRUE(compareChainsAndLists(c, l));
        }
    }

            // 测试链表元素为指针类型时的拷贝构造函数
    TEST(forwardChainTest, PointerCopyConstructor) {
        // 创建指针类型链表
        forwardChain<int*> c1;
        int a = 1, b = 2, c = 3;
        c1.pushEnd(&a);
        c1.pushEnd(&b);
        c1.pushEnd(&c);

        // 拷贝构造
        const forwardChain<int*> c2 = c1;
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(*c2.get(0), 1);
        EXPECT_EQ(*c2.get(1), 2);
        EXPECT_EQ(*c2.get(2), 3);

        // 检查拷贝后，原链表和新链表的指针是否相同
        EXPECT_EQ(&a, c2.get(0));  // 指针应该是相同的
        EXPECT_EQ(&b, c2.get(1));
        EXPECT_EQ(&c, c2.get(2));
    }

    // 测试链表元素为指针类型时的移动构造函数
    TEST(forwardChainTest, PointerMoveConstructor) {
        // 创建指针类型链表
        forwardChain<int*> c1;
        int a = 1, b = 2, c = 3;
        c1.pushEnd(&a);
        c1.pushEnd(&b);
        c1.pushEnd(&c);

        // 移动构造
        const forwardChain<int*> c2 = std::move(c1);
        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(*c2.get(0), 1);
        EXPECT_EQ(*c2.get(1), 2);
        EXPECT_EQ(*c2.get(2), 3);

        // 检查 c1 是否为空
        EXPECT_EQ(c1.size(), 0);
    }

    // 测试链表元素为指针类型时的拷贝赋值操作符
    TEST(forwardChainTest, PointerCopyAssignmentOperator) {
        // 创建指针类型链表
        forwardChain<int*> c1;
        int a = 1, b = 2, c = 3;
        c1.pushEnd(&a);
        c1.pushEnd(&b);
        c1.pushEnd(&c);

        // 创建一个空链表
        forwardChain<int*> c2;
        c2 = c1;

        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(*c2.get(0), 1);
        EXPECT_EQ(*c2.get(1), 2);
        EXPECT_EQ(*c2.get(2), 3);

        // 检查指针的赋值行为
        EXPECT_EQ(&a, c2.get(0));  // 指针应该是相同的
        EXPECT_EQ(&b, c2.get(1));
        EXPECT_EQ(&c, c2.get(2));
    }

    // 测试链表元素为指针类型时的移动赋值操作符
    TEST(forwardChainTest, PointerMoveAssignmentOperator) {
        // 创建指针类型链表
        forwardChain<int*> c1;
        int a = 1, b = 2, c = 3;
        c1.pushEnd(&a);
        c1.pushEnd(&b);
        c1.pushEnd(&c);

        // 创建一个空链表
        forwardChain<int*> c2;
        c2 = std::move(c1);

        EXPECT_EQ(c2.size(), 3);
        EXPECT_EQ(*c2.get(0), 1);
        EXPECT_EQ(*c2.get(1), 2);
        EXPECT_EQ(*c2.get(2), 3);

        // 检查 c1 是否为空
        EXPECT_EQ(c1.size(), 0);
    }
}