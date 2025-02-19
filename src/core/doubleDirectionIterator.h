#ifndef DOUBLEDIRECTIONITERATOR_H
#define DOUBLEDIRECTIONITERATOR_H
#include "stepIterator.h"

/**
 * @file doubleDirectionIterator.h
 * @brief Double-direction iterator base class
 * @details Provides common functionality for iterators that can only traverse in both directions (forward and backward).
 *          It extends the `stepIterator` class to include functionality that applies to single-direction step iterators.
 */

namespace original {

    /**
     * @class doubleDirectionIterator
     * @tparam TYPE Type of elements being iterated
     * @brief Abstract base class for double-direction iterators
     * @extends stepIterator
     * @details Implements core functionality for iterators that can traverse elements in both directions
     *          (forward and backward) with single-step movements. This class extends the `stepIterator`
     *          and provides support for both forward and backward iteration.
     */
    template<typename TYPE>
    class doubleDirectionIterator : public stepIterator<TYPE> {
    protected:
        /**
         * @brief Protected constructor for doubleDirectionIterator
         * @param ptr Raw pointer to the element being iterated
         */
        explicit doubleDirectionIterator(wrapper<TYPE>* ptr);

    public:
        /**
         * @brief Copy constructor for doubleDirectionIterator
         * @param other The iterator to copy from
         */
        doubleDirectionIterator(const doubleDirectionIterator& other);

        /**
         * @brief Copy assignment operator for doubleDirectionIterator
         * @param other The iterator to copy from
         * @return Reference to this iterator
         */
        doubleDirectionIterator& operator=(const doubleDirectionIterator& other);

        /**
         * @brief Creates a heap-allocated copy of the iterator
         * @return A new heap-allocated doubleDirectionIterator
         */
        doubleDirectionIterator* clone() const override;

        /**
         * @brief Checks if the iterator can move backward
         * @return True if the iterator is not at the beginning, otherwise false
         */
        [[nodiscard]] bool hasPrev() const override;

        /**
         * @brief Moves the iterator to the previous element
         * @throws nullPointerError if the iterator is invalid
         */
        void prev() const override;
    };
}

    template<typename TYPE>
    original::doubleDirectionIterator<TYPE>::doubleDirectionIterator(wrapper<TYPE>* ptr)
        : stepIterator<TYPE>(ptr) {}

    template<typename TYPE>
    original::doubleDirectionIterator<TYPE>::doubleDirectionIterator(const doubleDirectionIterator &other)
        : stepIterator<TYPE>(nullptr) {
        this->operator=(other);
    }

    template<typename TYPE>
    original::doubleDirectionIterator<TYPE> & original::doubleDirectionIterator<TYPE>::operator=(
        const doubleDirectionIterator &other) {
        if (this == &other) {
            return *this;
        }
        this->stepIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    bool original::doubleDirectionIterator<TYPE>::hasPrev() const {
        return this->isValid();
    }

    template<typename TYPE>
    void original::doubleDirectionIterator<TYPE>::prev() const {
        if (!this->isValid()) throw nullPointerError();
        this->_ptr = this->_ptr->getPPrev();
    }

    template<typename TYPE>
    auto original::doubleDirectionIterator<TYPE>::clone() const -> doubleDirectionIterator* {
        return new doubleDirectionIterator(*this);
    }

#endif //DOUBLEDIRECTIONITERATOR_H
