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
     * @brief Three-way comparison operator (<=>), returns an ordered comparison result.
     * @details This operator is implemented by invoking the compareTo() method of the comparable object.
     *          The result type is std::strong_ordering, indicating strong ordering semantics.
     *          Defined as a friend function to enable symmetric argument handling.
     *
     * @tparam EXTENDED The actual derived type using CRTP pattern.
     * @param lc Left-hand side comparable object
     * @param rc Right-hand side comparable object
     * @return std::strong_ordering
     *          - std::strong_ordering::less    if lhs < rhs
     *          - std::strong_ordering::equal    if lhs == rhs
     *          - std::strong_ordering::greater if lhs > rhs
     */
    template<typename EXTENDED>
    requires ExtendsOf<comparable<EXTENDED>, EXTENDED>
    friend std::strong_ordering operator<=>(const EXTENDED& lc, const EXTENDED& rc);

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

template<typename EXTENDED>
requires ExtendsOf<comparable<EXTENDED>, EXTENDED>
std::strong_ordering operator<=>(const EXTENDED& lc, const EXTENDED& rc) {
    return lc.compareTo(rc) <=> 0;
}

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

template <typename T>
requires original::ExtendsOf<original::comparable<T>, T>
struct std::less<T> { // NOLINT
    bool operator()(const T& t1, const T& t2) const noexcept {
        return t1 < t2;
    }
};

#endif //COMPARABLE_H
