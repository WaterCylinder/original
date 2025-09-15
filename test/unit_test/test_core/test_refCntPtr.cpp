#include <gtest/gtest.h>
#include "refCntPtr.h"

// 自定义测试对象用于追踪构造/析构次数
class TrackedObject {
public:
    static int alive_count;
    int id;

    TrackedObject() :TrackedObject(0) {}
    explicit TrackedObject(const int i) : id(i) { alive_count++; }
    ~TrackedObject() { alive_count--; }
};
int TrackedObject::alive_count = 0;

// 测试强引用基础功能
TEST(RefCntPtrTest, StrongPtrBasic) {
    TrackedObject::alive_count = 0;
    {
        original::strongPtr ptr1(new TrackedObject(10));
        EXPECT_EQ(ptr1->id, 10);
        EXPECT_EQ(TrackedObject::alive_count, 1);

        // 拷贝构造增加引用计数
        auto ptr2 = ptr1;
        EXPECT_EQ(ptr2->id, 10);
        EXPECT_EQ(TrackedObject::alive_count, 1);
        constexpr int id = 5;
        ptr2->id = id;
        EXPECT_EQ(ptr1->id, id);
        EXPECT_FALSE(ptr1 == nullptr);
    }
    EXPECT_EQ(TrackedObject::alive_count, 0); // 确保完全释放
}

// 测试弱引用基础功能
TEST(RefCntPtrTest, WeakPtrBasic) {
    const original::strongPtr strong(new TrackedObject(20));
    const auto weak = original::weakPtr(strong); // 通过强引用构造弱引用

    // 验证弱引用有效性
    auto locked = weak.lock();
    EXPECT_TRUE(locked);
    EXPECT_EQ(locked->id, 20);
}

// 测试移动语义
TEST(RefCntPtrTest, MoveSemantics) {
    original::strongPtr ptr1(new TrackedObject(30));
    auto ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr2->id, 30);
    EXPECT_FALSE(ptr1); // 移动后原指针为空
}

// 测试数组支持
TEST(RefCntPtrTest, ArraySupport) {
    auto arr = original::makeStrongPtrArray<TrackedObject>(3); // 创建含3个元素的数组
    arr[0].id = 1;
    arr[1].id = 2;
    arr[2].id = 3;
    EXPECT_EQ(arr[0].id, 1);
    EXPECT_EQ(arr[1].id, 2);
}

// 测试异常安全
TEST(RefCntPtrTest, ExceptionSafety) {
    original::strongPtr<TrackedObject> empty_ptr;
    original::strongPtr<TrackedObject> target_ptr = std::move(empty_ptr);
    EXPECT_THROW({
        std::cout << empty_ptr->id << std::endl; // 访问空指针
    }, original::nullPointerError);
}

TEST(RefCntPtrTest, LockAndResetTest) {
    TrackedObject::alive_count = 0;
    {
        auto p1 = original::strongPtr(new TrackedObject(10));
        EXPECT_EQ(TrackedObject::alive_count, 1);
        EXPECT_EQ(p1->id, 10);

        auto p2 = p1;
        p2->id = 20;
        EXPECT_EQ(p1->id, 20);

        auto p3 = original::weakPtr(p2);
        EXPECT_EQ(p3.strongRefs(), 2);
        EXPECT_EQ(p3.weakRefs(), 1);
        EXPECT_EQ(p3->id, 20);

        const auto p4 = p3.lock();
        EXPECT_EQ(p4.strongRefs(), 3);
        EXPECT_EQ(p4.weakRefs(), 1);

        p1.reset();
        EXPECT_FALSE(p1);
        EXPECT_EQ(p2.strongRefs(), 2);
        EXPECT_EQ(p2.weakRefs(), 1);
    }
    EXPECT_EQ(TrackedObject::alive_count, 0);
}

TEST(RefCntPtrTest, SwapTest) {
    TrackedObject::alive_count = 0;
    {
        auto p1 = original::strongPtr(new TrackedObject(10));
        EXPECT_EQ(TrackedObject::alive_count, 1);
        auto p2 = original::strongPtr(new TrackedObject(20));
        EXPECT_EQ(TrackedObject::alive_count, 2);

        auto p3 = original::weakPtr(p1);
        auto p4 = original::weakPtr(p2);
        EXPECT_EQ(TrackedObject::alive_count, 2);

        p3.swap(p4);
        EXPECT_EQ(p1->id, 10);
        EXPECT_EQ(p2->id, 20);
        EXPECT_EQ(p3->id, 20);
        EXPECT_EQ(p4->id, 10);

        p1.swap(p2);
        EXPECT_EQ(p1->id, 20);
        EXPECT_EQ(p2->id, 10);
        EXPECT_EQ(p3->id, 20);
        EXPECT_EQ(p4->id, 10);
    }
    EXPECT_EQ(TrackedObject::alive_count, 0);
}

// 定义包含循环引用的 Node 类
struct Node {
    original::strongPtr<Node> next;  // 强引用
    original::weakPtr<Node> prev;    // 弱引用，用于打破循环
    int value;

    explicit Node(const int val = 0) : value(val) {
    }
};

struct CNode {
    original::weakPtr<CNode> next;  // 弱引用
    original::weakPtr<CNode> prev;    // 弱引用
    int value;

    explicit CNode(const int val = 0) : value(val) {
    }
};

TEST(RefCntPtrTest, BreakCyclicReference) {
    {
        auto node1 = original::makeStrongPtr<Node>(1);
        auto node2 = original::makeStrongPtr<Node>(2);
    }

    {
        auto node1 = original::makeStrongPtr<Node>(1);
        auto node2 = original::makeStrongPtr<Node>(2);

        node1->next = node2;  // 强引用
        node2->prev = node1;  // 弱引用
    }

    {
        auto node1 = original::makeStrongPtr<Node>(1);
        auto node2 = original::makeStrongPtr<Node>(2);

        node1->prev = node2;  // 弱引用
        node2->prev = node1;  // 弱引用
    }

    {
        auto node1 = original::makeStrongPtr<Node>(1);
        auto node2 = original::makeStrongPtr<Node>(2);

        node1->next = node2;  // node2 强引用
        node2->next = node1;  // node1 强引用


        node1->next.reset();
        node2->prev = node1;  // node1弱引用
    }

    {
        auto node1 = original::makeStrongPtr<CNode>(1);
        auto node2 = original::makeStrongPtr<CNode>(2);

        node1->prev = node2;
        node1->next = node2;
        node2->prev = node1;
        node2->next = node1;
    }
}

// 基类和派生类用于测试类型转换
struct Base {
    virtual ~Base() = default; // 必须虚析构以支持 dynamic_cast
    int base_val = 0;
};
struct Derived final : Base {
    int derived_val = 0;
};

TEST(RefCntPtrTest, StaticCastTo) {
    auto d = original::makeStrongPtr<Derived>();
    d->base_val = 42;
    d->derived_val = 100;

    // Derived -> Base
    auto b = d.staticCastTo<Base>();
    EXPECT_EQ(b->base_val, 42);

    // 修改 base，两个指针共享同一对象
    b->base_val = 99;
    EXPECT_EQ(d->base_val, 99);

    // 引用计数一致
    EXPECT_EQ(d.strongRefs(), b.strongRefs());
}

TEST(RefCntPtrTest, DynamicCastToSuccess) {
    auto d = original::makeStrongPtr<Derived>();
    d->derived_val = 123;

    // Base -> Derived（成功）
    const auto b = d.staticCastTo<Base>();
    auto d2 = b.dynamicCastTo<Derived>();
    EXPECT_TRUE(static_cast<bool>(d2));
    EXPECT_EQ(d2->derived_val, 123);

    // 共享同一控制块
    EXPECT_EQ(d.strongRefs(), d2.strongRefs());
}

TEST(RefCntPtrTest, DynamicCastToFail) {
    auto b = original::makeStrongPtr<Base>();
    b->base_val = 55;

    // Base -> Derived（失败，应返回空 strongPtr）
    const auto d = b.dynamicCastTo<Derived>();
    EXPECT_FALSE(static_cast<bool>(d));
}

TEST(RefCntPtrTest, ConstCastTo) {
    auto d = original::makeStrongPtr<Derived>();
    d->derived_val = 10;

    // const Derived -> Derived
    const auto cd = d.staticCastTo<const Derived>();
    auto d2 = cd.constCastTo<Derived>();

    // 修改 d2，会反映到原始 d
    d2->derived_val = 20;
    EXPECT_EQ(d->derived_val, 20);

    // 确认引用计数保持一致
    EXPECT_EQ(d.strongRefs(), d2.strongRefs());
}