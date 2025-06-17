#include <gtest/gtest.h>
#include "thread.h"
#include <atomic>
#include <chrono>

using namespace original;

namespace {
    // Test fixture for thread tests
    class ThreadTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Setup code if needed
        }

        void TearDown() override {
            // Cleanup code if needed
        }
    };

    // Simple function for testing
    void simpleFunction(int& value) {
        value = 42;
    }

    // Function that throws an error
    void throwingFunction(std::exception_ptr& ptr) {
        try {
            throw sysError();
        }
        catch (const sysError&) {
            ptr = std::current_exception();
        }
    }

    // Function with delay for join/detach tests
    void delayedFunction(std::atomic<bool>& flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        flag = true;
    }
}

// Test basic thread creation with function
TEST_F(ThreadTest, BasicFunctionThread) {
    int value = 0;
    {
        thread t1(simpleFunction, std::ref(value));
    }
    ASSERT_EQ(value, 42);

    thread t2([](){});
    ul_integer id1 = t2.id();
    ASSERT_NE(id1, 0);  // ID should not be zero for a valid thread

    thread t3;
    ASSERT_EQ(t3.id(), 0);  // Default-constructed thread should have ID 0
}

// Test thread creation with lambda
TEST_F(ThreadTest, LambdaThread) {
    int value = 0;
    {
        thread t([&value]() { value = 100; });
    }
    ASSERT_EQ(value, 100);
}

// Test move constructor
TEST_F(ThreadTest, MoveConstructor) {
    std::atomic<bool> flag(false);
    {
        thread t1(delayedFunction, std::ref(flag));
        thread t2(std::move(t1));
    }
    ASSERT_TRUE(flag);
}

// Test move assignment
TEST_F(ThreadTest, MoveAssignment) {
    std::atomic<bool> flag1(false);
    std::atomic<bool> flag2(false);
    {
        thread t1(delayedFunction, std::ref(flag1));
        thread t2(delayedFunction, std::ref(flag2));
        ASSERT_FALSE(flag2); // t2's original thread should be detached
    }

    ASSERT_TRUE(flag1);
}

// Test joinable
TEST_F(ThreadTest, Joinable) {
    thread t([](){});
    ASSERT_TRUE(t.joinable());
    t.join();
    ASSERT_FALSE(t.joinable());
}

// Test detach
TEST_F(ThreadTest, Detach) {
    std::atomic<bool> flag(false);
    {
    thread t(delayedFunction, std::ref(flag));
    t.detach();
    ASSERT_FALSE(t);
}
    // Give some time for the thread to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ASSERT_TRUE(flag);
}

// Test automatic join in destructor
TEST_F(ThreadTest, DestructorJoin) {
    std::atomic<bool> flag(false);
    {
    thread t(delayedFunction, std::ref(flag));
    // t will be joined when it goes out of scope
    }
    ASSERT_TRUE(flag);
}

// Test automatic detach in destructor when will_join is false
TEST_F(ThreadTest, DestructorDetach) {
    std::atomic<bool> flag(false);
    {
    thread t(delayedFunction, std::ref(flag));
    // t will be detached when it goes out of scope
    }
    // Give some time for the thread to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ASSERT_TRUE(flag);
}

// Test thread with multiple arguments
TEST_F(ThreadTest, MultipleArguments) {
    int result = 0;
    auto func = [](int a, int b, int& r) { r = a + b; };
    {
        thread t(func, 10, 20, std::ref(result));
    }
    ASSERT_EQ(result, 30);
}

// Test error handling in thread
TEST_F(ThreadTest, ThreadThrowsError) {
    bool exception_caught = false;
    std::exception_ptr ptr;
    thread t{ throwingFunction, std::ref(ptr) };
    t.join();
    try {
        std::rethrow_exception(ptr);
    }
    catch (const sysError&) {
        exception_caught = true;
    }

    EXPECT_TRUE(exception_caught);
}

// Test bool operator
TEST_F(ThreadTest, BoolOperator) {
    thread t1;
    ASSERT_FALSE(t1);

    thread t2([](){});
    ASSERT_TRUE(t2);
    t2.join();
    ASSERT_FALSE(t2);
}

// Test not operator
TEST_F(ThreadTest, NotOperator) {
    thread t1;
    ASSERT_TRUE(!t1);

    thread t2([](){});
    ASSERT_FALSE(!t2);
    t2.join();
    ASSERT_TRUE(!t2);
}

// Test will_join flag
TEST_F(ThreadTest, WillJoinFlag) {
    std::atomic<bool> flag(false);
    {
    thread t(delayedFunction, std::ref(flag));
    // Will be detached on destruction
    }
    // Give some time for the thread to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    ASSERT_TRUE(flag);
}

// Test thread ID uniqueness
TEST_F(ThreadTest, ThreadIdUniqueness) {
    thread t1([](){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
    thread t2([](){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); });

    ul_integer id1 = t1.id();
    ul_integer id2 = t2.id();

    ASSERT_NE(id1, 0);
    ASSERT_NE(id2, 0);
    ASSERT_NE(id1, id2);  // Different threads should have different IDs

    t1.join();
    t2.join();

    // After joining, the IDs should be 0 because handles are reset
    ASSERT_EQ(t1.id(), 0);
    ASSERT_EQ(t2.id(), 0);
}

// Test thread ID after move
TEST_F(ThreadTest, ThreadIdAfterMove) {
    thread t1([](){});
    ul_integer original_id = t1.id();

    thread t2(std::move(t1));
    ASSERT_EQ(t2.id(), original_id);  // ID should be preserved after move
    ASSERT_EQ(t1.id(), 0);            // Moved-from thread should have ID 0

    t2.join();
    ASSERT_EQ(t2.id(), 0);            // After joining, ID should be 0
}

// Test pThread ID functionality
TEST_F(ThreadTest, PThreadId) {
    pThread pt([](){});
    ul_integer id1 = pt.id();
    ASSERT_NE(id1, 0);

    pThread pt2(std::move(pt));
    ASSERT_EQ(pt2.id(), id1);  // ID should be preserved after move
    ASSERT_EQ(pt.id(), 0);     // Moved-from pThread should have ID 0

    pt2.join();
}
