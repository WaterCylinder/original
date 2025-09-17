#include <gtest/gtest.h>
#include <stdexcept>
#include <functional>
#include <iostream>
#include "error.h"

using namespace original;

TEST(ErrorTest, OutOfBoundErrorTest) {
    outOfBoundError e;
    EXPECT_EQ(std::string(e.what()), "Original::outOfBoundError: Wrong value given.");
}

TEST(ErrorTest, ValueErrorTest) {
    valueError e;
    EXPECT_EQ(std::string(e.what()), "Original::valueError: Wrong value given.");
}

TEST(ErrorTest, NullPointerErrorTest) {
    nullPointerError e;
    EXPECT_EQ(std::string(e.what()), "Original::nullPointerError: Attempting to access null pointer.");
}

TEST(ErrorTest, UnSupportedMethodErrorTest) {
    unSupportedMethodError e;
    EXPECT_EQ(std::string(e.what()), "Original::unSupportedMethodError: Unsupported Method for the call.");
}

TEST(ErrorTest, NoElementErrorTest) {
    noElementError e;
    EXPECT_EQ(std::string(e.what()), "Original::noElementError: No such element.");
}

TEST(ErrorTest, CallbackSignatureErrorTest) {
    callbackSignatureError e;
    EXPECT_EQ(std::string(e.what()), "Original::callbackSignatureError: Callback signature mismatch.");
}

TEST(ErrorTest, CallbackReturnTypeErrorTest) {
    callbackReturnTypeError e;
    EXPECT_EQ(std::string(e.what()), "Original::callbackReturnTypeError: Return type of callback mismatch.");
}

TEST(ErrorTest, AllocateErrorTest) {
    allocateError e;
    EXPECT_EQ(std::string(e.what()), "Original::allocateError: Can not allocate memory.");
}

TEST(ErrorTest, SysErrorTest) {
    sysError e;
    EXPECT_EQ(std::string(e.what()), "Original::sysError: A system error triggered.");
}


// ------------------- CallBackChecker Tests -------------------

void validCallback(int a, double b) {
    std::cout << "a + b = " << a + b << std::endl;
}

int invalidReturnTypeCallback(int a, double b) {
    return a + static_cast<int>(b);
}

void invalidArgumentCallback(int a) {
    std::cout << a << std::endl;
}

class sampleClass {};

TEST(CallBackCheckerTest, ValidCallbackTest) {
    EXPECT_NO_THROW((callBackChecker::check<decltype(validCallback), void, int, double>()));
}

TEST(CallBackCheckerTest, InvalidReturnTypeTest) {
    EXPECT_THROW((callBackChecker::check<decltype(invalidReturnTypeCallback), float, int, double>()), callbackReturnTypeError);
}

TEST(CallBackCheckerTest, InvalidArgumentCountTest) {
    EXPECT_THROW((callBackChecker::check<decltype(invalidArgumentCallback), void, int, int>()), callbackSignatureError);
}

TEST(CallBackCheckerTest, InvalidArgumentTypeTest) {
    EXPECT_THROW((callBackChecker::check<decltype(invalidArgumentCallback), void, sampleClass>()), callbackSignatureError);
}

TEST(CallBackCheckerTest, EmptyCallbackTest) {
    EXPECT_NO_THROW((callBackChecker::check<std::function<void()>, void>()));
}
