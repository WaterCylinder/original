#include "gtest/gtest.h"
#include "iterator.h"

namespace original {

    template <typename TYPE>
    class array {
    private:
        TYPE* data_;
        size_t size_;

    public:
        class arrayIterator : public baseIterator<TYPE> {
        private:
            mutable TYPE* data_;
            mutable int64_t index_;
            mutable size_t max_size;
        public:
            arrayIterator(TYPE* data, int64_t index, size_t size_) : data_(data), index_(index), max_size(size_) {}

            bool equalPtr(const iterator<TYPE>* other) const override {
                auto* otherIt = dynamic_cast<const arrayIterator*>(other);
                return otherIt && this->data_ == otherIt->data_ && this->index_ == otherIt->index_;
            }

            TYPE& get() override {
                return data_[index_];
            }

            TYPE get() const override {
                return data_[index_];
            }

            void set(const TYPE& value) override {
                data_[index_] = value;
            }

            void next() const override {
                this->index_ += 1;
            }

            void prev() const override {
                index_ -= 1;
            }

            [[nodiscard]] bool hasNext() const override {
                return index_ < max_size - 1;
            }

            [[nodiscard]] bool hasPrev() const override {
                return index_ > 0;
            }

            bool atPrev(const iterator<TYPE>* other) const override {
                auto* other_it = dynamic_cast<const arrayIterator*>(other);
                return other_it && this->index_ == other_it->index_ - 1;
            }

            bool atNext(const iterator<TYPE>* other) const override {
                auto* other_it = dynamic_cast<const arrayIterator*>(other);
                return other_it && this->index_ == other_it->index_ + 1;
            }

            void operator+=(int64_t steps) const override {
                index_ += steps;
            }

            void operator-=(int64_t steps) const override {
                index_ -= steps;
            }

            int64_t operator-(const iterator<TYPE>& other) const override {
                auto* o = dynamic_cast<const arrayIterator*>(&other);
                if (!o)
                    return std::numeric_limits<int64_t>::max();
                return this->index_ - o->index_;
            }

            arrayIterator* clone() const override {
                return new arrayIterator(data_, index_, max_size);
            }

            [[nodiscard]] bool isValid() const override {
                return index_ >= 0 && index_ + 1 <= max_size;
            }

            [[nodiscard]] std::string className() const override {
                return "arrayIterator";
            }

            [[nodiscard]] std::string toString(bool enter) const override {
                std::stringstream ss;
                ss << this->className() << "(" << get() << ")";
                if (enter) ss << "\n";
                return ss.str();
            }

            iterator<TYPE>* getPrev() const override {
                return nullptr;
            }
        };

        explicit array(size_t size) : size_(size) {
            data_ = new TYPE[size_];
        }

        ~array() {
            delete[] data_;
        }

        [[nodiscard]] size_t size() const {
            return size_;
        }

        TYPE& operator[](size_t index) {
            if (index >= size_) throw std::out_of_range("Index out of range");
            return data_[index];
        }

        TYPE operator[](size_t index) const {
            if (index >= size_) throw std::out_of_range("Index out of range");
            return data_[index];
        }

        arrayIterator begin() {
            return arrayIterator(this->data_, 0, this->size_);
        }

        arrayIterator end() {
            return arrayIterator(this->data_, this->size_, this->size_);
        }
    };

    // 测试 array 类构造、访问和迭代器
    TEST(ArrayTest, ArrayConstructorAndAccess) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);  // 设置一些值
        }

        // 检查数组访问
        for (size_t i = 0; i < arr.size(); ++i) {
            EXPECT_EQ(arr[i], static_cast<int>(i * 2));
        }
    }

    // 测试 array 的迭代器
    TEST(ArrayTest, ArrayIterator) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        EXPECT_EQ(*it, 0);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 4);
        --it;
        EXPECT_EQ(*it, 2);

        auto it2 = arr.begin();
        ++it2;
        EXPECT_TRUE(it == it2);

        auto it3 = arr.end();
        --it3;
        EXPECT_EQ(*it3, 8);
    }

    // 测试迭代器加法与减法
    TEST(IteratorTest, IteratorPlusMinus) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        auto it1 = it + 3;
        EXPECT_EQ(**it1, 6);

        auto it2 = *it1 - 2;
        EXPECT_EQ(**it2, 2);
    }

    // 测试迭代器的相等与不相等
    TEST(IteratorTest, IteratorEquality) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it1 = arr.begin();
        auto it2 = arr.begin();
        auto it3 = arr.begin();
        ++it3;

        EXPECT_TRUE(it1 == it2);
        EXPECT_FALSE(it1 == it3);
        EXPECT_TRUE(it1 != it3);
    }

    // 测试迭代器是否有效
    TEST(IteratorTest, IteratorIsValid) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        EXPECT_TRUE(it);
        ++it;
        EXPECT_TRUE(it);
        it = arr.end();
        EXPECT_FALSE(it);
    }

    // 测试空数组的迭代器行为
    TEST(ArrayTest, EmptyArrayIterator) {
        array<int> arr(0);  // 创建空数组

        auto it = arr.begin();
        auto it_end = arr.end();

        // 空数组的 begin() 和 end() 应该相等
        EXPECT_TRUE(it == it_end);

        // 迭代器不应有效
        EXPECT_FALSE(it.isValid());
    }

    // 测试反向迭代器的行为
    TEST(ArrayTest, ReverseIterator) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        // 假设你有一个 reverseIterator 方法（这只是示例）
        auto rit = arr.end() - 1;
        EXPECT_EQ(**rit, 8);
        --*rit;
        EXPECT_EQ(**rit, 6);
        --*rit;
        EXPECT_EQ(**rit, 4);
        --*rit;
        EXPECT_EQ(**rit, 2);
        --*rit;
        EXPECT_EQ(**rit, 0);
    }

    // 测试迭代器越界行为
    TEST(ArrayTest, IteratorOutOfBounds) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        // 迭代到越界
        ++it;
        ++it;
        ++it;
        ++it;
        ++it; // 越过数组

        // 检查越界后，迭代器无效
        EXPECT_FALSE(it.isValid());
    }

    // 测试迭代器赋值
    TEST(ArrayTest, IteratorAssignment) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it1 = arr.begin();
        auto it2 = it1;  // 复制构造或赋值

        EXPECT_EQ(*it1, *it2);  // 两个迭代器应该指向同一元素
    }

    // 测试 operator[] 和迭代器的配合使用
    TEST(ArrayTest, IteratorAndSubscriptCompatibility) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        // 使用迭代器与 operator[] 一起访问数组元素
        EXPECT_EQ(arr[0], *it);  // 0
        ++it;
        EXPECT_EQ(arr[1], *it);  // 2
        ++it;
        EXPECT_EQ(arr[2], *it);  // 4
        ++it;
        EXPECT_EQ(arr[3], *it);  // 6
        ++it;
        EXPECT_EQ(arr[4], *it);  // 8
    }

    // 测试在容器被修改时迭代器的行为
    TEST(ArrayTest, IteratorAfterModification) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        EXPECT_EQ(*it, 0);

        arr[0] = 42;  // 修改元素
        EXPECT_EQ(*it, 42);  // 迭代器应正确反映修改后的值
    }

    // 测试随机访问迭代器
    TEST(ArrayTest, RandomAccessIterator) {
        array<int> arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }

        auto it = arr.begin();
        EXPECT_EQ(**(it + 2), 4);  // 随机访问到索引2
        EXPECT_EQ(**(it + 4), 8);  // 随机访问到索引4
    }

} // namespace original
