#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "types.h"

/**
 * @file comparator.h
 * @brief Comparator base class and concrete comparator classes
 * @details This file provides a base comparator class and several derived classes for different types of comparisons:
 * - `increaseComparator`: for increasing comparisons (less than)
 * - `decreaseComparator`: for decreasing comparisons (greater than)
 * - `equalComparator`: for equality comparisons (equal to)
 * - `notEqualComparator`: for inequality comparisons (not equal to)
 * - `increaseNotStrictComparator`: for non-strict increasing comparisons (less than or equal to)
 * - `decreaseNotStrictComparator`: for non-strict decreasing comparisons (greater than or equal to)
 */

namespace original
{

    /**
     * @class comparator
     * @tparam TYPE Type of the elements being compared
     * @brief Base class for comparison.
     * @details This class provides the interface for comparing two elements of type `TYPE`.
     *          Derived classes must implement the `compare` method to perform the actual comparison logic.
     */
    template<typename TYPE>
    class comparator{
        public:

        /**
         * @brief Virtual destructor for the comparator class.
         */
        virtual ~comparator() = default;

        /**
         * @brief Pure virtual compare method for comparing two elements.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` has a higher priority than `t2`, otherwise `false`.
         * @details The `compare` method must be implemented in derived classes. It is used to determine the priority
         *          of two elements, where `true` indicates that `t1` is considered to have a higher priority
         *          than `t2`, and `false` means `t2` has higher priority or they are equal.
         */
        virtual bool compare(const TYPE& t1, const TYPE& t2) const = 0;

        /**
         * @brief Function call operator for comparing two elements using the compare method.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return Result depends on @ref comparator::compare().
         */
        bool operator()(const TYPE& t1, const TYPE& t2) const;
    };

    /**
     * @class increaseComparator
     * @tparam TYPE Type of the elements being compared
     * @brief Comparator for increasing comparison (less than).
     * @extends comparator
     * @details This class compares two elements and returns `true` if the first element is less than the second.
     */
    template<typename TYPE>
    class increaseComparator final : public comparator<TYPE> {
    public:
        /**
         * @brief Compares two elements to check if the first is less than the second.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` is less than `t2`, otherwise `false`.
         */
        bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    /**
     * @class decreaseComparator
     * @tparam TYPE Type of the elements being compared
     * @brief Comparator for decreasing comparison (greater than).
     * @extends comparator
     * @details This class compares two elements and returns `true` if the first element is greater than the second.
     */
    template<typename TYPE>
    class decreaseComparator final : public comparator<TYPE> {
    public:
        /**
         * @brief Compares two elements to check if the first is greater than the second.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` is greater than `t2`, otherwise `false`.
         */
        bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    /**
     * @class equalComparator
     * @tparam TYPE Type of the elements being compared
     * @brief Comparator for equality comparison.
     * @extends comparator
     * @details This class compares two elements and returns `true` if the elements are equal.
     */
    template<typename TYPE>
    class equalComparator final : public comparator<TYPE> {
    public:
        /**
         * @brief Compares two elements to check if they are equal.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` is equal to `t2`, otherwise `false`.
         */
        bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    /**
     * @class notEqualComparator
     * @tparam TYPE Type of the elements being compared
     * @brief Comparator for inequality comparison.
     * @extends comparator
     * @details This class compares two elements and returns `true` if the elements are not equal.
     */
    template<typename TYPE>
    class notEqualComparator final : public comparator<TYPE> {
    public:
        /**
         * @brief Compares two elements to check if they are not equal.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` is not equal to `t2`, otherwise `false`.
         */
        bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    /**
     * @class increaseNotStrictComparator
     * @tparam TYPE Type of the elements being compared
     * @brief Comparator for non-strict increasing comparison (less than or equal to).
     * @extends comparator
     * @details This class compares two elements and returns `true` if the first element is less than or equal to the second.
     */
    template<typename TYPE>
    class increaseNotStrictComparator final : public comparator<TYPE> {
    public:
        /**
         * @brief Compares two elements to check if the first is less than or equal to the second.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` is less than or equal to `t2`, otherwise `false`.
         */
        bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    /**
     * @class decreaseNotStrictComparator
     * @tparam TYPE Type of the elements being compared
     * @brief Comparator for non-strict decreasing comparison (greater than or equal to).
     * @extends comparator
     * @details This class compares two elements and returns `true` if the first element is greater than or equal to the second.
     */
    template<typename TYPE>
    class decreaseNotStrictComparator final : public comparator<TYPE> {
    public:
        /**
         * @brief Compares two elements to check if the first is greater than or equal to the second.
         * @param t1 The first element to compare.
         * @param t2 The second element to compare.
         * @return `true` if `t1` is greater than or equal to `t2`, otherwise `false`.
         */
        bool compare(const TYPE& t1, const TYPE& t2) const override;
    };
}

    template <typename TYPE>
    auto original::comparator<TYPE>::operator()(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return this->compare(t1, t2);
    }

    template <typename TYPE>
    auto original::increaseComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 < t2;
    }

    template<typename TYPE>
    auto original::decreaseComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 > t2;
    }

    template <typename TYPE>
    auto original::equalComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 == t2;
    }

    template <typename TYPE>
    auto original::notEqualComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 != t2;
    }

template<typename TYPE>
    auto original::increaseNotStrictComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 < t2 || t1 == t2;
    }


    template <typename TYPE>
    auto original::decreaseNotStrictComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 > t2 || t1 == t2;
    }

#endif //COMPARATOR_H
