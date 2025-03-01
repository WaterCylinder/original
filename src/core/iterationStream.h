#ifndef ITERATIONSTREAM_H
#define ITERATIONSTREAM_H

#include <sstream>

#include "comparable.h"
#include "printable.h"
#include "iterable.h"
#include "types.h"

/**
 * @file iterationStream.h
 * @brief Provides functionality for an iteration stream.
 * @details This file contains the declaration of the `iterationStream` template class,
 * which allows iteration over elements while also supporting comparison and printing.
 * It derives from the `printable`, `iterable`, and `comparable` interfaces.
 */

namespace original {
    /**
     * @class iterationStream
     * @tparam TYPE The type of elements contained in the stream.
     * @tparam DERIVED The derived class type.
     * @brief A stream class that allows iteration, comparison, and printing.
     * @details This class allows iteration over a collection of elements of type `TYPE`.
     * It provides methods for comparing two iteration streams and converting the stream
     * into a string representation. The class is intended to be inherited by more specific stream types.
     * @extends printable
     * @extends iterable
     * @extends comparable
     */
    template<typename TYPE, typename DERIVED>
    class iterationStream : public printable, public iterable<TYPE>, public comparable<iterationStream<TYPE, DERIVED>> {
    protected:
        /**
         * @brief Returns a string representation of the elements in the stream.
         * @details This method iterates over the elements and formats them into a string.
         * @return A string that lists all elements in the stream.
         */
        [[nodiscard]] std::string elementsString() const;

    public:
        /**
         * @brief Compares the current iteration stream with another iteration stream.
         * @param other Another iteration stream to compare with.
         * @details Compares the elements of the two streams sequentially.
         * If the elements are equal, the comparison continues. The first unequal
         * element determines the result.
         * @return A value indicating the comparison result:
         *         -1 if the current stream is less than the other,
         *         1 if greater, and 0 if equal.
         */
        integer compareTo(const iterationStream &other) const override;

        /**
         * @brief Returns the class name.
         * @details This method returns a string representing the class name of the object.
         * @return The class name ("iterationStream").
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Converts the iteration stream to a string representation.
         * @param enter Whether to add a newline at the end of the string.
         * @details This method returns a string containing the class name followed by
         * a string representation of the stream elements. An optional newline character
         * can be appended based on the `enter` parameter.
         * @return A string representation of the stream.
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
