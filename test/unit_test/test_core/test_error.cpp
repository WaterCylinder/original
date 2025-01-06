#include <gtest/gtest.h>
#include <stdexcept>
#include "error.h"

using namespace original;

TEST(ErrorTest, OutOfBoundErrorTest) {
    outOfBoundError e;
    EXPECT_STREQ(e.what(), "Out of the bound of the object.");
}

TEST(ErrorTest, ValueErrorTest) {
    valueError e;
    EXPECT_STREQ(e.what(), "Wrong value given.");
}

TEST(ErrorTest, NullPointerErrorTest) {
    nullPointerError e;
    EXPECT_STREQ(e.what(), "Attempting to access null pointer.");
}

TEST(ErrorTest, UnSupportedMethodErrorTest) {
    unSupportedMethodError e;
    EXPECT_STREQ(e.what(), "Unsupported Method for class.");
}

TEST(ErrorTest, NoElementErrorTest) {
    noElementError e;
    EXPECT_STREQ(e.what(), "No such element.");
}


void validCallback(int a, double b) {
    std::cout << "a + b = " << a + b << std::endl;
}

int invalidReturnTypeCallback(int a, double b) {
    return a + static_cast<int>(b);
}


void invalidArgumentCallback(int a) {
    std::cout << a << std::endl;
}

class sampleClass{};

TEST(CallBackCheckerTest, ValidCallbackTest) {
    EXPECT_NO_THROW((original::callBackChecker::check<decltype(validCallback), void, int, double>()));
}

TEST(CallBackCheckerTest, InvalidCallbackTest) {
    EXPECT_THROW((original::callBackChecker::check<decltype(invalidReturnTypeCallback), float, int, double>()), CallbackReturnTypeError);
}

TEST(CallBackCheckerTest, InvalidArgumentCallbackTest1) {
    EXPECT_THROW((original::callBackChecker::check<decltype(invalidArgumentCallback), void, int, int>()), CallbackSignatureError);
}


TEST(CallBackCheckerTest, InvalidArgumentCallbackTest2) {
    EXPECT_THROW((original::callBackChecker::check<decltype(invalidArgumentCallback), void, sampleClass>()), CallbackSignatureError);
}

TEST(CallBackCheckerTest, EmptyCallbackTest) {
    EXPECT_NO_THROW((original::callBackChecker::check<std::function<void()>, void>()));
}