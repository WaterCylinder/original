#include <gtest/gtest.h>
#include "hash.h"
#include <string>
#include <cstdint>

using namespace original;

// Test class that inherits from hashable
class TestHashable : public hashable<TestHashable> {
public:
    int value;

    explicit TestHashable(int v) : value(v) {}

    bool operator==(const TestHashable& other) const {
        return value == other.value;
    }

    [[nodiscard]] u_integer toHash() const noexcept override {
        return hash<int>::hashFunc(value);
    }
};

// Trivially copyable test struct
struct TestStruct {
    int a;
    [[maybe_unused]] float b;
    char c;
};

// Test fixture for hash tests
class HashTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize any common test data here
    }
};

// Test FNV1a algorithm
TEST_F(HashTest, FNV1aAlgorithm) {
const char* testStr = "hello";
auto size = std::strlen(testStr);
auto expected = hash<int>::FNV_OFFSET_BASIS;
for (u_integer i = 0; i < size; ++i) {
expected ^= testStr[i];
expected *= hash<int>::FNV_32_PRIME;
}

auto actual = hash<int>::fnv1a(reinterpret_cast<const byte*>(testStr), size);
EXPECT_EQ(expected, actual);
}

// Test integral types
TEST_F(HashTest, IntegralTypes) {
EXPECT_EQ(42u, hash<int>::hashFunc(42));
EXPECT_EQ(42u, hash<int>::hashFunc(42u));
EXPECT_EQ(42u, hash<int>::hashFunc(static_cast<int8_t>(42)));
EXPECT_EQ(42u, hash<int>::hashFunc(static_cast<int16_t>(42)));
EXPECT_EQ(42u, hash<int>::hashFunc(static_cast<int64_t>(42)));
}

// Test pointer types
TEST_F(HashTest, PointerTypes) {
int x = 42;
int* ptr = &x;
auto expected = reinterpret_cast<uintptr_t>(ptr);
EXPECT_EQ(expected, hash<int*>::hashFunc(ptr));
EXPECT_EQ(0u, hash<int*>::hashFunc(nullptr));
}

// Test character types
TEST_F(HashTest, CharacterTypes) {
EXPECT_EQ(static_cast<u_integer>('a'), hash<char>::hashFunc('a'));
EXPECT_EQ(static_cast<u_integer>('\0'), hash<char>::hashFunc('\0'));
EXPECT_EQ(static_cast<u_integer>(65), hash<char>::hashFunc('A'));
}

// Test C-string types
TEST_F(HashTest, CStringTypes) {
const char* str = "test";
auto expected = hash<int>::fnv1a(reinterpret_cast<const byte*>(str), std::strlen(str));
EXPECT_EQ(expected, hash<const char*>::hashFunc(str));
EXPECT_EQ(0u, hash<const char*>::hashFunc(nullptr));
EXPECT_EQ(hash<int>::FNV_OFFSET_BASIS, hash<const char*>::hashFunc(""));
}

// Test std::string types
TEST_F(HashTest, StdStringTypes) {
std::string str = "test";
auto expected = hash<int>::fnv1a(reinterpret_cast<const byte*>(str.data()), str.size());
EXPECT_EQ(expected, hash<std::string>::hashFunc(str));
EXPECT_EQ(hash<int>::FNV_OFFSET_BASIS, hash<std::string>::hashFunc(std::string()));
}

// Test trivially copyable types
TEST_F(HashTest, TriviallyCopyableTypes) {
TestStruct ts{42, 3.14f, 'a'};
byte buffer[sizeof(TestStruct)];
std::memcpy(buffer, &ts, sizeof(TestStruct));
auto expected = hash<int>::fnv1a(buffer, sizeof(TestStruct));
EXPECT_EQ(expected, hash<TestStruct>::hashFunc(ts));
}

// Test hashable objects
TEST_F(HashTest, HashableObjects) {
TestHashable th1(42);
TestHashable th2(42);
TestHashable th3(100);

// Test toHash()
auto hash1 = th1.toHash();
auto hash2 = th2.toHash();
auto hash3 = th3.toHash();

EXPECT_EQ(hash1, hash2);
EXPECT_NE(hash1, hash3);

// Test equals()
EXPECT_TRUE(th1.equals(th2));
EXPECT_FALSE(th1.equals(th3));

// Test hash function specialization for hashable types
EXPECT_EQ(hash1, hash<TestHashable>::hashFunc(th1));
}

// Test hash functor
TEST_F(HashTest, HashFunctor) {
hash<std::string> hasher;
std::string str = "functor test";
auto expected = hash<std::string>::hashFunc(str);
EXPECT_EQ(expected, hasher(str));
}

// Test edge cases
TEST_F(HashTest, EdgeCases) {
// Empty string
EXPECT_EQ(hash<int>::FNV_OFFSET_BASIS, hash<std::string>::hashFunc(std::string()));

// Zero value
EXPECT_EQ(0u, hash<int>::hashFunc(0));

// Maximum value
EXPECT_EQ(static_cast<u_integer>(std::numeric_limits<int>::max()),
        hash<int>::hashFunc(std::numeric_limits<int>::max()));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}