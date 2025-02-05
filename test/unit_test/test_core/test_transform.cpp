#include <gtest/gtest.h>
#include "transform.h"
#include "vector.h"

namespace original {

    // 测试 addOptTransform 类
    TEST(TransformTest, AddOptTransformTest) {
        addOptTransform addTransform(10);
        int value = 5;

        addTransform(value);  // 应用 add 变换
        EXPECT_EQ(value, 15);  // 5 + 10 = 15
    }

    // 测试 assignOptTransform 类
    TEST(TransformTest, AssignOptTransformTest) {
        assignOptTransform assignTransform(10);
        int value = 5;

        assignTransform(value);  // 应用 assign 变换
        EXPECT_EQ(value, 10);  // 应该赋值为 10
    }

    // 测试 multiOptTransform 类
    TEST(TransformTest, MultiOptTransformTest) {
        multiOptTransform multiTransform(2);
        int value = 5;

        multiTransform(value);  // 应用乘法变换
        EXPECT_EQ(value, 10);  // 5 * 2 = 10
    }

    // 测试 absTransform 类
    TEST(TransformTest, AbsTransformTest) {
        absTransform<int> absTransform;
        int value = -5;

        absTransform(value);  // 应用绝对值变换
        EXPECT_EQ(value, 5);  // -5 应该变为 5
    }

    // 测试 copyTransform 类
    TEST(TransformTest, CopyTransformTest) {
        vector<int> v;
        copyTransform copyTransform(v);
        int value = 5;

        copyTransform(value);  // 应用复制变换
        EXPECT_EQ(v.size(), 1);  // 应该将元素添加到容器中
        EXPECT_EQ(v.get(0), 5);  // 容器中的元素应该是 5
    }

    // 测试 clone 方法
    TEST(TransformTest, CloneTest) {
        addOptTransform addTransform(10);
        const auto clonedTransform = addTransform.clone();

        int value = 5;
        addTransform(value);  // 应用 add 变换
        EXPECT_EQ(value, 15);  // 5 + 10 = 15

        value = 5;
        clonedTransform->operator()(value);  // 使用克隆的转换器
        EXPECT_EQ(value, 15);  // 克隆的转换器也应当使值为 15

        delete clonedTransform;  // 清理克隆的转换器
    }

}

