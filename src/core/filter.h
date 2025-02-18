#ifndef FILTER_H
#define FILTER_H
#include "cloneable.h"

/**
 * @file filter.h
 * @brief Filter base class and derived filter classes for various matching operations.
 * @details This file provides the definition of a base class `filter` for matching elements based on specific conditions,
 *          and derived classes that implement different types of filters, such as equality, inequality, range checking, etc.
 *          The filter classes include:
 *          - `equalFilter`: checks if an element is equal to a target value.
 *          - `notEqualFilter`: checks if an element is not equal to a target value.
 *          - `lessFilter`: checks if an element is less than a target value.
 *          - `greaterFilter`: checks if an element is greater than a target value.
 *          - `notLessFilter`: checks if an element is greater than or equal to a target value.
 *          - `notGreaterFilter`: checks if an element is less than or equal to a target value.
 *          - `rangeFilter`: checks if an element lies within a given range.
 */

namespace original {

    /**
     * @class filter
     * @tparam TYPE Type of the element to match
     * @brief Base class for filter operations.
     * @details This class provides the interface for performing match operations on an element of type `TYPE`.
     *          Derived classes should implement the `match` method for specific matching logic.
     *          The `operator()` method is used to invoke the `match` method.
     */
    template<typename TYPE>
    class filter : public cloneable {
    protected:
        /**
         * @brief Determines if an element matches the filter condition.
         * @param t The element to check.
         * @return `true` if the element matches the filter, `false` otherwise.
         */
        virtual bool match(const TYPE& t) const;

    public:
        /**
         * @brief Virtual destructor for the filter class.
         */
        ~filter() override = default;

        /**
         * @brief Clones the filter object.
         * @return A pointer to a new instance of the filter object.
         */
        filter* clone() const override;

        /**
         * @brief Applies the filter to the element.
         * @param t The element to check.
         * @return `true` if the element matches the filter, `false` otherwise.
         */
        bool operator()(const TYPE& t) const;
    };

    /**
     * @class equalFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element is equal to a target value.
     * @extends filter
     * @details This filter compares an element to a target value and returns `true` if the element is equal to the target.
     */
    template<typename TYPE>
    class equalFilter final : public filter<TYPE> {
        TYPE target; ///< The target value to compare the element with

        /**
         * @brief Checks if the element is equal to the target value.
         * @param t The element to check.
         * @return `true` if the element is equal to the target value, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs an equalFilter with the target value.
         * @param target The value to compare elements against.
         */
        explicit equalFilter(const TYPE& target);

        /**
         * @brief Clones the equalFilter object.
         * @return A pointer to a new instance of equalFilter.
         */
        equalFilter* clone() const override;
    };

    /**
     * @class notEqualFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element is not equal to a target value.
     * @extends filter
     * @details This filter compares an element to a target value and returns `true` if the element is not equal to the target.
     */
    template<typename TYPE>
    class notEqualFilter final : public filter<TYPE> {
        TYPE target; ///< The target value to compare the element with

        /**
         * @brief Checks if the element is not equal to the target value.
         * @param t The element to check.
         * @return `true` if the element is not equal to the target value, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs a notEqualFilter with the target value.
         * @param target The value to compare elements against.
         */
        explicit notEqualFilter(const TYPE& target);

        /**
         * @brief Clones the notEqualFilter object.
         * @return A pointer to a new instance of notEqualFilter.
         */
        notEqualFilter* clone() const override;
    };

    /**
     * @class lessFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element is less than a target value.
     * @extends filter
     * @details This filter compares an element to a target value and returns `true` if the element is less than the target.
     */
    template<typename TYPE>
    class lessFilter final : public filter<TYPE> {
        TYPE low; ///< The target value to compare the element with

        /**
         * @brief Checks if the element is less than the target value.
         * @param t The element to check.
         * @return `true` if the element is less than the target value, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs a lessFilter with the target value.
         * @param low The value to compare elements against.
         */
        explicit lessFilter(const TYPE& low);

        /**
         * @brief Clones the lessFilter object.
         * @return A pointer to a new instance of lessFilter.
         */
        lessFilter* clone() const override;
    };

    /**
     * @class greaterFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element is greater than a target value.
     * @extends filter
     * @details This filter compares an element to a target value and returns `true` if the element is greater than the target.
     */
    template<typename TYPE>
    class greaterFilter final : public filter<TYPE> {
        TYPE high; ///< The target value to compare the element with

        /**
         * @brief Checks if the element is greater than the target value.
         * @param t The element to check.
         * @return `true` if the element is greater than the target value, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs a greaterFilter with the target value.
         * @param high The value to compare elements against.
         */
        explicit greaterFilter(const TYPE& high);

        /**
         * @brief Clones the greaterFilter object.
         * @return A pointer to a new instance of greaterFilter.
         */
        greaterFilter* clone() const override;
    };

    /**
     * @class notLessFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element is greater than or equal to a target value.
     * @extends filter
     * @details This filter compares an element to a target value and returns `true` if the element is greater than or equal to the target.
     */
    template<typename TYPE>
    class notLessFilter final : public filter<TYPE> {
        TYPE high; ///< The target value to compare the element with

        /**
         * @brief Checks if the element is greater than or equal to the target value.
         * @param t The element to check.
         * @return `true` if the element is greater than or equal to the target value, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs a notLessFilter with the target value.
         * @param high The value to compare elements against.
         */
        explicit notLessFilter(const TYPE& high);

        /**
         * @brief Clones the notLessFilter object.
         * @return A pointer to a new instance of notLessFilter.
         */
        notLessFilter* clone() const override;
    };

    /**
     * @class notGreaterFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element is less than or equal to a target value.
     * @extends filter
     * @details This filter compares an element to a target value and returns `true` if the element is less than or equal to the target.
     */
    template<typename TYPE>
    class notGreaterFilter final : public filter<TYPE> {
        TYPE low; ///< The target value to compare the element with

        /**
         * @brief Checks if the element is less than or equal to the target value.
         * @param t The element to check.
         * @return `true` if the element is less than or equal to the target value, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs a notGreaterFilter with the target value.
         * @param low The value to compare elements against.
         */
        explicit notGreaterFilter(const TYPE& low);

        /**
         * @brief Clones the notGreaterFilter object.
         * @return A pointer to a new instance of notGreaterFilter.
         */
        notGreaterFilter* clone() const override;
    };

    /**
     * @class rangeFilter
     * @tparam TYPE Type of the element to match
     * @brief A filter that checks if an element lies within a specified range.
     * @extends filter
     * @details This filter compares an element to a lower and upper bound, and returns `true` if the element is within the range.
     */
    template<typename TYPE>
    class rangeFilter final : public filter<TYPE> {
        TYPE low;  ///< The lower bound of the range
        TYPE high; ///< The upper bound of the range

        /**
         * @brief Checks if the element lies within the range.
         * @param t The element to check.
         * @return `true` if the element lies within the range, `false` otherwise.
         */
        bool match(const TYPE& t) const override;

    public:
        /**
         * @brief Constructs a rangeFilter with the specified range.
         * @param low The lower bound of the range.
         * @param high The upper bound of the range.
         */
        explicit rangeFilter(const TYPE& low, const TYPE& high);

        /**
         * @brief Clones the rangeFilter object.
         * @return A pointer to a new instance of rangeFilter.
         */
        rangeFilter* clone() const override;
    };

} // namespace original

    template<typename TYPE>
    bool original::filter<TYPE>::match(const TYPE&) const {
        return true;
    }

    template <typename TYPE>
    auto original::filter<TYPE>::clone() const -> filter*
    {
        return new filter(*this);
    }

    template<typename TYPE>
    bool original::filter<TYPE>::operator()(const TYPE& t) const {
        return this->match(t);
    }

    template<typename TYPE>
    bool original::equalFilter<TYPE>::match(const TYPE& t) const {
        return t == target;
    }

    template<typename TYPE>
    original::equalFilter<TYPE>::equalFilter(const TYPE& target)
            : target(target) {}

    template <typename TYPE>
    auto original::equalFilter<TYPE>::clone() const -> equalFilter*
    {
        return new equalFilter(*this);
    }

    template<typename TYPE>
    bool original::notEqualFilter<TYPE>::match(const TYPE& t) const {
        return t != target;
    }

    template<typename TYPE>
    original::notEqualFilter<TYPE>::notEqualFilter(const TYPE& target)
            : target(target) {}

    template <typename TYPE>
    auto original::notEqualFilter<TYPE>::clone() const -> notEqualFilter*
    {
        return new notEqualFilter(*this);
    }

    template<typename TYPE>
    bool original::lessFilter<TYPE>::match(const TYPE& t) const {
        return t < low;
    }

    template<typename TYPE>
    original::lessFilter<TYPE>::lessFilter(const TYPE& low)
            : low(low) {}

    template <typename TYPE>
    auto original::lessFilter<TYPE>::clone() const -> lessFilter*
    {
        return new lessFilter(*this);
    }

    template<typename TYPE>
    bool original::greaterFilter<TYPE>::match(const TYPE& t) const {
        return t > high;
    }

    template<typename TYPE>
    original::greaterFilter<TYPE>::greaterFilter(const TYPE& high)
            : high(high) {}

    template <typename TYPE>
    auto original::greaterFilter<TYPE>::clone() const -> greaterFilter*
    {
        return new greaterFilter(*this);
    }

    template<typename TYPE>
    bool original::notLessFilter<TYPE>::match(const TYPE& t) const {
        return t >= high;
    }

    template<typename TYPE>
    original::notLessFilter<TYPE>::notLessFilter(const TYPE& high)
            : high(high) {}

    template <typename TYPE>
    auto original::notLessFilter<TYPE>::clone() const -> notLessFilter*
    {
        return new notLessFilter(*this);
    }

    template<typename TYPE>
    bool original::notGreaterFilter<TYPE>::match(const TYPE& t) const {
        return t <= low;
    }

    template<typename TYPE>
    original::notGreaterFilter<TYPE>::notGreaterFilter(const TYPE& low)
            : low(low) {}

    template <typename TYPE>
    auto original::notGreaterFilter<TYPE>::clone() const -> notGreaterFilter*
    {
        return new notGreaterFilter(*this);
    }

    template<typename TYPE>
    bool original::rangeFilter<TYPE>::match(const TYPE& t) const {
        return t >= low && t <= high;
    }

    template<typename TYPE>
    original::rangeFilter<TYPE>::rangeFilter(const TYPE& low, const TYPE& high)
            : low(low), high(high) {}

    template <typename TYPE>
    auto original::rangeFilter<TYPE>::clone() const -> rangeFilter*
    {
        return new rangeFilter(*this);
    }

#endif // FILTER_H
