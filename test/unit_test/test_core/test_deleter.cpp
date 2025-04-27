#include <gtest/gtest.h>
#include "deleter.h"

// 测试普通对象的删除
TEST(DeleterTest, DeleteSingleObject) {
    const int* ptr = new int(42);
    const original::deleter<int> d;
    
    // 正常删除
    EXPECT_NO_THROW(d(ptr));
    
    // 验证指针已被删除（需结合valgrind手动检测内存泄漏）
    // 此处无法直接验证内存是否释放，但可确保没有崩溃
}

// 测试数组对象的删除
TEST(DeleterTest, DeleteArrayObject) {
    const int* arr = new int[5];
    const original::deleter<int[]> d;
    
    // 正常删除数组
    EXPECT_NO_THROW(d(arr));
}

// 测试删除空指针
TEST(DeleterTest, DeleteNullPointer) {
    const int* ptr = nullptr;
    const original::deleter<int> d;
    
    // 删除空指针应安全
    EXPECT_NO_THROW(d(ptr));
}
