#ifndef SERIAL_H
#define SERIAL_H

#include "container.h"

namespace original {
    /**
    * @file serial.h
    * @brief Defines an abstract base class for sequential containers
    *        with index-based access.
    *
    * @details The `serial` class is intended to be used as a base for containers that support index-based
    *          access, such as arrays, lists, and other data structures where elements can be accessed
    *          via an index. Derived classes are required to implement certain methods such as `get()`,
    *          `operator[]`, `set()`, and `indexOf()`. This class offers methods for safely accessing
    *          elements, including negative index handling and bounds checking.
    */

    /**
     * @class serial
     * @tparam TYPE Type of the elements contained in the serial container
     * @brief Abstract base class for sequential containers with index-based access.
     * @extends container
     * @details Defines common interface for containers that support index-based access to their elements.
     *          It provides methods for element retrieval, bounds checking, and negative index handling.
     *          Derived classes must implement `get()`, `operator[]`, `set()`, and `indexOf()` methods.
     */
    template <typename TYPE, typename ALLOC>
    class serial : public container<TYPE, ALLOC> {
    protected:
        using container<TYPE, ALLOC>::container;

        /**
         * @brief Checks if the provided index is out of bounds.
         * @param index The index to check.
         * @return `true` if the index is out of bounds, otherwise `false`.
         * @details This method accounts for negative indices by converting them to positive indices
         *          before performing the bounds check.
         */
        [[nodiscard]] bool indexOutOfBound(integer index) const;

        /**
         * @brief Converts negative indices into valid positive indices.
         * @param index The index to convert.
         * @return The corresponding positive index for negative indices, otherwise returns the original index.
         * @details For example, if the index is -1, it will be converted to the last element's index.
         */
        [[nodiscard]] integer parseNegIndex(integer index) const;

    public:
        /**
         * @brief Retrieves the element at the specified index.
         * @param index The index of the element.
         * @return The element at the given index.
         * @details Derived classes must implement this method to provide the actual element retrieval.
         */
        virtual TYPE get(integer index) const = 0;

        /**
         * @brief Retrieves the first element in the container.
         * @return The first element in the container.
         * @details This method provides a shortcut to get the first element by calling `get(0)`.
         */
        virtual TYPE getBegin() const;

        /**
         * @brief Retrieves the last element in the container.
         * @return The last element in the container.
         * @details This method provides a shortcut to get the last element by calling `get(-1)`.
         */
        virtual TYPE getEnd() const;

        /**
         * @brief Retrieves the element at the specified index (const version).
         * @param index The index of the element.
         * @return A copy of the element at the given index.
         * @details This method calls `get(index)` to retrieve the element, but returns a copy.
         */
        virtual TYPE operator[](integer index) const;

        /**
         * @brief Retrieves or sets the element at the specified index (non-const version).
         * @param index The index of the element.
         * @return Reference to the element at the given index.
         * @details This method allows direct modification of the element at the specified index.
         */
        virtual TYPE& operator[](integer index) = 0;

        /**
         * @brief Sets the element at the specified index.
         * @param index The index of the element to set.
         * @param e The new value to store at the specified index.
         * @post get(index) == e
         */
        virtual void set(integer index, const TYPE& e) = 0;

        /**
         * @brief Finds the index of the specified element.
         * @param e The element to find.
         * @return The index of the element, or `size()` if not found.
         * @details This method searches for the element in the container and returns its index.
         */
        virtual u_integer indexOf(const TYPE& e) const = 0;

        /**
         * @brief Checks if the container contains the specified element.
         * @param e The element to check for.
         * @return `true` if the container contains the element, otherwise `false`.
         * @details This method uses `indexOf()` by default to check if the element exists within the container.
         */
        bool contains(const TYPE& e) const override;
    };
}

// -------------------- Definitions of serial.h --------------------

template<typename TYPE, typename ALLOC>
auto original::serial<TYPE, ALLOC>::indexOutOfBound(const integer index) const -> bool
{
    integer parsed_index = this->parseNegIndex(index);
    return parsed_index < 0 || parsed_index >= this->size();
}

template<typename TYPE, typename ALLOC>
auto original::serial<TYPE, ALLOC>::parseNegIndex(integer index) const -> integer {
    return index >= 0 ? index : this->size() + index;
}

template<typename TYPE, typename ALLOC>
auto original::serial<TYPE, ALLOC>::getBegin() const -> TYPE {
    return this->get(0);
}

template<typename TYPE, typename ALLOC>
auto original::serial<TYPE, ALLOC>::getEnd() const -> TYPE {
    return this->get(-1);
}

template<typename TYPE, typename ALLOC>
auto original::serial<TYPE, ALLOC>::operator[](const integer index) const -> TYPE
{
    return this->get(index);
}

template<typename TYPE, typename ALLOC>
auto original::serial<TYPE, ALLOC>::contains(const TYPE &e) const -> bool
{
    return this->indexOf(e) != this->size();
}

#endif //SERIAL_H
