#include "coroutines.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>
#include <string>

using namespace original;

// 测试基础生成器功能
TEST(GeneratorTest, BasicGenerator) {
    auto simpleGenerator = []() -> coroutine::generator<int> {
        co_yield 1;
        co_yield 2;
        co_yield 3;
    };

    auto gen = simpleGenerator();

    EXPECT_FALSE(gen.launched()); // 尚未启动

    // 测试 next() 方法
    auto val1 = gen.next();
    EXPECT_TRUE(val1.hasValue());
    EXPECT_EQ(*val1, 1);
    EXPECT_TRUE(gen.launched());

    auto val2 = gen.next();
    EXPECT_TRUE(val2.hasValue());
    EXPECT_EQ(*val2, 2);

    auto val3 = gen.next();
    EXPECT_TRUE(val3.hasValue());
    EXPECT_EQ(*val3, 3);

    auto val4 = gen.next();
    EXPECT_FALSE(val4); // 生成结束
    EXPECT_FALSE(gen.hasNext());
}

// 测试迭代器功能
TEST(GeneratorTest, IteratorTest) {
    auto numberGenerator = []() -> coroutine::generator<int> {
        for (int i = 0; i < 5; ++i) {
            co_yield i * 10;
        }
    };

    auto gen = numberGenerator();
    std::vector<int> results;

    // 使用迭代器遍历
    for (auto value : gen) {
        results.push_back(value);
    }

    EXPECT_EQ(results.size(), 5);
    EXPECT_EQ(results, std::vector({0, 10, 20, 30, 40}));
}

// 测试空生成器
TEST(GeneratorTest, EmptyGenerator) {
    auto emptyGen = []() -> coroutine::generator<int> {
        co_return;
    };

    auto gen = emptyGen();

    EXPECT_FALSE(gen.launched());
    EXPECT_TRUE(gen.hasNext());

    const auto val = gen.next();
    EXPECT_FALSE(val);

    // 测试迭代器
    int count = 0;
    for (const auto e : gen) {
        std::cout << e << std::endl;
        ++count;
    }
    EXPECT_EQ(count, 0);
}

// 测试 peek() 功能
TEST(GeneratorTest, PeekTest) {
    auto peekGenerator = []() -> coroutine::generator<std::string> {
        co_yield "first";
        co_yield "second";
    };

    auto gen = peekGenerator();

    // 在调用 next() 前 peek 应该没有值
    auto peek1 = gen.peek();
    EXPECT_FALSE(peek1);

    // 调用 next() 后 peek 应该有值
    const auto val1 = gen.next();
    EXPECT_TRUE(val1);

    auto peek2 = gen.peek();
    EXPECT_TRUE(peek2);
    EXPECT_EQ(*peek2, "first");

    gen.next();
    auto peek3 = gen.peek();
    EXPECT_TRUE(peek3);
    EXPECT_EQ(*peek3, "second");
}

// 测试异常处理
TEST(GeneratorTest, ExceptionHandling) {
    auto throwingGenerator = []() -> coroutine::generator<int> {
        co_yield 1;
        throw std::runtime_error("Test exception");
        co_yield 2; // 这行不会执行 // NOLINT
    };

    auto gen = throwingGenerator();

    // 第一个值应该正常返回
    auto val1 = gen.next();
    EXPECT_TRUE(val1);
    EXPECT_EQ(*val1, 1);

    // 第二个 next() 应该抛出异常
    EXPECT_THROW({
        gen.next();
    }, std::runtime_error);

    EXPECT_FALSE(gen.hasNext());
}

// 测试复杂类型
struct TestStruct {
    int id;
    std::string name;

    bool operator==(const TestStruct& other) const {
        return id == other.id && name == other.name;
    }
};

TEST(GeneratorTest, ComplexTypeTest) {
    auto structGenerator = []() -> coroutine::generator<TestStruct> {
        co_yield TestStruct{1, "Alice"};
        co_yield TestStruct{2, "Bob"};
        co_yield TestStruct{3, "Charlie"};
    };

    auto gen = structGenerator();
    std::vector<TestStruct> results;

    for (const auto& value : gen) {
        results.push_back(value);
    }

    EXPECT_EQ(results.size(), 3);
    EXPECT_EQ(results[0], (TestStruct{1, "Alice"}));
    EXPECT_EQ(results[1], (TestStruct{2, "Bob"}));
    EXPECT_EQ(results[2], (TestStruct{3, "Charlie"}));
}

// 测试大容量生成器
TEST(GeneratorTest, LargeGenerator) {
    auto largeGenerator = [](const int count) -> coroutine::generator<int> {
        for (int i = 0; i < count; ++i) {
            co_yield i;
        }
    };

    constexpr int TEST_SIZE = 1000;
    auto gen = largeGenerator(TEST_SIZE);

    int sum = 0;
    int expected_sum = 0;
    for (int i = 0; i < TEST_SIZE; ++i) {
        expected_sum += i;
    }

    for (const auto value : gen) {
        sum += value;
    }

    EXPECT_EQ(sum, expected_sum);
}

// 测试移动语义
TEST(GeneratorTest, MoveSemantics) {
    auto createGenerator = []() -> coroutine::generator<int> {
        co_yield 100;
        co_yield 200;
    };

    auto gen1 = createGenerator();

    // 测试移动构造
    auto gen2 = std::move(gen1);

    auto val1 = gen2.next();
    EXPECT_TRUE(val1);
    EXPECT_EQ(*val1, 100);

    // 测试移动赋值
    auto gen3 = createGenerator();
    gen3 = std::move(gen2);

    auto val2 = gen3.next();
    EXPECT_TRUE(val2);
    EXPECT_EQ(*val2, 200);
}

// 测试迭代器比较
TEST(GeneratorTest, IteratorComparison) {
    auto gen1 = []() -> coroutine::generator<int> {
        co_yield 1;
    }();

    auto gen2 = []() -> coroutine::generator<int> {
        co_yield 2;
    }();

    auto it1 = gen1.begin();
    auto it2 = it1;
    auto end1 = gen1.end();
    auto end2 = gen2.end();

    // 相同生成器的相同位置迭代器应该相等
    EXPECT_EQ(it1, it2);

    // 开始迭代器不应该等于结束迭代器
    EXPECT_NE(it1, end1);

    // 不同生成器的结束迭代器应该相等
    EXPECT_EQ(end1, end2);
}

// 测试迭代器解引用异常
TEST(GeneratorTest, IteratorDereferenceException) {
    auto gen = []() -> coroutine::generator<int> {
        co_yield 42;
    }();

    auto end_iter = gen.end();

    // 对结束迭代器解引用应该抛出异常
    EXPECT_THROW({
        *end_iter;
    }, nullPointerError);

    // 正常迭代器解引用应该工作
    auto begin_iter = gen.begin();
    EXPECT_NO_THROW({
        const int value = *begin_iter;
        EXPECT_EQ(value, 42);
    });
}

// 性能测试：确保没有内存泄漏
TEST(GeneratorTest, MemoryManagement) {
    // 创建和销毁大量生成器，确保没有内存泄漏
    for (int i = 0; i < 1000; ++i) {
        auto gen = []() -> coroutine::generator<int> {
            for (int j = 0; j < 10; ++j) {
                co_yield j;
            }
        }();

        // 消耗所有值
        while (gen.hasNext()) {
            gen.next();
        }
    }
    // 如果测试通过，说明没有内存泄漏
}