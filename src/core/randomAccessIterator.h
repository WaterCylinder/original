#ifndef RANDOMACCESSITERATOR_H
#define RANDOMACCESSITERATOR_H

#include "container.h"
#include "error.h"
#include "iterator.h"
#include <limits>

/**
 * @file randomAccessIterator.h
 * @brief Base class for random-access iterators
 * @details Provides common functionality for iterators supporting random access operations.
 * Features include pointer arithmetic, validity checking, and bidirectional traversal.
 */

namespace original {

    /**
     * @class randomAccessIterator
     * @tparam TYPE Type of elements being iterated
     * @brief Abstract base class for random-access iterators
     * @extends baseIterator
     * @details Implements core functionality for iterators that support:
     * - Pointer arithmetic operations (+, +=, -, -=)
     * - Bidirectional traversal
     * - Position validity checking
     * - Element access/modification
     *
     * @note Maintains three state markers:
     * - Raw pointer to the current element
     * - Reference to parent container
     * - Absolute position index
     */
    template<typename TYPE>
    class randomAccessIterator : public baseIterator<TYPE> {
    protected:
        mutable TYPE* _ptr;             ///< Pointer to the current element
        mutable const container<TYPE>* _container; ///< Reference to the parent container
        mutable int64_t _pos;           ///< Absolute position in the container

        /**
         * @brief Protected constructor for derived classes
         * @param ptr Raw element pointer
         * @param container Parent container reference
         * @param pos Initial position index
         */
        explicit randomAccessIterator(TYPE* ptr, const container<TYPE>* container, int64_t pos);

        /**
         * @brief Equality comparison implementation
         * @param other Iterator to compare with
         * @return True when pointing to the same memory location
         */
        bool equalPtr(const iterator<TYPE>* other) const override;

    public:

        /**
         * @brief Copy constructor
         * @param other Iterator to copy from
         */
        randomAccessIterator(const randomAccessIterator& other);

        /**
         * @brief Copy assignment operator
         * @param other Iterator to copy from
         * @return Reference to this iterator
         */
        randomAccessIterator& operator=(const randomAccessIterator& other);

        /**
         * @brief Creates a heap-allocated copy
         * @return New iterator instance
         */
        randomAccessIterator* clone() const override;

        /**
         * @brief Checks forward traversal capability
         * @return True if not at the end
         */
        [[nodiscard]] bool hasNext() const override;

        /**
         * @brief Checks reverse traversal capability
         * @return True if not at the beginning
         */
        [[nodiscard]] bool hasPrev() const override;

        /**
         * @brief Checks if the current iterator is at the previous position compared to another iterator.
         * @param other The iterator to compare with.
         * @return True if the iterator is at the previous position, otherwise false.
         */
        bool atPrev(const iterator<TYPE>* other) const override;

        /**
         * @brief Checks if the current iterator is at the next position compared to another iterator.
         * @param other The iterator to compare with.
         * @return True if the iterator is at the next position, otherwise false.
         */
        bool atNext(const iterator<TYPE>* other) const override;

        /**
         * @brief Advances to the next position in the container
         */
        void next() const override;

        /**
         * @brief Retreats to the previous position in the container
         */
        void prev() const override;

        /**
         * @brief Moves forward by N positions
         * @param steps Number of positions to advance
         */
        void operator+=(int64_t steps) const override;

        /**
         * @brief Moves backward by N positions
         * @param steps Number of positions to retreat
         */
        void operator-=(int64_t steps) const override;

        /**
         * @brief Computes the distance between the current iterator and another iterator.
         * @param other The iterator to compare with
         * @return The number of positions between the two iterators
         */
        int64_t operator-(const iterator<TYPE>& other) const override;

        /**
         * @brief Gets an iterator pointing to the next element
         * @return A new iterator pointing to the next element
         */
        randomAccessIterator* getNext() const override;

        /**
         * @brief Gets an iterator pointing to the previous element
         * @return A new iterator pointing to the previous element
         */
        randomAccessIterator* getPrev() const override;

        /**
         * @brief Gets the current element in the container
         * @return Reference to the current element
         */
        TYPE& get() override;

        /**
         * @brief Gets the current element in the container (const version)
         * @return Const reference to the current element
         */
        TYPE get() const override;

        /**
         * @brief Sets the value of the current element
         * @param data The value to set to the current element
         */
        void set(const TYPE& data) override;

        /**
         * @brief Checks if the iterator is valid (points to a valid element in the container)
         * @return True if the iterator is valid, otherwise false
         */
        [[nodiscard]] bool isValid() const override;

        /**
         * @brief Gets the class name of the iterator
         * @return The class name as a string
         */
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE>
    original::randomAccessIterator<TYPE>::randomAccessIterator(TYPE* ptr, const container<TYPE>* container,
                                                               const int64_t pos)
        : _ptr{ptr}, _container{container}, _pos(pos) {}

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::equalPtr(const iterator<TYPE> *other) const -> bool {
        auto* other_it = dynamic_cast<const randomAccessIterator*>(other);
        return other_it != nullptr && _ptr == other_it->_ptr;
    }

    template<typename TYPE>
    original::randomAccessIterator<TYPE>::randomAccessIterator(const randomAccessIterator &other) : _pos(0) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator=(
        const randomAccessIterator &other) -> randomAccessIterator& {
        if (this == &other) {
            return *this;
        }
        this->_ptr = other._ptr;
        this->_container = other._container;
        this->_pos = other._pos;
        return *this;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::clone() const -> randomAccessIterator* {
        return new randomAccessIterator{_ptr, _container, _pos};
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::hasNext() const -> bool {
        return this->isValid() && this->_pos <= this->_container->size() - 1;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::hasPrev() const -> bool {
        return this->isValid() && this->_pos >= 1;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::atPrev(const iterator<TYPE>*) const -> bool {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::atNext(const iterator<TYPE>*) const -> bool {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::next() const -> void {
        ++this->_pos;
        ++this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::prev() const -> void {
        --this->_pos;
        --this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator+=(int64_t steps) const -> void {
        this->_pos += steps;
        this->_ptr += steps;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator-=(int64_t steps) const -> void {
        this->_pos -= steps;
        this->_ptr -= steps;
    }

    template <typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator-(const iterator<TYPE>& other) const -> int64_t
    {
        auto* other_it = dynamic_cast<const randomAccessIterator*>(&other);
        if (other_it == nullptr)
            return this > &other ?
                std::numeric_limits<int64_t>::max() :
                std::numeric_limits<int64_t>::min();
        if (this->_container != other_it->_container)
            return this->_container > other_it->_container ?
                std::numeric_limits<int64_t>::max() :
                std::numeric_limits<int64_t>::min();
        return this->_ptr - other_it->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::getNext() const -> randomAccessIterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto it = this->clone();
        it->next();
        return it;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::getPrev() const -> randomAccessIterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto it = this->clone();
        it->prev();
        return it;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::get() -> TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return *this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::set(const TYPE& data) -> void {
        if (!this->isValid()) throw outOfBoundError();
        *this->_ptr = data;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::get() const -> TYPE
    {
        if (!this->isValid()) throw outOfBoundError();
        return *this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::isValid() const -> bool {
        return this->_pos >= 0 && this->_pos < this->_container->size();
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::className() const -> std::string {
        return "RandomAccessIterator";
    }

#endif //RANDOMACCESSITERATOR_H
