#include <gtest/gtest.h>
#include "wrapper.h"
#include "string"

using namespace original;

template <typename TYPE>
class test_wrapper : public wrapper<TYPE> {
private:
    TYPE value_;
    test_wrapper<TYPE>* prev_;
    test_wrapper<TYPE>* next_;

public:
    explicit test_wrapper(TYPE value, test_wrapper<TYPE>* prev = nullptr, test_wrapper<TYPE>* next = nullptr)
            : value_(value), prev_(prev), next_(next) {}

    void connect(test_wrapper<TYPE>* next){
        this->next_ = next;
        if (next != nullptr)
            next->prev_ = this;
    }

    TYPE& getVal() override {
        return value_;
    }

    const TYPE& getVal() const override {
        return value_;
    }

    void setVal(TYPE data) override {
        value_ = data;
    }

    test_wrapper<TYPE>* getPPrev() const override {
        return prev_;
    }

    test_wrapper<TYPE>* getPNext() const override {
        return next_;
    }

    ~test_wrapper() override = default;
};

// 测试用例 1: 测试 getVal 和 setVal
TEST(WrapperTest, GetSetValue) {
    test_wrapper<int> tw(10);  // 创建一个带有 int 值的 test_wrapper

    EXPECT_EQ(tw.getVal(), 10);  // 初始值应该是 10

    tw.setVal(20);  // 修改值
    EXPECT_EQ(tw.getVal(), 20);  // 应该是 20
}

// 测试用例 2: 测试 toString
TEST(WrapperTest, ToString) {
    test_wrapper tw(10);

    // 断言 toString 输出格式正确
    EXPECT_EQ(tw.toString(false), std::string(
            "wrapper(") + printable::formatString(&tw) + ", " + std::to_string(tw.getVal())  + ")");
}

// 测试用例 3: 测试链表结构（prev 和 next）
TEST(WrapperTest, PrevNextPointer) {
    test_wrapper<int> tw1(10);
    test_wrapper<int> tw2(20, &tw1);
    tw1.connect(&tw2);
    test_wrapper<int> tw3(30, &tw2);
    tw2.connect(&tw3);

    EXPECT_EQ(tw3.getPPrev(), &tw2);  // tw3 的前一个元素应该是 tw2
    EXPECT_EQ(tw2.getPPrev(), &tw1);  // tw2 的前一个元素应该是 tw1
    EXPECT_EQ(tw1.getPPrev(), nullptr);  // tw1 没有前一个元素
    EXPECT_EQ(tw1.getPNext(), &tw2);  // tw1 的下一个元素应该是 tw2
    EXPECT_EQ(tw2.getPNext(), &tw3);  // tw2 的下一个元素应该是 tw3
    EXPECT_EQ(tw3.getPNext(), nullptr);  // tw3 没有下一个元素
}

// 测试用例 4: 测试 className
TEST(WrapperTest, ClassName) {
    test_wrapper<int> tw(10);
    EXPECT_EQ(tw.className(), "wrapper");
}