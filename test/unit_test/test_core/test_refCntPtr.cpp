#include <gtest/gtest.h>
#include "refCntPtr.h"

// 自定义测试对象用于追踪构造/析构次数
class TrackedObject {
public:
    static int alive_count;
    int id;

    explicit TrackedObject(const int i = 0) : id(i) { alive_count++; }
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
    auto arr = original::makeStrongPtr<TrackedObject>(3); // 创建含3个元素的数组
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
                 empty_ptr->id; // 访问空指针
    }, original::nullPointerError);
}

// 定义包含循环引用的 Node 类
struct Node {
    original::strongPtr<Node> next;  // 强引用
    original::weakPtr<Node> prev;    // 弱引用，用于打破循环
    int value;

    explicit Node(const int val = 0) : next(static_cast<Node *>(nullptr)), value(val) {
    }
};

TEST(RefCntPtrTest, BreakCyclicReference) {

    {
        auto node1 = original::strongPtr<Node>(1);
        auto node2 = original::strongPtr<Node>(2);
    }

    {
        auto node1 = original::strongPtr<Node>(1);
        auto node2 = original::strongPtr<Node>(2);

        node1->next = node2;  // 强引用
        node2->prev = node1;  // 弱引用
    }

    {
        auto node1 = original::strongPtr<Node>(1);
        auto node2 = original::strongPtr<Node>(2);

        node1->prev = node2;  // 弱引用
        node2->prev = node1;  // 弱引用
    }

    {
        auto node1 = original::strongPtr<Node>(1);
        auto node2 = original::strongPtr<Node>(2);

        node1->next = node2;  // node2 强引用
        node2->next = node1;  // node1 强引用


        node1->next.reset();
        node2->prev = node1;  // node1弱引用
    }
}