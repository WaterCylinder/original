#include <bitset>
#include <random>

#include "bitSet.h"
#include "gtest/gtest.h"


    template <size_t N, template <typename> typename ALLOC = original::allocator>
    bool compareBitSets(const original::bitSet<ALLOC>& a, const std::bitset<N>& b)
    {
        if (a.size() != N)
        {
            std::cout << "Not equal at size of " << a.size() << " and " << N << std::endl;
            return false;
        }
        bool result = true;
        for (original::integer i = 0; i < N; ++i) {
            if (a.get(i) != b[i])
            {
                std::cout << "Not equal at index " << i << std::endl;
                result = false;
            }
        }
        return result;
    }

TEST(BitSetTest, Constructor) {
    const original::bitSet bs1(64);  // 创建一个64位的 bitSet
    constexpr std::bitset<64> bs2;  // 创建一个64位的 std::bitset

    for (size_t i = 0; i < 64; ++i) {
        ASSERT_EQ(bs1.get(i), bs2[i]);
    }
    EXPECT_TRUE(compareBitSets(bs1, bs2));
}

TEST(BitSetTest, SetAndGet) {
    original::bitSet bs1(265);
    std::bitset<265> bs2;

    bs1.set(0, true);
    bs2.set(0, true);

    bs1.set(5, true);
    bs2.set(5, true);

    ASSERT_EQ(bs1.get(0), bs2[0]);
    ASSERT_EQ(bs1.get(5), bs2[5]);
    ASSERT_EQ(bs1.get(10), bs2[10]);  // 确保未设置的位返回 false
    EXPECT_TRUE(compareBitSets(bs1, bs2));
}

TEST(BitSetTest, IndexOperator) {
    original::bitSet bs1(64);
    bs1.set(10, true);

    std::bitset<64> bs2;
    bs2.set(10, true);

    ASSERT_EQ(bs1.get(10), bs2[10]);
    EXPECT_TRUE(compareBitSets(bs1, bs2));
}

TEST(BitSetTest, BitwiseOperators) {
    constexpr original::integer SIZE = 886;
    original::bitSet bs1(SIZE);
    original::bitSet bs2(SIZE);
    std::bitset<SIZE> bs3;
    std::bitset<SIZE> bs4;
    bs1.set(0, true);
    bs1.set(5, true);
    bs2.set(5, true);
    bs2.set(10, true);
    bs1.set(220, true);
    bs1.set(225, true);
    bs2.set(225, true);
    bs2.set(510, true);
    bs3.set(0, true);
    bs3.set(5, true);
    bs4.set(5, true);
    bs4.set(10, true);
    bs3.set(220, true);
    bs3.set(225, true);
    bs4.set(225, true);
    bs4.set(510, true);

    original::bitSet result_and = bs1 & bs2;
    std::bitset std_result_and = bs3 & bs4;
    EXPECT_TRUE(compareBitSets(result_and, std_result_and));

    original::bitSet result_or = bs1 | bs2;
    std::bitset std_result_or = bs3 | bs4;
    EXPECT_TRUE(compareBitSets(result_or, std_result_or));

    original::bitSet result_xor = bs1 ^ bs2;
    std::bitset std_result_xor = bs3 ^ bs4;
    EXPECT_TRUE(compareBitSets(result_xor, std_result_xor));

    original::bitSet result_not2 = ~original::bitSet(65);
    std::bitset std_result_not2 = ~std::bitset<65>();
    EXPECT_TRUE(compareBitSets(result_not2, std_result_not2));
    ASSERT_EQ(result_not2.count(), std_result_not2.count());

    original::bitSet result_not = ~bs1;
    std::bitset std_result_not = ~bs3;
    EXPECT_TRUE(compareBitSets(result_not, std_result_not));
    ASSERT_EQ(result_not.count(), std_result_not.count());
}

TEST(BitSetTest, BitwiseMemberOperators2)
{
    constexpr original::integer SIZE = 1025;
    original::bitSet bs1(SIZE);
    original::bitSet bs2(SIZE);
    std::bitset<SIZE> bs3;
    std::bitset<SIZE> bs4;
    bs1.set(0, true);
    bs1.set(9, true);
    bs2.set(9, true);
    bs2.set(10, true);
    bs1.set(220, true);
    bs1.set(225, true);
    bs2.set(226, true);
    bs2.set(519, true);
    bs3.set(0, true);
    bs3.set(9, true);
    bs4.set(9, true);
    bs4.set(10, true);
    bs3.set(220, true);
    bs3.set(225, true);
    bs4.set(226, true);
    bs4.set(519, true);

    const original::bitSet result_and = bs1;
    bs1 &= bs2;
    const std::bitset std_result_and = bs3;
    bs3 &= bs4;
    EXPECT_TRUE(compareBitSets(result_and, std_result_and));

    const original::bitSet result_or = bs1;
    bs1 |= bs2;
    const std::bitset std_result_or = bs3;
    bs3 |= bs4;
    EXPECT_TRUE(compareBitSets(result_or, std_result_or));

    const original::bitSet result_xor = bs1;
    bs1 ^= bs2;
    const std::bitset std_result_xor = bs3;
    bs3 ^= bs4;
    EXPECT_TRUE(compareBitSets(result_xor, std_result_xor));
}

TEST(BitSetTest, Count) {
    constexpr original::integer SIZE = 2048;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, SIZE - 1);

    original::bitSet bs1(SIZE);
    std::vector<int> indexes;
    for (int i = 0; i < SIZE / 5; ++i)
    {
        int rand = dist(gen);
        indexes.emplace_back(rand);
        bs1.set(rand, true);
    }

    std::bitset<SIZE> bs2;
    for (const auto &index : indexes)
    {
        bs2.set(index, true);
    }

    EXPECT_TRUE(compareBitSets(bs1, bs2));
    ASSERT_EQ(bs1.count(), bs2.count());
}

TEST(BitSetTest, Resize) {
    constexpr original::integer SIZE1 = 218;
    constexpr original::integer SIZE2 = 1024;
    original::bitSet bs1(SIZE1);
    bs1.set(0, true);
    bs1.set(5, true);
    bs1.set(75, true);
    bs1.set(110, true);

    const original::bitSet bs_resized = bs1.resize(SIZE2);

    ASSERT_EQ(bs_resized.size(), SIZE2);
    ASSERT_EQ(bs_resized.get(0), true);
    ASSERT_EQ(bs_resized.get(5), true);
    ASSERT_EQ(bs_resized.get(75), true);
    ASSERT_EQ(bs_resized.get(110), true);
    ASSERT_EQ(bs_resized.get(-10), false);
    ASSERT_EQ(bs_resized.get(-1), false);
}

TEST(BitSetTest, Iterator) {
    original::bitSet bs1(64);
    bs1.set(0, true);
    bs1.set(5, true);
    bs1.set(10, true);

    const auto it = bs1.begin();
    const auto& it1 = it;
    const auto it2 = it + 1;
    const auto it3 = it + 5;
    const auto it4 = it + 10;

    ASSERT_EQ(it1.getElem(), true);  // 0位
    ASSERT_EQ(it2->getElem(), false);  // 1位
    ASSERT_EQ(it3->getElem(), true);  // 5位
    ASSERT_EQ(it4->getElem(), true);  // 10位
}
