#include <gtest/gtest.h>
#include <type_traits>
#include "ownerPtr.h"

// 自定义删除器用于验证删除操作
template<typename TYPE>
struct TestDeleter final : original::deleterBase<TYPE>{
    ~TestDeleter() override = default;

    static int delete_count;

    void operator()(const TYPE* p) const noexcept override {
        delete p;
        delete_count++;
    }
};

template<>
struct TestDeleter<int[]> final : original::deleterBase<int[]> {
    static int delete_count;

    void operator()(const int* p) const noexcept override {
        delete[] p;
        delete_count++;
    }
};

template<>
int TestDeleter<int>::delete_count = 0;
int TestDeleter<int[]>::delete_count = 0;

// 测试基础功能
TEST(OwnerPtrTest, BasicFunctionality) {
    // 测试构造函数和对象访问
    original::ownerPtr ptr(new int(42));
    EXPECT_TRUE(ptr != nullptr);  // 检查有效性
    EXPECT_EQ(*ptr, 42);                  // 通过operator*访问值

    const original::ownerPtr ptr2(new int(42));
    EXPECT_TRUE(ptr2 != ptr);

    // 测试移动构造函数
    original::ownerPtr moved_ptr(std::move(ptr));
    EXPECT_FALSE(ptr); // 原指针应失效
    EXPECT_EQ(*moved_ptr, 42);

    // 测试移动赋值
    original::ownerPtr<int> moved_assigned = std::move(moved_ptr);
    EXPECT_FALSE(moved_ptr); // NOLINT
    EXPECT_EQ(*moved_assigned, 42);
}

// 测试工厂函数
TEST(OwnerPtrTest, FactoryFunctions) {
    auto single = original::makeOwnerPtr<int>();
    EXPECT_TRUE(single);
    EXPECT_EQ(*single, 0);
    *single = 10;
    EXPECT_EQ(*single, 10);

    // 测试数组工厂
    auto array = original::makeOwnerPtrArray<int>(5);
    array[3] = 7;       // 通过operator[]访问
    EXPECT_EQ(array[3], 7);
}

// 测试资源释放
TEST(OwnerPtrTest, ResourceManagement) {
    TestDeleter<int>::delete_count = 0;
    {
        original::ownerPtr<int, TestDeleter<int>> ptr(new int(42));
        EXPECT_EQ(TestDeleter<int>::delete_count, 0);
        EXPECT_EQ(*ptr, 42);  // 正常访问
    }
    EXPECT_EQ(TestDeleter<int>::delete_count, 1);  // 确保析构调用删除器
    TestDeleter<int[]>::delete_count = 0;
    {
        constexpr int size = 10;
        auto array = original::ownerPtr<int, TestDeleter<int[]>>(new int[size]);
        for (int i = 0; i < size; i++) {
            array[i] = i;
        }
        for (int i = 0; i < size; i++) {
            EXPECT_EQ(array[i], i);   // 正常访问
        }
    }
    EXPECT_EQ(TestDeleter<int[]>::delete_count, 1); // 确保析构调用删除器
}

// 测试解锁功能
TEST(OwnerPtrTest, UnlockOperation) {
    original::ownerPtr ptr(new int(42));
    const int* raw = ptr.unlock();
    EXPECT_EQ(*raw, 42);
    EXPECT_FALSE(ptr);  // unlock后指针失效
    delete raw; // 需要手动管理
}

TEST(OwnerPtrTest, DeletedFunctionsAssert) {
    // 测试拷贝禁止（编译时检查）
static_assert(!std::is_copy_constructible_v<original::ownerPtr<int>>,
              "ownerPtr should not be copy constructible");
static_assert(!std::is_copy_assignable_v<original::ownerPtr<int>>,
              "ownerPtr should not be copy assignable");
}

TEST(OwnerPtrTest, SwapTest) {
    original::ownerPtr ptr1(new int(42));
    original::ownerPtr ptr2(new int(24));
    EXPECT_TRUE(ptr1);
    EXPECT_TRUE(ptr2);

    ptr1.swap(ptr2);
    EXPECT_EQ(*ptr1, 24);
    EXPECT_EQ(*ptr2, 42);

    ptr2.swap(ptr1);
    EXPECT_EQ(*ptr1, 42);
    EXPECT_EQ(*ptr2, 24);
}

// 测试空指针处理
TEST(OwnerPtrTest, NullPointerHandling) {
    int* p = nullptr;
    original::ownerPtr ptr(p);
    EXPECT_FALSE(ptr);          // operator bool返回false
    EXPECT_THROW(*ptr, original::nullPointerError); // operator*抛出异常
}