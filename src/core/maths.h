#ifndef MATHS_H
#define MATHS_H
#include "error.h"

namespace original {

/**
 * @file maths.h
 * @brief Mathematical utilities and constants.
 * @details This header file defines commonly used mathematical constants and utility
 * functions that can be used throughout the library. These functions provide basic
 * mathematical operations such as absolute value, maximum, minimum, and power
 * calculations.
 */

/**
 * @brief The mathematical constant E (Euler's number).
 * @details This is the base of the natural logarithm, approximately equal to 2.71828.
 * It is used in various mathematical, scientific, and engineering applications,
 * particularly in exponential growth and decay models, as well as in complex analysis.
 */
constexpr long double E = 2.7182818284590452353602874713526624977572470937000L;

/**
 * @brief The mathematical constant PI (π).
 * @details This is the ratio of a circle's circumference to its diameter, approximately equal to 3.14159.
 * It is used in geometry, trigonometry, and many other mathematical fields, especially when dealing with circular or spherical shapes.
 */
constexpr long double PI = 3.1415926535897932384626433832795028841971693993751L;

/**
 * @brief Returns the absolute value of a given number.
 * @tparam TYPE The type of the input value.
 * @param a The input value.
 * @return The absolute value of the input value.
 * @details This function returns the absolute value of the input argument `a`, which
 * is the distance of `a` from zero. It works for both positive and negative values.
 *
 * @code{.cpp}
 * // Example usage:
 * int a = -5;
 * int result = original::abs(a); // result will be 5
 * @endcode
 */
template<typename TYPE>
TYPE abs(TYPE a);

/**
 * @brief Returns the larger of two given values.
 * @tparam TYPE The type of the input values.
 * @param a The first input value.
 * @param b The second input value.
 * @return The larger of `a` and `b`.
 * @details This function compares the two values `a` and `b`, and returns the value
 * that is greater.
 *
 * @code{.cpp}
 * // Example usage:
 * double a = 3.5, b = 7.2;
 * double result = original::max(a, b); // result will be 7.2
 * @endcode
 */
template<typename TYPE>
TYPE max(TYPE a, TYPE b);

/**
 * @brief Returns the smaller of two given values.
 * @tparam TYPE The type of the input values.
 * @param a The first input value.
 * @param b The second input value.
 * @return The smaller of `a` and `b`.
 * @details This function compares the two values `a` and `b`, and returns the value
 * that is smaller.
 *
 * @code{.cpp}
 * // Example usage:
 * double a = 3.5, b = 7.2;
 * double result = original::min(a, b); // result will be 3.5
 * @endcode
 */
template<typename TYPE>
TYPE min(TYPE a, TYPE b);

/**
 * @brief Returns the result of raising a base to an exponent.
 * @param base The base value.
 * @param exp The exponent value.
 * @return The result of `base` raised to the power of `exp`.
 * @throw valueError If `base` is 0 and `exp` is less than or equal to 0.
 * @details This function performs exponentiation by iterating `exp` times and
 * multiplying the base. If `exp` is negative, it computes the reciprocal of the
 * result. An exception is thrown if `base` is 0 and `exp` is non-positive.
 *
 * @code{.cpp}
 * // Example usage:
 * double base = 2.0;
 * int exp = 3;
 * double result = original::pow(base, exp); // result will be 8.0
 *
 * // For negative exponent:
 * exp = -2;
 * result = original::pow(base, exp); // result will be 0.25
 * @endcode
 */
double pow(double base, int exp);

} // namespace original

// ----------------- Definitions of maths.h -----------------

template<typename TYPE>
auto original::abs(TYPE a) -> TYPE
{
    return a > TYPE{} ? a : -a;
}

template<typename TYPE>
auto original::max(TYPE a, TYPE b) -> TYPE
{
    return a > b ? a : b;
}

template<typename TYPE>
auto original::min(TYPE a, TYPE b) -> TYPE
{
    return a < b ? a : b;
}

inline auto original::pow(const double base, const int exp) -> double
{
    if (base == 0 && exp <= 0)
    {
        throw valueError();
    }
    double res = 1.0;
    for (int i = 0; i < abs(exp); i += 1)
    {
        res *= base;
    }
    return exp >= 0 ? res : 1 / res;
}

#endif //MATHS_H
