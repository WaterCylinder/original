#ifndef SINGLEDIRECTIONITERATOR_H
#define SINGLEDIRECTIONITERATOR_H

#include "stepIterator.h"

/**
 * @file singleDirectionIterator.h
 * @brief Single-direction iterator base class
 * @details Provides common functionality for iterators that can only traverse in one direction (forward).
 *          It extends the `stepIterator` class to include functionality that applies to single-direction step iterators.
 */

namespace original {

    /**
     * @class singleDirectionIterator
     * @tparam TYPE Type of elements being iterated
     * @brief Abstract base class for single-direction iterators
     * @extends stepIterator
     * @details Implements core functionality for iterators that support single-direction traversal.
     *          It extends `stepIterator` and provides a base for all iterators that traverse elements
     *          one step at a time in a single direction (typically forward).
     */
    template<typename TYPE>
    class singleDirectionIterator : public stepIterator<TYPE> {
    protected:
        /**
         * @brief Protected constructor for singleDirectionIterator
         * @param ptr Raw pointer to the element being iterated
         * @param pos Initial position index
         */
        explicit singleDirectionIterator(wrapper<TYPE>* ptr, int64_t pos);

    public:
        /**
         * @brief Copy constructor for singleDirectionIterator
         * @param other The iterator to copy from
         */
        singleDirectionIterator(const singleDirectionIterator& other);

        /**
         * @brief Copy assignment operator for singleDirectionIterator
         * @param other The iterator to copy from
         * @return A reference to this iterator
         */
        singleDirectionIterator& operator=(const singleDirectionIterator& other);

        /**
         * @brief Creates a heap-allocated copy of the iterator
         * @return A new heap-allocated singleDirectionIterator
         */
        singleDirectionIterator* clone() const override;
    };
}

    template<typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(wrapper<TYPE>* ptr, int64_t pos)
        : stepIterator<TYPE>(ptr, pos) {}

    template<typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(const singleDirectionIterator &other)
        : stepIterator<TYPE>(nullptr, 0) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::singleDirectionIterator<TYPE>::operator=(
        const singleDirectionIterator &other) -> singleDirectionIterator& {
        if (this == &other) {
            return *this;
        }
        stepIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::singleDirectionIterator<TYPE>::clone() const -> singleDirectionIterator* {
        return new singleDirectionIterator(*this);
    }

#endif //SINGLEDIRECTIONITERATOR_H
