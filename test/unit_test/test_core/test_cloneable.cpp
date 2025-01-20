#include "gtest/gtest.h"
#include "cloneable.h"

using namespace original;

template <typename DERIVED>
class crtpCloneable : public baseCloneable<DERIVED> {
public:
    int data;
    explicit crtpCloneable(int value) : data(value) {}
    ~crtpCloneable() override = default;
};

class crtpDerivedCloneable final : public crtpCloneable<crtpDerivedCloneable> {
public:
    explicit crtpDerivedCloneable(int value) : crtpCloneable(value) {}
    ~crtpDerivedCloneable() override = default;
};

class derivedCloneable : public cloneable {
public:
    int data;
    explicit derivedCloneable(int value) : data(value) {}

    [[nodiscard]] derivedCloneable *clone() const override {
        return new derivedCloneable(*this);
    }

    ~derivedCloneable() override = default;
};

TEST(CloneableTest, CloneTest) {
derivedCloneable obj(10); // 创建一个原始对象
cloneable* clonedObj = obj.clone(); // 克隆对象

// 检查克隆对象的类型是否与原对象相同
ASSERT_NE(clonedObj, nullptr);
ASSERT_NE(clonedObj, &obj); // 确保克隆对象是不同的对象

// 检查克隆对象的数据是否正确
auto* derivedClonedObj = dynamic_cast<derivedCloneable*>(clonedObj);
ASSERT_NE(derivedClonedObj, nullptr); // 检查类型转换是否成功
EXPECT_EQ(derivedClonedObj->data, 10); // 检查克隆对象的数据是否正确

// 清理内存
delete clonedObj;
}

TEST(CloneableTest, CloneableBaseTest) {
derivedCloneable baseObj(0);
auto* clonedBaseObj = baseObj.clone();

// 确保克隆没有错误
ASSERT_NE(clonedBaseObj, nullptr);
EXPECT_NE(clonedBaseObj, &baseObj); // 克隆对象与原对象不同

delete clonedBaseObj;
}

TEST(CloneableTest, CrtpCloneableTest) {
crtpDerivedCloneable obj(20); // 创建一个 CRTP 派生对象
crtpCloneable<crtpDerivedCloneable>* clonedObj = obj.clone(); // 克隆对象

// 检查克隆对象的类型是否与原对象相同
ASSERT_NE(clonedObj, nullptr);
ASSERT_NE(clonedObj, &obj); // 确保克隆对象是不同的对象

// 检查克隆对象的数据是否正确
EXPECT_EQ(clonedObj->data, 20); // 检查克隆对象的数据是否正确

// 清理内存
delete clonedObj;
}

TEST(CloneableTest, MultipleCloningTest) {
derivedCloneable obj(30); // 创建一个对象
derivedCloneable* clonedObj1 = obj.clone(); // 克隆对象 1
derivedCloneable* clonedObj2 = obj.clone(); // 克隆对象 2

// 验证两个克隆对象不相同
ASSERT_NE(clonedObj1, clonedObj2);
ASSERT_NE(clonedObj1, &obj);
ASSERT_NE(clonedObj2, &obj);

// 验证克隆对象的数据一致性
EXPECT_EQ(clonedObj1->data, 30);
EXPECT_EQ(clonedObj2->data, 30);

// 清理内存
delete clonedObj1;
delete clonedObj2;
}

