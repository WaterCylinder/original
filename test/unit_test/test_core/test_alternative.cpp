#include <gtest/gtest.h>
#include "optional.h"
#include "vector.h"

using namespace original;

// 用于测试构造和析构是否触发
struct Tracker {
    static int constructed;
    static int destroyed;
    Tracker() { ++constructed; }
    ~Tracker() { ++destroyed; }
};

int Tracker::constructed = 0;
int Tracker::destroyed = 0;

// 用于测试移动语义
struct MoveTracker {
    bool moved = false;
    MoveTracker() = default;
    MoveTracker(MoveTracker&& other) noexcept { other.moved = true; }
    MoveTracker& operator=(MoveTracker&& other) noexcept {
        other.moved = true;
        return *this;
    }
};

// 用于测试资源管理
struct Resource {
    static constexpr int val = 100;
    int* ptr;
    Resource() : ptr(new int(val)) {}
    ~Resource() { delete ptr; }
};

TEST(AlternativeTest, DefaultConstructedIsNone) {
    const alternative<int> a;
    EXPECT_FALSE(a.hasValue());
}

TEST(AlternativeTest, ConstructWithValue) {
    alternative<std::string> a("hello");
    EXPECT_TRUE(a.hasValue());
    EXPECT_EQ(*a, "hello");
}

TEST(AlternativeTest, CopyConstructed) {
    const alternative<int> a(42);
    alternative b(a);
    EXPECT_TRUE(b.hasValue());
    *b += 1;
    EXPECT_EQ(*b, 43);
}

TEST(AlternativeTest, MoveConstructed) {
    alternative<std::string> a("world");
    alternative b(std::move(a));
    EXPECT_TRUE(b.hasValue());
    EXPECT_EQ(*b, "world");
}

TEST(AlternativeTest, DestructorCalledProperly) {
    {
        Tracker::constructed = 0;
        Tracker::destroyed = 0;
        alternative<Tracker> a;
        EXPECT_EQ(Tracker::constructed, 0);
    }
    EXPECT_EQ(Tracker::destroyed, 0);
    {
        alternative<Tracker> a;
        a.emplace();
        EXPECT_EQ(Tracker::constructed, 1);
    }
    EXPECT_EQ(Tracker::destroyed, 1);
}

TEST(AlternativeTest, CopyAssignment) {
    const alternative<int> a(10);
    alternative<int> b = a; // NOLINT: Test copy assignment here
    EXPECT_TRUE(b.hasValue());
    EXPECT_EQ(*b, 10);
}

TEST(AlternativeTest, MoveAssignment) {
    alternative<std::string> a("test");
    alternative<std::string> b = std::move(a);
    EXPECT_TRUE(b.hasValue());
    EXPECT_EQ(*b, "test");
}

TEST(AlternativeTest, SelfAssignment) {
    alternative<int> a(5);
    a = a; // NOLINT: Test self-assignment here
    EXPECT_TRUE(a.hasValue());
    EXPECT_EQ(*a, 5);
}

TEST(AlternativeTest, ValueModification) {
    alternative<int> a(1);
    *a = 2;
    EXPECT_EQ(*a, 2);
    a.set(3);
    EXPECT_EQ(*a, 3);
}

TEST(AlternativeTest, ConstAccess) {
    const alternative<int> a(42);
    EXPECT_EQ(*a, 42);
    EXPECT_EQ(*a.get(), 42);
}

TEST(AlternativeTest, ArrowOperator) {
    struct Test { int x; };
    alternative<Test> a(Test{10});
    EXPECT_EQ(a->x, 10);
}

TEST(AlternativeTest, ResetToNone) {
    alternative<int> a(5);
    a.reset();
    EXPECT_FALSE(a.hasValue());
    EXPECT_EQ(a.get(), nullptr);
}

TEST(AlternativeTest, EmplaceOverwrites) {
    alternative<std::string> a("old");
    a.emplace("new");
    EXPECT_EQ(*a, "new");
}

TEST(AlternativeTest, SetOverwrites) {
    alternative<int> a;
    constexpr int x = 10;
    a.set(x);
    EXPECT_EQ(*a, 10);
}

TEST(AlternativeTest, DereferenceNoneThrows) {
    alternative<int> a;
    EXPECT_THROW(*a, valueError);
    EXPECT_THROW(a.operator->(), valueError);
}

TEST(AlternativeTest, ConstDereferenceNoneThrows) {
    const alternative<int> a;
    EXPECT_THROW(*a, valueError);
    EXPECT_THROW(a.operator->(), valueError);
}

TEST(AlternativeTest, BoolConversion) {
    alternative<int> a;
    EXPECT_FALSE(a);
    a.emplace(1);
    EXPECT_TRUE(a);
}

TEST(AlternativeTest, ComplexTypeWithDestructor) {
    {
        alternative<Resource> a;
        a.emplace();
        EXPECT_EQ(*a->ptr, 100);
    } // Should properly destroy the Resource
}

TEST(AlternativeTest, VerifyMoveSemantics) {
    alternative<MoveTracker> a;
    a.emplace();
    alternative b(std::move(a));
    EXPECT_TRUE(a->moved);

    alternative<MoveTracker> c;
    EXPECT_FALSE(c);
    c = std::move(b);
    EXPECT_TRUE(c);
    EXPECT_TRUE(b->moved);

    alternative<vector<int>> vec{1, 2, 3, 4};
    EXPECT_TRUE(vec);
    EXPECT_EQ(vec->size(), 4);
    vec->pushEnd(5);
    EXPECT_EQ(vec->size(), 5);
    EXPECT_EQ((*vec)[4], 5);
    vec.reset();
    EXPECT_FALSE(vec);
}

TEST(AlternativeTest, HasValueMethod) {
    alternative<int> a;
    EXPECT_FALSE(a.hasValue());
    a.emplace(42);
    EXPECT_TRUE(a.hasValue());
}