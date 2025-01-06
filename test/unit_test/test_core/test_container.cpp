#include "container.h"
#include "vector"
#include "algorithm"
#include <gtest/gtest.h>

using namespace original;

template<typename TYPE>
class vectorContainer : public container<TYPE> {
private:
    std::vector<TYPE> data_{};
public:
    [[nodiscard]] uint32_t size() const override {
        return data_.size();
    }

    [[nodiscard]] bool contains(const TYPE& e) const override {
        return std::find(data_.begin(), data_.end(), e) != data_.end();
    }

    void add(const TYPE& e) override {
        data_.push_back(e);
    }

    void clear() override {
        data_.clear();
    }
};

TEST(ContainerTest, TestSize) {
    vectorContainer<int> container;
    EXPECT_EQ(container.size(), 0);
    container.add(1);
    container.add(2);
    EXPECT_EQ(container.size(), 2);
}

TEST(ContainerTest, TestEmpty) {
    vectorContainer<int> container;
    EXPECT_TRUE(container.empty());
    container.add(1);
    EXPECT_FALSE(container.empty());
}

TEST(ContainerTest, TestContains) {
    vectorContainer<int> container;
    container.add(1);
    container.add(2);
    EXPECT_TRUE(container.contains(1));
    EXPECT_FALSE(container.contains(3));
}

TEST(ContainerTest, TestClear) {
    vectorContainer<int> container;
    container.add(1);
    container.add(2);
    EXPECT_EQ(container.size(), 2);
    container.clear();
    EXPECT_EQ(container.size(), 0);
    EXPECT_TRUE(container.empty());
}

TEST(ContainerTest, TestEmptyContainer) {
    vectorContainer<int> container;

    EXPECT_EQ(container.size(), 0);
    EXPECT_TRUE(container.empty());

    container.clear();
    EXPECT_EQ(container.size(), 0);
    EXPECT_TRUE(container.empty());
}

TEST(ContainerTest, TestDuplicateElements) {
    vectorContainer<int> container;
    container.add(1);
    container.add(1);
    container.add(1);

    EXPECT_EQ(container.size(), 3);
    EXPECT_TRUE(container.contains(1));
}

TEST(ContainerTest, TestEmptyContainerContainsAndClear) {
    vectorContainer<int> container;

    EXPECT_FALSE(container.contains(1));

    container.clear();
    EXPECT_EQ(container.size(), 0);
}

TEST(ContainerTest, TestMaxSize) {
    vectorContainer<int> container;

    for (int i = 0; i < 1000000; ++i) {
        container.add(i);
    }

    EXPECT_EQ(container.size(), 1000000);
}

TEST(ContainerTest, TestStringData) {
    vectorContainer<std::string> container;

    std::string emptyStr;
    std::string longStr(1000, 'A');

    container.add(emptyStr);
    container.add(longStr);

    EXPECT_TRUE(container.contains(emptyStr));

    EXPECT_TRUE(container.contains(longStr));

    EXPECT_EQ(container.size(), 2);
}

TEST(ContainerTest, TestPointerData) {
    vectorContainer<int*> container;

    int a = 10;
    int* ptrA = &a;
    int* ptrNull = nullptr;

    container.add(ptrA);
    container.add(ptrNull);

    EXPECT_TRUE(container.contains(ptrA));
    EXPECT_TRUE(container.contains(ptrNull));

    container.add(nullptr);
    EXPECT_TRUE(container.contains(nullptr));
}