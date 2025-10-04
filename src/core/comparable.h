#ifndef COMPARABLE_H
#define COMPARABLE_H
#include "config.h"
#include "types.h"
#include <compare>


namespace original {

/**
 * @file comparable.h
 * @brief Interface for objects that can be compared.
 * @details Defines an interface for comparing instances of derived classes.
 *          Derived classes must implement `compareTo()` to provide comparison logic.
 *          Provides common comparison operators for convenience.
 */

/**
 * @class comparable
 * @tparam DERIVED The type of the derived class.
 * @brief Base class for comparable objects.
 * @details This class defines a `compareTo()` method that must be implemented by derived classes to
 *          compare their instances. It also provides common comparison operators (`==`, `!=`, `<`,
 *          `>`, `<=`, `>=`) based on the `compareTo()` method.
 *
 * Derived classes are expected to override `compareTo()` to return:
 *  - A negative value if the current object is less than the other.
 *  - Zero if the current object is equal to the other.
 *  - A positive value if the current object is greater than the other.
 *
 * Classes implementing this interface automatically satisfy the `CmpTraits` concept.
 * @see CmpTraits
 */
template <typename DERIVED>
class comparable {
public:
    /**
     * @brief Compares the current object with another of the same type.
     * @param other The object to compare against.
     * @return A negative value if less than, zero if equal, and positive if greater than.
     */
    virtual integer compareTo(const DERIVED &other) const = 0;

    /**
     * @brief Checks if the current object is equal to another.
     * @param other The object to compare against.
     * @return True if equal, otherwise false.
     */
    bool operator==(const DERIVED &other) const;

    /**
     * @brief Checks if the current object is not equal to another.
     * @param other The object to compare against.
     * @return True if not equal, otherwise false.
     */
    bool operator!=(const DERIVED &other) const;

    /**
     * @brief Checks if the current object is less than another.
     * @param other The object to compare against.
     * @return True if less than, otherwise false.
     */
    bool operator<(const DERIVED &other) const;

    /**
     * @brief Checks if the current object is greater than another.
     * @param other The object to compare against.
     * @return True if greater than, otherwise false.
     */
    bool operator>(const DERIVED &other) const;

    /**
     * @brief Checks if the current object is less than or equal to another.
     * @param other The object to compare against.
     * @return True if less than or equal, otherwise false.
     */
    bool operator<=(const DERIVED &other) const;

    /**
     * @brief Checks if the current object is greater than or equal to another.
     * @param other The object to compare against.
     * @return True if greater than or equal, otherwise false.
     */
    bool operator>=(const DERIVED &other) const;

    /**
     * @brief Virtual destructor for proper cleanup of derived objects.
     */
    virtual ~comparable() = default;
};

// ----------------- Definitions of comparable.h -----------------

template<typename DERIVED>
auto comparable<DERIVED>::operator==(const DERIVED &other) const -> bool {
    return compareTo(other) == 0;
}

template<typename DERIVED>
auto comparable<DERIVED>::operator!=(const DERIVED &other) const -> bool {
    return compareTo(other) != 0;
}

template<typename DERIVED>
auto comparable<DERIVED>::operator<(const DERIVED &other) const -> bool {
    return compareTo(other) < 0;
}

template<typename DERIVED>
auto comparable<DERIVED>::operator>(const DERIVED &other) const -> bool {
    return compareTo(other) > 0;
}

template<typename DERIVED>
auto comparable<DERIVED>::operator<=(const DERIVED &other) const -> bool {
    return compareTo(other) <= 0;
}

template<typename DERIVED>
auto comparable<DERIVED>::operator>=(const DERIVED &other) const -> bool {
    return compareTo(other) >= 0;
}

} // namespace original

/**
 * @brief Three-way comparison operator for CmpTraits types
 * @tparam T Type satisfying CmpTraits concept
 * @param lhs Left-hand side object
 * @param rhs Right-hand side object
 * @return std::strong_ordering result based on compareTo()
 * @details Provides C++20 spaceship operator support for types implementing
 *          the comparable interface. Enables modern comparison syntax.
 *
 * Example:
 * @code{.cpp}
 * class MyComparable : public original::comparable<MyComparable> {
 *     integer compareTo(const MyComparable& other) const override { ... }
 * };
 *
 * MyComparable a, b;
 * auto result = a <=> b; // Uses this operator
 * @endcode
 */
template <original::CmpTraits T>
std::strong_ordering operator<=>(const T& lhs, const T& rhs)
{
    return lhs.compareTo(rhs) <=> 0;
}

#endif //COMPARABLE_H
