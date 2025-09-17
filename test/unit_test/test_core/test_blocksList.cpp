#include <algorithm>
#include <gtest/gtest.h>
#include "blocksList.h"
#include <deque>

// 对比函数，用于比较 original::blocksList 和 std::deque
void compareBlocksList(const original::blocksList<int>& originalBL, const std::deque<int>& stdDQ) {
    ASSERT_EQ(originalBL.size(), stdDQ.size());
    for (size_t i = 0; i < stdDQ.size(); ++i) {
        ASSERT_EQ(originalBL.get(i), stdDQ[i]);
    }
}

// 测试 original::blocksList 类
class BlocksListTest : public testing::Test {
protected:
    void SetUp() override {
        // 每个测试用例前都创建一个空的 original::blocksList 和 std::deque
        originalBL = original::blocksList<int>();
        stdDQ = std::deque<int>();
    }

    original::blocksList<int> originalBL;
    std::deque<int> stdDQ;
};

// 测试 push 和 pop 操作
TEST_F(BlocksListTest, PushPopTest) {
    // Testing pushEnd
    this->originalBL.pushEnd(1);
    this->stdDQ.push_back(1);
    compareBlocksList(this->originalBL, this->stdDQ);

    // Testing pushBegin
    this->originalBL.pushBegin(2);
    this->stdDQ.insert(this->stdDQ.begin(), 2);
    compareBlocksList(this->originalBL, this->stdDQ);

    // Testing push at middle
    this->originalBL.push(1, 3);
    this->stdDQ.insert(this->stdDQ.begin() + 1, 3);
    compareBlocksList(this->originalBL, this->stdDQ);

    // Testing popEnd
    int endPop = this->originalBL.popEnd();
    ASSERT_EQ(endPop, 1);
    this->stdDQ.pop_back();
    compareBlocksList(this->originalBL, this->stdDQ);

    // Testing popBegin
    int beginPop = this->originalBL.popBegin();
    ASSERT_EQ(beginPop, 2);
    this->stdDQ.erase(this->stdDQ.begin());
    compareBlocksList(this->originalBL, this->stdDQ);

    // Testing pop at middle
    int middlePop = this->originalBL.pop(0);
    ASSERT_EQ(middlePop, 3);
    this->stdDQ.erase(this->stdDQ.begin() + 0);
    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试索引和赋值操作
TEST_F(BlocksListTest, IndexAndSetTest) {
    this->originalBL.pushEnd(1);
    this->originalBL.pushEnd(2);
    this->stdDQ.push_back(1);
    this->stdDQ.push_back(2);

    // Testing operator[]
    ASSERT_EQ(this->originalBL[0], this->stdDQ[0]);
    ASSERT_EQ(this->originalBL[1], this->stdDQ[1]);

    // Testing set
    this->originalBL.set(0, 3);
    this->stdDQ[0] = 3;
    compareBlocksList(this->originalBL, this->stdDQ);

    this->originalBL[0] = 4;
    this->stdDQ[0] = 4;
    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试迭代器操作
TEST_F(BlocksListTest, IteratorTest) {
    this->originalBL.pushEnd(1);
    this->originalBL.pushEnd(2);
    this->originalBL.pushEnd(3);
    this->stdDQ.push_back(1);
    this->stdDQ.push_back(2);
    this->stdDQ.push_back(3);

    // Testing Iterator (begin and end)
    auto* originalBegin = this->originalBL.begins();
    auto stdBegin = this->stdDQ.begin();

    for (const auto it = originalBegin; it->isValid(); it->next(), ++stdBegin) {
        ASSERT_EQ(**it, *stdBegin);
    }

    delete originalBegin;
}

// 测试其他基本方法
TEST_F(BlocksListTest, BasicMethodsTest) {
    // Testing size
    ASSERT_EQ(this->originalBL.size(), this->stdDQ.size());

    // Testing push and pop at begin and end
    this->originalBL.pushEnd(1);
    this->stdDQ.push_back(1);
    this->originalBL.pushEnd(2);
    this->stdDQ.push_back(2);
    compareBlocksList(this->originalBL, this->stdDQ);

    // Testing indexOf
    ASSERT_EQ(this->originalBL.indexOf(1), this->stdDQ.size() - 2);
    ASSERT_EQ(this->originalBL.indexOf(2), this->stdDQ.size() - 1);

    // Testing equality operator
    const original::blocksList<int> otherBL = this->originalBL;
    ASSERT_TRUE(this->originalBL == otherBL);
}

// 测试构造函数
TEST_F(BlocksListTest, ConstructorTest) {
    // Testing with initializer list
    const original::blocksList blFromList = {1, 2};
    const std::deque dqFromList = {1, 2};
    compareBlocksList(blFromList, dqFromList);

    // Testing with array
    const original::array arr = {3, 4};
    const original::blocksList vecFromArray(arr);
    const std::deque dqFromArray = {3, 4};
    compareBlocksList(vecFromArray, dqFromArray);
}

// 测试大数据量
TEST_F(BlocksListTest, LargeDataTest) {
    constexpr size_t dataSize = 1000000;  // 测试 100万数据
    for (size_t i = 0; i < dataSize; ++i) {
        this->originalBL.pushEnd(static_cast<int>(i));
        this->stdDQ.push_back(static_cast<int>(i));
    }

    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试空容器
TEST_F(BlocksListTest, EmptyContainerTest) {
    ASSERT_EQ(this->originalBL.size(), 0);
    ASSERT_EQ(this->stdDQ.size(), 0);

    // Testing pop on empty vector (should throw an exception)
    ASSERT_THROW(this->originalBL.popEnd(), original::noElementError);

    // Testing index on empty vector (should throw an exception)
    ASSERT_THROW(this->originalBL[0], original::outOfBoundError);
}

// 测试单一元素
TEST_F(BlocksListTest, SingleElementTest) {
    this->originalBL.pushEnd(1);
    this->stdDQ.push_back(1);

    // Test if the single element is correct
    ASSERT_EQ(this->originalBL[0], this->stdDQ[0]);

    // Test popping the only element
    int poppedValue = this->originalBL.popEnd();
    ASSERT_EQ(poppedValue, 1);
    this->stdDQ.pop_back();
    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试在容器中间插入和弹出
TEST_F(BlocksListTest, InsertPopMiddleTest) {
    this->originalBL.pushEnd(1);
    this->originalBL.pushEnd(2);
    this->originalBL.pushEnd(3);
    this->stdDQ.push_back(1);
    this->stdDQ.push_back(2);
    this->stdDQ.push_back(3);

    // Insert in the middle
    this->originalBL.push(1, 10);
    this->stdDQ.insert(this->stdDQ.begin() + 1, 10);
    compareBlocksList(this->originalBL, this->stdDQ);

    // Pop from the middle
    int middlePop = this->originalBL.pop(1);
    ASSERT_EQ(middlePop, 10);
    this->stdDQ.erase(this->stdDQ.begin() + 1);
    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试重复元素
TEST_F(BlocksListTest, DuplicateElementTest) {
    this->originalBL.pushEnd(1);
    this->originalBL.pushEnd(1);
    this->stdDQ.push_back(1);
    this->stdDQ.push_back(1);

    // Test if the repeated elements are correctly handled
    ASSERT_EQ(this->originalBL[0], this->stdDQ[0]);
    ASSERT_EQ(this->originalBL[1], this->stdDQ[1]);

    // Test removing duplicate elements
    this->originalBL.popEnd();
    this->stdDQ.pop_back();
    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试容器大小
TEST_F(BlocksListTest, SizeTest) {
    // Initially both vectors should be empty
    ASSERT_EQ(this->originalBL.size(), 0);
    ASSERT_EQ(this->stdDQ.size(), 0);

    // Push elements and check size
    this->originalBL.pushEnd(1);
    this->stdDQ.push_back(1);
    ASSERT_EQ(this->originalBL.size(), 1);
    ASSERT_EQ(this->stdDQ.size(), 1);

    this->originalBL.pushEnd(2);
    this->stdDQ.push_back(2);
    ASSERT_EQ(this->originalBL.size(), 2);
    ASSERT_EQ(this->stdDQ.size(), 2);

    // Pop elements and check size
    this->originalBL.popEnd();
    this->stdDQ.pop_back();
    ASSERT_EQ(this->originalBL.size(), 1);
    ASSERT_EQ(this->stdDQ.size(), 1);

    this->originalBL.popEnd();
    this->stdDQ.pop_back();
    ASSERT_EQ(this->originalBL.size(), 0);
    ASSERT_EQ(this->stdDQ.size(), 0);
}

// 测试边界访问
TEST_F(BlocksListTest, BoundaryAccessTest) {
    this->originalBL.pushEnd(10);
    this->stdDQ.push_back(10);

    // Testing accessing the last element
    ASSERT_EQ(this->originalBL[0], this->stdDQ[0]);
    ASSERT_EQ(this->originalBL.get(0), this->stdDQ[0]);

    // Testing out-of-bounds access
    ASSERT_THROW(this->originalBL[1], original::outOfBoundError);
}

// 测试 pushBegin
TEST_F(BlocksListTest, PushBeginTest) {
    constexpr size_t dataSize = 100000;  // 大数据量测试
    for (size_t i = dataSize; i > 0; --i) {
        this->originalBL.pushBegin(static_cast<int>(i));
        this->stdDQ.insert(this->stdDQ.begin(), static_cast<int>(i));
    }

    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试 popEnd
TEST_F(BlocksListTest, PopEndTest) {
    constexpr size_t dataSize = 100000;  // 大数据量测试
    for (size_t i = 0; i < dataSize; ++i) {
        this->originalBL.pushEnd(static_cast<int>(i));
        this->stdDQ.push_back(static_cast<int>(i));
    }

    // 弹出元素并验证
    for (size_t i = dataSize - 1; i > 0; --i) {
        int popValueOriginal = this->originalBL.popEnd();
        int popValueStd = this->stdDQ.back();
        ASSERT_EQ(popValueOriginal, popValueStd);
        this->stdDQ.pop_back();
    }

    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试 popBegin
TEST_F(BlocksListTest, PopBeginTest) {
    constexpr size_t dataSize = 100000;  // 大数据量测试
    for (size_t i = 0; i < dataSize; ++i) {
        this->originalBL.pushEnd(static_cast<int>(i));
        this->stdDQ.push_back(static_cast<int>(i));
    }

    // 弹出元素并验证
    for (size_t i = 0; i < dataSize; ++i) {
        int popValueOriginal = this->originalBL.popBegin();
        int popValueStd = this->stdDQ.front();
        ASSERT_EQ(popValueOriginal, popValueStd);
        this->stdDQ.erase(this->stdDQ.begin());
    }

    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试 push（插入到容器中间）
TEST_F(BlocksListTest, PushTestMiddle) {
    constexpr size_t dataSize = 10000;  // 大数据量测试
    for (size_t i = 0; i < dataSize; ++i) {
        this->originalBL.pushEnd(static_cast<int>(i));
        this->stdDQ.push_back(static_cast<int>(i));
    }

    // 在中间插入元素
    for (size_t i = 0; i < dataSize / 2; ++i) {
        this->originalBL.push(dataSize / 2, static_cast<int>(i));
        this->stdDQ.insert(this->stdDQ.begin() + dataSize / 2, static_cast<int>(i));
    }

    compareBlocksList(this->originalBL, this->stdDQ);
}

// 测试插入和弹出混合操作
TEST_F(BlocksListTest, MixedPushPopTest) {
    constexpr size_t dataSize = 10000;  // 大数据量测试

    // 在末尾插入数据
    for (size_t i = 0; i < dataSize; ++i) {
        this->originalBL.pushEnd(static_cast<int>(i));
        this->stdDQ.push_back(static_cast<int>(i));
    }

    // 在开始插入数据
    for (size_t i = dataSize; i > 0; --i) {
        this->originalBL.pushBegin(static_cast<int>(i));
        this->stdDQ.insert(this->stdDQ.begin(), static_cast<int>(i));
    }

    // 弹出数据
    for (size_t i = 0; i < dataSize; ++i) {
        int popValueOriginalEnd = this->originalBL.popEnd();
        int popValueStdEnd = this->stdDQ.back();
        ASSERT_EQ(popValueOriginalEnd, popValueStdEnd);
        this->stdDQ.pop_back();

        int popValueOriginalBegin = this->originalBL.popBegin();
        int popValueStdBegin = this->stdDQ.front();
        ASSERT_EQ(popValueOriginalBegin, popValueStdBegin);
        this->stdDQ.erase(this->stdDQ.begin());
    }

    compareBlocksList(this->originalBL, this->stdDQ);
}

