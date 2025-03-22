#ifndef ARRAY_H
#define ARRAY_H
#pragma once

#include <initializer_list>
#include "config.h"
#include "baseArray.h"
#include "iterationStream.h"
#include "randomAccessIterator.h"
#include "error.h"

namespace original {

    /**
     * @file array.h
     * @brief Provides the `array` class for a fixed-size container with random access.
     * @details This file defines the `array` class, which represents a fixed-size array-like container
     *          with random access and iterator support. The class inherits from `baseArray` and `iterationStream`.
     *          It provides functionality to manage the underlying array, as well as accessing, modifying, and
     *          iterating over elements.
     *
     *          The `array` class also defines an iterator for random access, as well as constructors and
     *          assignment operators for copying and moving array data.
     */

    /**
     * @class array
     * @tparam TYPE Type of the elements contained in the array
     * @brief A fixed-size array container with random access.
     * @extends baseArray
     * @extends iterationStream
     * @details The `array` class encapsulates a fixed-size array and provides operations such as indexing,
     *          element assignment, and iteration. It supports random access via iterators, which can be
     *          used to traverse the container from beginning to end, and vice versa.
     *
     *          This class offers both copy and move semantics, along with an iterator class that supports
     *          random access operations.
     */
    template<typename TYPE>
    class array final : public iterationStream<TYPE, array<TYPE>>, public baseArray<TYPE> {
        u_integer size_; ///< Size of the array
        TYPE* body;    ///< Pointer to the array's data

        /**
         * @brief Initializes the array with a specified size.
         * @param size The size of the array to allocate.
         * @details Allocates memory for the array and initializes each element to the default value of TYPE.
         */
        void arrInit(u_integer size);

        /**
         * @brief Destroys the array and releases its allocated memory.
         * @details Deallocates the memory used by the array.
         */
        void arrDestruct() const;

    public:

    /**
     * @class Iterator
     * @brief Iterator for the array class that supports random access.
     * @tparam TYPE The type of elements stored in the array.
     * @details The `Iterator` class provides random access to the elements of an `array`. It supports
     *          operations like `operator++`, `operator--`, and comparisons to iterate through the array
     *          elements in a bidirectional manner. The `Iterator` is an internal class used for providing
     *          iterator functionality on the `array` container.
     */
    class Iterator final : public randomAccessIterator<TYPE> {
        /**
         * @brief Constructs an iterator for the given position.
         * @param ptr A pointer to the element at the current position.
         * @param container A pointer to the array container.
         * @param pos The current position of the iterator in the array.
         */
        explicit Iterator(TYPE* ptr, const array* container, integer pos);

    public:
        friend array;

        /**
         * @brief Copy constructor for the iterator.
         * @param other The iterator to copy.
         * @details This constructor copies the state of the given iterator.
         */
        Iterator(const Iterator& other);

        /**
         * @brief Copy assignment operator for the iterator.
         * @param other The iterator to copy from.
         * @return A reference to this iterator.
         * @details This operator copies the state of the given iterator.
         */
        Iterator& operator=(const Iterator& other);

        /**
         * @brief Clones the iterator.
         * @return A new iterator object that is a copy of the current one.
         * @details The cloned iterator will point to the same position as the current iterator.
         */
        Iterator* clone() const override;

        /**
         * @brief Checks if this iterator is positioned just before the given iterator.
         * @param other The iterator to compare with.
         * @return `true` if this iterator is just before the `other` iterator, `false` otherwise.
         * @details This method is used to compare the relative positions of two iterators.
         */
        bool atPrev(const iterator<TYPE> *other) const override;

        /**
         * @brief Checks if this iterator is positioned just after the given iterator.
         * @param other The iterator to compare with.
         * @return `true` if this iterator is just after the `other` iterator, `false` otherwise.
         * @details This method is used to compare the relative positions of two iterators.
         */
        bool atNext(const iterator<TYPE> *other) const override;

        /**
         * @brief Returns the class name of this iterator.
         * @return A string representing the class name, which is "array::Iterator".
         */
        [[nodiscard]] std::string className() const override;
    };

        /**
         * @brief Constructs an empty array.
         * @param size The initial size of the array.
         * @details Initializes the array with a given size, allocating memory for the array and setting
         *          each element to its default value.
         */
        explicit array(u_integer size = 0);

        /**
         * @brief Constructs an array from an initializer list.
         * @param lst The initializer list to initialize the array.
         * @details This constructor allows creating an array from a list of elements. The size of the array
         *          will be the size of the initializer list.
         */
        array(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Copy constructor.
         * @param other The array to copy from.
         * @details Initializes the array by copying the elements from another array.
         */
        array(const array& other);

        /**
         * @brief Copy assignment operator.
         * @param other The array to copy from.
         * @return The assigned array.
         * @details Copies the elements of another array into this array.
         */
        array& operator=(const array& other);

        /**
         * @brief Move constructor.
         * @param other The array to move from.
         * @details Moves the elements and resources from another array into this one, leaving the source array
         *          in a valid but unspecified state.
         */
        array(array&& other) noexcept;

        /**
         * @brief Move assignment operator.
         * @param other The array to move from.
         * @return The moved array.
         * @details Moves the elements and resources from another array into this one, leaving the source array
         *          in a valid but unspecified state.
         */
        array& operator=(array&& other) noexcept;

        /**
         * @brief Returns the size of the array.
         * @return The number of elements in the array.
         */
        [[nodiscard]] u_integer size() const override;

        /**
         * @brief Returns a reference to the first element of the array.
         * @return A reference to the first element of the array.
         */
        TYPE& data() const;

        /**
         * @brief Retrieves an element at a specified index.
         * @param index The index of the element to retrieve.
         * @return The element at the specified index.
         * @throws outOfBoundError If the index is out of bounds.
         */
        TYPE get(integer index) const override;

        /**
         * @brief Access an element at a specified index for modification.
         * @param index The index of the element to access.
         * @return A reference to the element at the specified index.
         * @throws outOfBoundError If the index is out of bounds.
         */
        TYPE& operator[](integer index) override;

        /**
         * @brief Sets the value of an element at the specified index.
         * @param index The index of the element to set.
         * @param e The value to assign to the element.
         * @throws outOfBoundError If the index is out of bounds.
         */
        void set(integer index, const TYPE& e) override;

        /**
         * @brief Finds the index of the specified element in the array.
         * @param e The element to find.
         * @return The index of the element, or the size of the array if the element is not found.
         */
        u_integer indexOf(const TYPE& e) const override;

        /**
         * @brief Returns an iterator to the first element of the array.
         * @return An iterator to the first element.
         */
        Iterator* begins() const override;

        /**
         * @brief Returns an iterator to the last element of the array.
         * @return An iterator to the last element.
         */
        Iterator* ends() const override;

        /**
         * @brief Returns the class name.
         * @return The class name as a string.
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Destroys the array and releases its memory.
         * @details This destructor frees the memory allocated for the array.
         */
        ~array() override;
    };

} // namespace original

    template<typename TYPE>
    void original::array<TYPE>::arrInit(const u_integer size) {
        this->size_ = size;
        this->body = new TYPE[this->size_];
        for (u_integer i = 0; i < this->size(); ++i) {
            this->body[i] = TYPE{};
        }
    }

    template<typename TYPE>
    void original::array<TYPE>::arrDestruct() const
    {
        delete[] this->body;
    }

    template <typename TYPE>
    original::array<TYPE>::Iterator::Iterator(TYPE* ptr, const array* container, integer pos)
        : randomAccessIterator<TYPE>(ptr, container, pos) {}

    template <typename TYPE>
    original::array<TYPE>::Iterator::Iterator(const Iterator& other)
        : randomAccessIterator<TYPE>(nullptr, nullptr, 0)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::array<TYPE>::Iterator::operator=(const Iterator& other) -> Iterator&
    {
        if (this == &other) {
            return *this;
        }
        randomAccessIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return this->_ptr + 1 == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it->_ptr + 1 == this->_ptr;
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::className() const -> std::string {
        return "array::Iterator";
    }

    template <typename TYPE>
    original::array<TYPE>::array(const u_integer size)
        : size_(), body() {
        this->arrInit(size);
    }

    template <typename TYPE>
    original::array<TYPE>::array(const std::initializer_list<TYPE>& lst)
        : array(lst.size()) {
        u_integer i = 0;
        for (const auto& e : lst) {
            this->body[i] = e;
            i += 1;
        }
    }

    template <typename TYPE>
    original::array<TYPE>::array(const array& other)
        : array(other.size()) {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::array<TYPE>::operator=(const array& other) -> array&
    {
        if (this == &other)
            return *this;

        this->arrDestruct();

        this->arrInit(other.size());
        for (u_integer i = 0; i < this->size_; i++) {
            this->body[i] = other.body[i];
        }
        return *this;
    }

    template<typename TYPE>
    original::array<TYPE>::array(array&& other) noexcept : size_(0) {
        this->operator=(std::move(other));
    }

    template<typename TYPE>
    original::array<TYPE>& original::array<TYPE>::operator=(array&& other) noexcept {
        if (this == &other)
            return *this;

        this->arrDestruct();

        this->body = other.body;
        this->size_ = other.size_;
        other.arrInit(0);
        return *this;
    }

    template <typename TYPE>
    original::array<TYPE>::~array() {
        this->arrDestruct();
    }

    template <typename TYPE>
    auto original::array<TYPE>::size() const -> u_integer
    {
        return this->size_;
    }

    template<typename TYPE>
    TYPE& original::array<TYPE>::data() const {
        return this->body[0];
    }

    template <typename TYPE>
    auto original::array<TYPE>::get(integer index) const -> TYPE
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        return this->body[this->parseNegIndex(index)];
    }

    template <typename TYPE>
    auto original::array<TYPE>::operator[](integer index) -> TYPE&
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        return this->body[this->parseNegIndex(index)];
    }

    template <typename TYPE>
    auto original::array<TYPE>::set(integer index, const TYPE &e) -> void
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        this->body[this->parseNegIndex(index)] = e;
    }

    template <typename TYPE>
    auto original::array<TYPE>::indexOf(const TYPE &e) const -> u_integer
    {
        for (u_integer i = 0; i < this->size(); i += 1)
        {
            if (this->get(i) == e)
            {
                return i;
            }
        }
        return this->size();
    }

    template<typename TYPE>
    auto original::array<TYPE>::begins() const -> Iterator* {
        return new Iterator(&this->body[0], this, 0);
    }

    template<typename TYPE>
    auto original::array<TYPE>::ends() const -> Iterator* {
        return new Iterator(&this->body[this->size() - 1], this, this->size() - 1);
    }

    template <typename TYPE>
    std::string original::array<TYPE>::className() const
    {
        return "array";
    }

#endif //ARRAY_H
