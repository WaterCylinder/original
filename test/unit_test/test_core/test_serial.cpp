#include "gtest/gtest.h"
#include "serial.h"
#include "container.h"
#include "error.h"

namespace original {

    template <typename TYPE>
    class vectorSerial : public serial<TYPE> {
    private:
        std::vector<TYPE> data;

    public:
        vectorSerial() = default;
        ~vectorSerial() override = default;

        [[nodiscard]] uint32_t size() const override {
            return static_cast<uint32_t>(data.size());
        }

        void set(int64_t index, const TYPE& e) override {
            if (this->indexOutOfBound(index)) {
                throw outOfBoundError();
            }
            index = this->parseNegIndex(index);
            data[index] = e;
        }

        [[nodiscard]] TYPE get(int64_t index) const override {
            if (this->indexOutOfBound(index)) {
                throw outOfBoundError();
            }
            index = this->parseNegIndex(index);
            return data[index];
        }

        void add(const TYPE& e) override {
            this->pushEnd(e);
        }

        void clear() override {
            data.clear();
        }

        TYPE operator[](int64_t index) const override {
            return this->get(index);
        }

        TYPE& operator[](int64_t index) override {
            if (this->indexOutOfBound(index)) {
                throw outOfBoundError();
            }
            index = this->parseNegIndex(index);
            return data[index];
        }

        [[nodiscard]] uint32_t indexOf(const TYPE& e) const override {
            for (uint32_t i = 0; i < data.size(); ++i) {
                if (data[i] == e) {
                    return i;
                }
            }
            return data.size();
        }

        void push(int64_t index, const TYPE& e) override {
            if (this->parseNegIndex(index) == this->size()){
                this->pushEnd(e);
                return;
            }
            if (this->indexOutOfBound(index)) {
                throw outOfBoundError();
            }
            index = this->parseNegIndex(index);
            data.insert(data.begin() + index, e);
        }

        TYPE pop(int64_t index) override {
            if (this->indexOutOfBound(index)) {
                throw outOfBoundError();
            }
            index = this->parseNegIndex(index);
            TYPE popped = data[index];
            data.erase(data.begin() + index);
            return popped;
        }

        void pushBegin(const TYPE& e) override {
            data.insert(data.begin(), e);
        }

        TYPE popBegin() override {
            if (data.empty()) {
                throw noElementError();
            }
            TYPE popped = data.front();
            data.erase(data.begin());
            return popped;
        }

        void pushEnd(const TYPE& e) override {
            data.push_back(e);
        }

        TYPE popEnd() override {
            if (data.empty()) {
                throw outOfBoundError();
            }
            TYPE popped = data.back();
            data.pop_back();
            return popped;
        }
    };
}

using namespace original;

// 测试添加元素
TEST(SerialTest, AddTest) {
    vectorSerial<int> serial;
    serial.add(5);
    EXPECT_EQ(serial.size(), 1);
    EXPECT_EQ(serial[0], 5);
}

// 测试清空容器
TEST(SerialTest, ClearTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    serial.clear();
    EXPECT_EQ(serial.size(), 0);
}

// 测试容器是否包含特定元素
TEST(SerialTest, ContainsTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    EXPECT_TRUE(serial.contains(5));
    EXPECT_FALSE(serial.contains(20));
}

// 测试获取第一个元素
TEST(SerialTest, GetBeginTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    EXPECT_EQ(serial.getBegin(), 5);
}

// 测试获取最后一个元素
TEST(SerialTest, GetEndTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    EXPECT_EQ(serial.getEnd(), 10);
}

// 测试通过下标操作元素
TEST(SerialTest, IndexOperatorTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    EXPECT_EQ(serial[0], 5);
    EXPECT_EQ(serial[1], 10);
}

// 测试通过负索引操作元素
TEST(SerialTest, NegativeIndexOperatorTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    EXPECT_EQ(serial[-1], 10);
    EXPECT_EQ(serial[-2], 5);
}

// 测试超出边界的下标访问
TEST(SerialTest, OutOfBoundTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);

    // 正常下标访问
    EXPECT_NO_THROW(serial[0]);
    EXPECT_NO_THROW(serial[1]);

    // 非法下标访问
    EXPECT_THROW(serial[2], outOfBoundError);
    EXPECT_THROW(serial[-3], outOfBoundError);
}

// 测试对不存在的元素进行 `indexOf` 查询
TEST(SerialTest, IndexOfTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);

    EXPECT_EQ(serial.indexOf(5), 0);
    EXPECT_EQ(serial.indexOf(10), 1);
    EXPECT_EQ(serial.indexOf(20), 2);  // 如果元素不存在，返回size
}

// 测试 `getBegin` 和 `getEnd` 在空容器中的行为
TEST(SerialTest, GetBeginEndEmptyTest) {
    vectorSerial<int> serial;

    EXPECT_THROW(serial.getBegin(), outOfBoundError);
    EXPECT_THROW(serial.getEnd(), outOfBoundError);
}

// 测试 push 方法
TEST(SerialTest, PushMethodTest) {
    vectorSerial<int> serial;

    // 向容器中间插入元素
    serial.push(0, 5);  // 插入到第一个位置
    EXPECT_EQ(serial.size(), 1);
    EXPECT_EQ(serial[0], 5);

    serial.push(1, 10);  // 插入到末尾
    EXPECT_EQ(serial.size(), 2);
    EXPECT_EQ(serial[1], 10);

    serial.push(1, 7);  // 插入到第二个位置
    EXPECT_EQ(serial.size(), 3);
    EXPECT_EQ(serial[1], 7);
    EXPECT_EQ(serial[2], 10);
}

// 测试 pop 方法
TEST(SerialTest, PopMethodTest) {
    vectorSerial<int> serial;

    serial.add(5);
    serial.add(10);
    serial.add(15);

    // 从中间弹出
    EXPECT_EQ(serial.pop(1), 10);  // 弹出第二个元素
    EXPECT_EQ(serial.size(), 2);
    EXPECT_EQ(serial[0], 5);
    EXPECT_EQ(serial[1], 15);

    // 从末尾弹出
    EXPECT_EQ(serial.popEnd(), 15);
    EXPECT_EQ(serial.size(), 1);
    EXPECT_EQ(serial[0], 5);

    // 从开头弹出
    EXPECT_EQ(serial.popBegin(), 5);
    EXPECT_EQ(serial.size(), 0);
}

// 测试 `clear` 对空容器的行为
TEST(SerialTest, ClearEmptyTest) {
    vectorSerial<int> serial;
    serial.clear();  // 对空容器调用 should not crash
    EXPECT_EQ(serial.size(), 0);
}

// 测试容器包含多个相同元素的情况
TEST(SerialTest, MultipleSameElementsTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(5);
    serial.add(5);

    EXPECT_EQ(serial.indexOf(5), 0);  // 只返回第一个位置
    EXPECT_EQ(serial.size(), 3);
}

// 测试负数索引和正数索引的一致性
TEST(SerialTest, NegativeIndexConsistencyTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    serial.add(15);

    EXPECT_EQ(serial[-1], 15);
    EXPECT_EQ(serial[-2], 10);
    EXPECT_EQ(serial[-3], 5);
}

// 测试中间插入和删除操作
TEST(SerialTest, MiddlePushPopTest) {
    vectorSerial<int> serial;
    serial.add(5);
    serial.add(10);
    serial.add(15);

    // 插入到中间
    serial.push(1, 7);
    EXPECT_EQ(serial[1], 7);
    EXPECT_EQ(serial.size(), 4);

    // 删除中间元素
    int popped = serial.pop(1);
    EXPECT_EQ(popped, 7);
    EXPECT_EQ(serial.size(), 3);
}