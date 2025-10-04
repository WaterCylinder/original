#ifndef ITERATIONSTREAM_H
#define ITERATIONSTREAM_H

#include <sstream>
#include "comparable.h"
#include "hash.h"
#include "printable.h"
#include "iterable.h"
#include "types.h"

/**
 * @file iterationStream.h
 * @brief Provides functionality for an iteration stream with comparison, hashing and printing.
 * @details This file contains the declaration of the `iterationStream` template class,
 * which allows iteration over elements while also supporting comparison, hashing and printing.
 * It derives from the `printable`, `iterable`, `comparable` and `hashable` interfaces.
 * The class provides CRTP (Curiously Recurring Template Pattern) implementation for
 * derived stream types to inherit common iteration functionality.
 */

namespace original {
    /**
     * @class iterationStream
     * @tparam TYPE The type of elements contained in the stream.
     * @tparam DERIVED The derived class type using CRTP pattern.
     * @brief A stream class that allows iteration, comparison, hashing and printing.
     * @details This class provides a unified interface for streams that need to support:
     * - Element iteration through iterable interface
     * - Lexicographical comparison with other streams
     * - Hash value computation based on element contents
     * - String representation with formatted output
     *
     * The class uses CRTP to enable static polymorphism, allowing derived classes
     * to inherit common functionality while maintaining their specific type identity.
     *
     * @extends printable Provides string conversion capabilities
     * @extends iterable Provides element iteration support
     * @extends comparable Enables stream comparison
     * @extends hashable Provides hash computation
     *
     * @note Derived classes must implement the iterable interface methods (begins()/ends())
     *       to provide actual element access.
     *
     * Example usage:
     * @code
     * class MyStream : public iterationStream<int, MyStream> {
     *     // Implement begins()/ends() and other specific methods
     * };
     *
     * MyStream stream1, stream2;
     * if (stream1 == stream2) { ... }           // Uses compareTo
     * auto hash = stream1.toHash();            // Uses element-based hashing
     * std::cout << stream1.toString();         // Formatted output
     * @endcode
     */
    template<typename TYPE, typename DERIVED>
    class iterationStream : public printable,
                            public iterable<TYPE>,
                            public comparable<iterationStream<TYPE, DERIVED>>,
                            public hashable<iterationStream<TYPE, DERIVED>> {
    protected:
        /**
         * @brief Returns a string representation of the elements in the stream.
         * @details This method iterates over all elements and formats them into a
         * comma-separated list enclosed in parentheses. Each element is formatted
         * using formatString function for consistent representation.
         *
         * Example output: "(element1, element2, element3)"
         *
         * @return A string that lists all elements in the stream in iteration order.
         * @note Uses the iterable interface to traverse elements. Empty streams
         *       return "()".
         */
        [[nodiscard]] std::string elementsString() const;

    public:
        /**
         * @brief Compares the current iteration stream with another iteration stream.
         * @param other Another iteration stream to compare with.
         * @details Performs lexicographical comparison of the two streams:
         * - Compares elements sequentially using their natural ordering
         * - If TYPE is Comparable, uses element comparison operators
         * - If elements are equal, continues to next elements
         * - If one stream is a prefix of the other, the shorter stream is considered less
         * - The comparison stops at the first unequal element
         *
         * @return A value indicating the comparison result:
         *         -1 if the current stream is less than the other,
         *         1 if greater, and 0 if equal.
         * @note For non-Comparable TYPE, element comparison is not performed and
         *       only length comparison applies after exhausting equal elements.
         */
        integer compareTo(const iterationStream &other) const override;

        /**
         * @brief Computes a hash value for the iteration stream.
         * @return A hash value computed from all elements in the stream.
         * @details Uses hash combination algorithm to create a single hash value
         * from all elements. The hash is computed by iterating through all elements
         * and combining their individual hash values.
         *
         * @note Provides noexcept guarantee. For empty streams, returns 0.
         * @warning Hash quality depends on the quality of hash<TYPE> specialization.
         */
        [[nodiscard]] u_integer toHash() const noexcept override;

        /**
         * @brief Returns the class name.
         * @return The class name ("iterationStream").
         * @note Derived classes should override this to return their actual class name.
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Converts the iteration stream to a string representation.
         * @param enter Whether to add a newline at the end of the string.
         * @details This method returns a string containing the class name followed by
         * a string representation of the stream elements in parentheses. An optional
         * newline character can be appended based on the `enter` parameter.
         *
         * Example output with enter=true: "iterationStream(element1, element2)\n"
         *
         * @return A complete string representation of the stream including class name
         *         and formatted elements.
         */
        [[nodiscard]] std::string toString(bool enter) const override;
    };
}

template<typename TYPE, typename DERIVED>
auto original::iterationStream<TYPE, DERIVED>::elementsString() const -> std::string
{
    std::stringstream ss;
    ss << "(";
    bool first = true;
    for (const auto it = this->begin(); it.isValid(); ++it) {
        if (!first) {
            ss << ", ";
        }
        ss << formatString(it.get());
        first = false;
    }
    ss << ")";
    return ss.str();
}

template<typename TYPE, typename DERIVED>
auto original::iterationStream<TYPE, DERIVED>::compareTo(const iterationStream &other) const -> integer {
    const auto this_it = this->begin();
    const auto other_it = other.begin();
    for (; this_it.isValid() && other_it.isValid(); ++this_it, ++other_it) {
        if constexpr (Comparable<TYPE>){
            if (*this_it != *other_it)
                return *this_it < *other_it ? -1 : 1;
        }
    }
    return this_it.isValid() - other_it.isValid();
}

template <typename TYPE, typename DERIVED>
original::u_integer original::iterationStream<TYPE, DERIVED>::toHash() const noexcept
{
    u_integer seed = 0;
    for (const auto& elem : *this) {
        hash<TYPE>::hashCombine(seed, elem);
    }
    return seed;
}

template<typename TYPE, typename DERIVED>
auto original::iterationStream<TYPE, DERIVED>::className() const -> std::string {
    return "iterationStream";
}

template<typename TYPE, typename DERIVED>
auto original::iterationStream<TYPE, DERIVED>::toString(const bool enter) const -> std::string
{
    std::stringstream ss;
    ss << this->className() << this->elementsString();
    if (enter) ss << "\n";
    return ss.str();
}

#endif //ITERATIONSTREAM_H
