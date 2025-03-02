#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <functional>
#include <cmath>
#include "vector.h"
#include "filter.h"
#include "iterator.h"
#include "types.h"

/**
 * @file algorithms.h
 * @brief Standard algorithm implementations for iterator-based containers
 * @details Provides generic algorithms such as range operations, heap operations,
 * element search/count, sorting, and data manipulation functions.
 */

namespace original
{

    /**
     * @class algorithms
     * @brief Utility class containing generic container algorithms
     * @details Provides static methods for iterator-based container operations including:
     * - Range queries (distance, front/back navigation)
     * - Element search/count (find, count, condition checks)
     * - Data manipulation (fill, swap, copy, reverse)
     * - Heap operations (heapify, adjust, priority comparison)
     * - Conditional element processing
     * - Generic sorting algorithm
     */
    class algorithms final
    {
    public:

        /**
         * @brief Calculates distance between two iterators
         * @tparam TYPE Element type
         * @param end Ending iterator
         * @param begin Starting iterator
         * @return integer Number of elements between begin and end (negative if reversed)
         */
        template<typename TYPE>
        static integer distance(const iterator<TYPE>& end, const iterator<TYPE>& begin);

        /**
         * @brief Gets iterator n steps forward
         * @tparam TYPE Element type
         * @param it Base iterator
         * @param steps Number of steps to advance
         * @return iterator<TYPE>* New iterator position
         */
        template<typename TYPE>
        static iterator<TYPE>* frontOf(const iterator<TYPE>& it, integer steps);

        /**
         * @brief Gets iterator n steps backward
         * @tparam TYPE Element type
         * @param it Base iterator
         * @param steps Number of steps to retreat
         * @return iterator<TYPE>* New iterator position
         */
        template<typename TYPE>
        static iterator<TYPE>* backOf(const iterator<TYPE>& it, integer steps);

        /**
         * @brief Checks if all elements satisfy condition
         * @tparam TYPE Element type
         * @tparam Callback Condition callback type
         * @param begin Start iterator
         * @param end End iterator
         * @param condition Validation callback
         * @return bool True if all elements pass condition
         * @requires Condition
         */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static bool allOf(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        /**
         * @brief Checks if any elements satisfy condition
         * @tparam TYPE Element type
         * @tparam Callback Condition callback type
         * @param begin Start iterator
         * @param end End iterator
         * @param condition Validation callback
         * @return bool True if any element passes condition
         * @requires Condition<Callback, TYPE>
         */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static bool anyOf(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        /**
         * @brief Checks if no elements satisfy condition
         * @tparam TYPE Element type
         * @tparam Callback Condition callback type
         * @param begin Start iterator
         * @param end End iterator
         * @param condition Validation callback
         * @return bool True if no elements pass condition
         * @requires Condition<Callback, TYPE>
         */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static bool noneOf(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        /**
         * @brief Find first occurrence of target in iterator range
         * @tparam TYPE Element type
         * @param begin Start iterator
         * @param end End iterator
         * @param target Value to search for
         * @return iterator<TYPE>* Iterator pointing to found element or end
         */
        template<typename TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, const iterator<TYPE> &end, const TYPE &target);

        /**
        * @brief Find first element satisfying condition in iterator range
        * @tparam TYPE Element type
        * @param begin Start iterator
        * @param n Maximum number of elements to search
        * @param target The target element to find
        * @return iterator<TYPE>* Iterator pointing to found element or end
        * @requires Condition<Callback, TYPE>
        */
        template<typename TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, u_integer n, const TYPE &target);

        /**
         * @brief Find first element satisfying condition in iterator range
         * @tparam TYPE Element type
         * @tparam Callback Condition callback type
         * @param begin Start iterator
         * @param end End iterator
         * @param condition Search condition callback
         * @return iterator<TYPE>* Iterator pointing to found element or end
         * @requires Condition<Callback, TYPE>
         */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin,
                                    const iterator<TYPE> &end, const Callback& condition);

        /**
        * @brief Fill range with value (fixed number of elements)
        * @tparam TYPE Element type
        * @tparam Callback Finding condition
        * @param begin Start iterator
        * @param n Number of elements to fill
        * @param condition Condition to find element
        * @return iterator<TYPE>* Iterator past last filled element
        */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, u_integer n, const Callback& condition);

        /**
         * @brief Counts occurrences of a target element in the range
         * @tparam TYPE Element type
         * @param begin Start iterator
         * @param end End iterator
         * @param target Value to count occurrences of
         * @return u_integer The number of occurrences of target in the range
         */
        template<typename TYPE>
        static u_integer count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        /**
         * @brief Counts occurrences of elements satisfying condition in the range
         * @tparam TYPE Element type
         * @tparam Callback Condition callback type
         * @param begin Start iterator
         * @param end End iterator
         * @param condition Validation callback
         * @return u_integer The number of elements satisfying the condition
         * @requires Condition<Callback, TYPE>
         */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static u_integer count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        /**
         * @brief Checks if two iterator ranges are equal
         * @tparam TYPE Element type
         * @param begin1 Start iterator of the first range
         * @param end1 End iterator of the first range
         * @param begin2 Start iterator of the second range
         * @param end2 End iterator of the second range
         * @return bool True if the ranges are equal, false otherwise
         */
        template<typename TYPE>
        static bool equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                          const iterator<TYPE>& begin2, const iterator<TYPE>& end2);

        /**
         * @brief Applies an operation to each element in the range
         * @tparam TYPE Element type
         * @tparam Callback Operation callback type
         * @param begin Start iterator
         * @param end End iterator
         * @param operation Operation to apply to each element
         * @return void
         * @requires Operation<Callback, TYPE>
         */
        template<typename TYPE, typename Callback>
        requires Operation<Callback, TYPE>
        static void forEach(const iterator<TYPE>& begin,
                            const iterator<TYPE>& end, Callback operation);

        /**
         * @brief Applies an operation to the first n elements in the range
         * @tparam TYPE Element type
         * @tparam Callback Operation callback type
         * @param begin Start iterator
         * @param n Number of elements to process
         * @param operation Operation to apply to each element
         * @return iterator<TYPE>* Iterator to the element after the last processed element
         * @requires Operation<Callback, TYPE>
         */
        template<typename TYPE, typename Callback>
        requires Operation<Callback, TYPE>
        static iterator<TYPE>* forEach(const iterator<TYPE> &begin, u_integer n, Callback operation);

        /**
        * @brief Apply operation to elements with condition check
        * @tparam TYPE Element type
        * @tparam Callback_O Operation callback type
        * @tparam Callback_C Condition callback type
        * @param begin Start iterator
        * @param end End iterator
        * @param operation Operation to apply
        * @param condition Filter condition for elements
        * @requires Operation<Callback_O, TYPE> && Condition<Callback_C, TYPE>
        */
        template<typename TYPE, typename Callback_O, typename Callback_C>
        requires Operation<Callback_O, TYPE> && Condition<Callback_C, TYPE>
        static void forEach(const iterator<TYPE>& begin, const iterator<TYPE>& end, Callback_O operation, const Callback_C& condition);

        /**
        * @brief Conditional forEach with element count limit
        * @tparam TYPE Element type
        * @tparam Callback_O Operation callback type
        * @tparam Callback_C Condition callback type
        * @param begin Start iterator
        * @param n Maximum number of elements to process
        * @param operation Operation to apply
        * @param condition Filter condition
        * @return iterator<TYPE>* Iterator past last processed element
        * @requires Operation<Callback_O, TYPE> && Condition<Callback_C, TYPE>
        */
        template<typename TYPE, typename Callback_O, typename Callback_C>
        requires Operation<Callback_O, TYPE> && Condition<Callback_C, TYPE>
        static iterator<TYPE>* forEach(const iterator<TYPE> &begin, u_integer n, Callback_O operation, const Callback_C& condition);

        /**
         * @brief Fills a range with a specific value
         * @tparam TYPE Element type
         * @param begin Start iterator
         * @param end End iterator
         * @param value Value to fill the range with (default constructed if omitted)
         * @return void
         */
        template<typename TYPE>
        static void fill(const iterator<TYPE>& begin,
                         const iterator<TYPE>& end, const TYPE& value = TYPE{});

        /**
        * @brief Fill range with value (fixed number of elements)
        * @tparam TYPE Element type
        * @param begin Start iterator
        * @param n Number of elements to fill
        * @param value Value to fill with (default constructed if omitted)
        * @return iterator<TYPE>* Iterator past last filled element
        */
        template<typename TYPE>
        static iterator<TYPE>* fill(const iterator<TYPE> &begin, u_integer n, const TYPE &value = TYPE{});

        /**
         * @brief Swaps the values of two elements
         * @tparam TYPE Element type
         * @param it1 First element iterator
         * @param it2 Second element iterator
         * @return void
         */
        template<typename TYPE>
        static void swap(const iterator<TYPE>& it1, const iterator<TYPE>& it2) noexcept;

        /**
         * @brief Copies a range of elements from one iterator to another
         * @tparam TYPE Element type
         * @param begin_src Start iterator of source range
         * @param end_src End iterator of source range
         * @param begin_tar Start iterator of target range
         * @return iterator<TYPE>* Iterator pointing past the last copied element
         */
        template<typename TYPE>
        static iterator<TYPE>* copy(const iterator<TYPE> &begin_src, const iterator<TYPE> &end_src,
                                    const iterator<TYPE> &begin_tar);

        /**
        * @brief Conditional copy with filtering
        * @tparam TYPE Element type
        * @tparam Callback Condition callback type
        * @param begin_src Source start iterator
        * @param end_src Source end iterator
        * @param begin_tar Target start iterator
        * @param condition Filter condition for elements
        * @return iterator<TYPE>* Iterator past last copied element
        * @requires Condition<Callback, TYPE>
        */
        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static iterator<TYPE>* copy(const iterator<TYPE> &begin_src, const iterator<TYPE> &end_src,
                                    const iterator<TYPE> &begin_tar, Callback condition = filter<TYPE>{});

        /**
        * @brief Range-based reverse operation
        * @tparam TYPE Element type
        * @param begin Start iterator
        * @param end End iterator
        * @return iterator<TYPE>* Iterator to new end of reversed range
        * @note Performs in-place reversal of elements
        */
        template<typename TYPE>
        static iterator<TYPE>* reverse(const iterator<TYPE> &begin, const iterator<TYPE> &end);

        /**
         * @brief Compares two elements using a comparison callback
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param it1 First iterator
         * @param it2 Second iterator
         * @param compares Comparison callback
         * @return bool True if the elements are considered equal by the comparison
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static bool compare(const iterator<TYPE>& it1, const iterator<TYPE>& it2, const Callback& compares);

        /**
         * @brief Adjusts a heap structure downwards starting from a given iterator
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the heap
         * @param range End iterator of the heap
         * @param current Iterator of the current element to adjust
         * @param compares Comparison callback
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void heapAdjustDown(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                   const iterator<TYPE>& current, const Callback& compares);

        /**
         * @brief Adjusts a heap structure upwards from a given iterator
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the heap
         * @param current Iterator of the current element to adjust
         * @param compares Comparison callback
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void heapAdjustUp(const iterator<TYPE>& begin, const iterator<TYPE>& current,
                                 const Callback& compares);

        /**
         * @brief Initializes a heap structure from a range of iterators
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the heap
         * @param end End iterator of the heap
         * @param compares Comparison callback
         * @return void
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void heapInit(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                             const Callback& compares);

        /**
        * @brief Sorts a range of elements using either stable or unstable sorting algorithm
        * @tparam TYPE Element type
        * @tparam Callback Comparison callback type
        * @param begin Start iterator of the range
        * @param end End iterator of the range
        * @param compares Comparison callback to define the order
        * @param isStable Flag indicating whether to use stable sorting (default: false)
        * @details This function sorts the elements in the range [begin, end] based on the specified comparison callback.
        * When isStable is set to true, it delegates to stable sort (hybrid of mergesort/insertionSort, see @ref stableSort())
        * which preserves the relative order of equivalent elements,
        * suitable for scenarios requiring stability. When false (default), it uses introspective sort
        * (hybrid of quicksort/heapSort/insertionSort, see @ref introSort())
        * which provides O(n log n) worst-case performance but does not preserve element order equality.
        * The choice depends on use-case requirements:
        * Introspective sort (default) is optimal for general-purpose sorting with large datasets
        * Stable sort is explicitly chosen for when stability is required
        */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void sort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                              const Callback& compares, bool isStable = false);

        /**
         * @brief Sorts a range of elements using introspective sort
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the range
         * @param end End iterator of the range
         * @param compares Comparison callback to define the order
         * @details This method sorts the elements in the range [begin, end] into ascending order
         * using an introspective sort (a hybrid of quicksort, heapsort, and insertion sort).
         * The order is determined by the provided comparison function.
         *
         * The algorithm switches to heapsort if the recursion depth exceeds 2*log2(n),
         * ensuring O(n log n) worst-case time complexity. For small partitions (<=16 elements),
         * it uses insertion sort for better cache performance.
         *
         * @note The comparison function must provide strict
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void introSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                               const Callback& compares);

        /**
         * @brief Performs stable hybrid sort on element range
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the range
         * @param end End iterator of the range
         * @param compares Comparison callback for ordering
         * @details Implements a stable sort algorithm (a hybrid of merge sort and insertion sort) with O(n log n) time complexity.
         * - Preserves original order of equivalent elements
         * - Uses insertion sort for small ranges (<=16 elements)
         * - Recursively splits and merges sub-ranges
         * @note Preferred for scenarios requiring stable ordering guarantees
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void stableSort(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                const Callback& compares);

        /**
         * @brief Sorts a range of elements using heap sort
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the range
         * @param end End iterator of the range
         * @param compares Comparison callback to define the order
         * @details This function performs a heap sort on the elements in the range defined by the iterators `begin` and `end`.
         *          It first constructs a heap by calling `heapInit`, and then iteratively swaps the root of the heap with
         *          the last element in the range, re-adjusting the heap after each swap.
         *          The comparison callback `compares` is used to determine the order of elements during the sorting process.
         *          The sorting is performed in-place.
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void heapSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                         const Callback& compares);

        /**
         * @brief Sorts a range of elements using insertion sort
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the range
         * @param end End iterator of the range
         * @param compares Comparison callback to define the order
         * @details This function performs an insertion sort on the elements in the range defined by the iterators `begin` and `end`.
         *          It iterates through the elements starting from the second element, inserting each element into its correct position
         *          relative to the already sorted portion of the range. The comparison callback `compares` is used to determine the
         *          order of the elements during the sorting process. The sorting is performed in-place.
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void insertionSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                  const Callback& compares);
    protected:
        /**
        * @brief Get parent node's priority child in heap structure
        * @tparam TYPE Element type
        * @tparam Callback Comparison callback type
        * @param begin Heap root iterator
        * @param range Valid heap range boundary
        * @param parent Parent node iterator
        * @param compares Comparison callback
        * @return iterator<TYPE>* Iterator to higher-priority child node
        * @requires Compare<Callback, TYPE>
        * @note Protected helper for heap operations
        */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static iterator<TYPE>* _heapGetPrior(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                             const iterator<TYPE>& parent, const Callback& compares);

        /**
         * @brief Selects pivot element for partitioning
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the partition
         * @param end End iterator of the partition
         * @param compares Comparison callback
         * @return iterator<TYPE>* Median-of-three pivot element
         * @private
         * @note Internal helper for introspective sort
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static iterator<TYPE>* _introSortGetPivot(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                                  const Callback& compares);

        /**
         * @brief Partitions elements around selected pivot
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the partition
         * @param end End iterator of the partition
         * @param compares Comparison callback
         * @return iterator<TYPE>* Final pivot position
         * @private
         * @note Internal helper for introspective sort
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static iterator<TYPE>* _introSortPartition(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                                   const Callback& compares);

        /**
         * @brief Hybrid sorting algorithm implementation
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start iterator of the range
         * @param end End iterator of the range
         * @param compares Comparison callback
         * @param depth_limit Current recursion depth limit
         * @private
         * @note Internal implementation detail for introSort
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void _introSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                               const Callback& compares, u_integer depth_limit);

        /**
         * @brief Merges two sorted sub-ranges during merge sort
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Start of first sub-range
         * @param mid Start of second sub-range (end of first)
         * @param end End of second sub-range
         * @param compares Comparison callback
         * @details Key merge step of merge sort algorithm:
         * 1. Creates temporary storage for merged elements
         * 2. Sequentially compares elements from both sub-ranges
         * 3. Copies sorted elements back to original container
         * @note Requires both input sub-ranges to be pre-sorted
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void _stableSortMerge(const iterator<TYPE>& begin, const iterator<TYPE>& mid,
                                     const iterator<TYPE>& end, const Callback& compares);

        /**
         * @brief Recursive implementation of stable sort
         * @tparam TYPE Element type
         * @tparam Callback Comparison callback type
         * @param begin Current sub-range start
         * @param end Current sub-range end
         * @param compares Comparison callback
         * @details Internal recursive method handling:
         * - Base case handling for small ranges
         * - Sub-range division
         * - Recursive sorting of divided ranges
         * - Merge operation invocation
         * @note Not intended for direct external use
         */
        template<typename TYPE, typename Callback>
        requires Compare<Callback, TYPE>
        static void _stableSort(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                const Callback& compares);

    // ---- Implementation of pointer overload version ----

    public:
        /**
         * @brief Pointer overload version of @ref distance()
         * */
        template <typename TYPE>
        static auto distance(const iterator<TYPE>* end, const iterator<TYPE>* begin) -> integer
        {
            return distance(*end, *begin);
        }

        /**
         * @brief Pointer overload version of @ref frontOf()
         * */
        template<typename TYPE>
        static auto frontOf(const iterator<TYPE> *it, integer steps) -> iterator<TYPE> * {
            return frontOf(*it, steps);
        }

        /**
         * @brief Pointer overload version of @ref backOf()
         * */
        template<typename TYPE>
        static auto backOf(const iterator<TYPE> *it, integer steps) -> iterator<TYPE> * {
            return backOf(*it, steps);
        }

        /**
         * @brief Pointer overload version of @ref allOf()
         * */
        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto allOf(const iterator<TYPE> *begin, const iterator<TYPE> *end,
                          const Callback &condition) -> bool {
            return allOf(*begin, *end, condition);
        }

        /**
         * @brief Pointer overload version of @ref anyOf()
         * */
        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto anyOf(const iterator<TYPE> *begin, const iterator<TYPE> *end,
                          const Callback &condition) -> bool {
            return anyOf(*begin, *end, condition);
        }

        /**
         * @brief Pointer overload version of @ref noneOf()
         * */
        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto noneOf(const iterator<TYPE> *begin, const iterator<TYPE> *end,
                           const Callback &condition) -> bool {
            return noneOf(*begin, *end, condition);
        }

        /**
         * @brief Pointer overload version of @ref find()
         * */
        template <typename TYPE>
        static auto find(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                         const TYPE& target) -> iterator<TYPE>* {
            return find(*begin, *end, target);
        }

        /**
         * @brief Pointer overload version of @ref find()
         * */
        template <typename TYPE>
        static auto find(const iterator<TYPE>* begin, u_integer n,
                         const TYPE& target) -> iterator<TYPE>* {
            return find(*begin, n, target);
        }

        /**
         * @brief Pointer overload version of @ref find()
         * */
        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto find(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                         const Callback& condition) -> iterator<TYPE>* {
            return find(*begin, *end, condition);
        }

        /**
         * @brief Pointer overload version of @ref find()
         * */
        template <typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto find(const iterator<TYPE>* begin, u_integer n, const Callback& condition) -> iterator<TYPE>* {
            return find(*begin, n, condition);
        }

        /**
         * @brief Pointer overload version of @ref count()
         * */
        template <typename TYPE>
        static auto count(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                         const TYPE& target) -> u_integer {
            return count(*begin, *end, target);
        }

        /**
         * @brief Pointer overload version of @ref count()
         * */
        template <typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto count(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                         const Callback& condition) -> u_integer {
            return count(*begin, *end, condition);
        }

        /**
         * @brief Pointer overload version of @ref equal()
         * */
        template <typename TYPE>
        static auto equal(const iterator<TYPE>* begin1, const iterator<TYPE>* end1,
                                         const iterator<TYPE>* begin2, const iterator<TYPE>* end2) -> bool {
            return equal(*begin1, *end1, *begin2, *end2);
        }

        /**
         * @brief Pointer overload version of @ref forEach()
         * */
        template <typename TYPE, typename Callback>
        requires original::Operation<Callback, TYPE>
        static auto forEach(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                           Callback operation) -> void {
            forEach(*begin, *end, operation);
        }

        /**
         * @brief Pointer overload version of @ref forEach()
         * */
        template <typename TYPE, typename Callback>
        requires original::Operation<Callback, TYPE>
        static auto forEach(const iterator<TYPE>* begin, u_integer n, Callback operation) -> iterator<TYPE>* {
            return forEach(*begin, n, operation);
        }

        /**
         * @brief Pointer overload version of @ref fill()
         * */
        template<typename TYPE>
        static auto fill(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                        const TYPE& value) -> void {
            fill(*begin, *end, value);
        }

        /**
         * @brief Pointer overload version of @ref fill()
         * */
        template<typename TYPE>
        static auto fill(const iterator<TYPE>* begin, u_integer n, const TYPE& value) -> iterator<TYPE>* {
            return fill(*begin, n, value);
        }

        /**
         * @brief Pointer overload version of @ref swap()
         * */
        template <typename TYPE>
        static auto swap(const iterator<TYPE>* it1, const iterator<TYPE>* it2) noexcept -> void {
            swap(*it1, *it2);
        }

        /**
         * @brief Pointer overload version of @ref copy()
         * */
        template <typename TYPE>
        static auto copy(const iterator<TYPE>* begin_src, const iterator<TYPE>* end_src,
                                        const iterator<TYPE>* begin_tar) -> iterator<TYPE>* {
            return copy(*begin_src, *end_src, *begin_tar);
        }

        /**
         * @brief Pointer overload version of @ref copy()
         * */
        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto copy(const iterator<TYPE>* begin_src, const iterator<TYPE>* end_src,
                                        const iterator<TYPE>* begin_tar, Callback condition) -> iterator<TYPE>* {
            return copy(*begin_src, *end_src, *begin_tar, condition);
        }

        /**
         * @brief Pointer overload version of @ref reverse()
         * */
        template<typename TYPE>
        static auto reverse(const iterator<TYPE>* begin, const iterator<TYPE>* end) -> iterator<TYPE>* {
            return reverse(*begin, *end);
        }

        /**
         * @brief Pointer overload version of @ref compare()
         * */
        template <typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto compare(const iterator<TYPE>* it1, const iterator<TYPE>* it2,
                                           const Callback& compares) -> bool {
            return compare(*it1, *it2, compares);
        }

        /**
         * @brief Pointer overload version of @ref heapAdjustDown()
         * */
        template<typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto heapAdjustDown(const iterator<TYPE>* begin, const iterator<TYPE>* range,
                                                  const iterator<TYPE>* current, const Callback& compares) -> void {
            heapAdjustDown(*begin, *range, *current, compares);
        }

        /**
         * @brief Pointer overload version of @ref heapAdjustUp()
         * */
        template<typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto heapAdjustUp(const iterator<TYPE>* begin, const iterator<TYPE>* current,
                                                const Callback& compares) -> void {
            heapAdjustUp(*begin, *current, compares);
        }

        /**
         * @brief Pointer overload version of @ref heapInit()
         * */
        template<typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto heapInit(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                            const Callback& compares) -> void {
            heapInit(*begin, *end, compares);
        }

        /**
         * @brief Pointer overload version of @ref heapSort()
         * */
        template<typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto heapSort(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                             const Callback& compares) -> void{
            heapSort(*begin, *end, compares);
        }

        /**
         * @brief Pointer overload version of @ref insertionSort()
         * */
        template<typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto insertionSort(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                             const Callback& compares) -> void{
            insertionSort(*begin, *end, compares);
        }

    protected:
        /**
         * @brief Pointer overload version of @ref _heapGetPrior()
         * */
        template<typename TYPE, typename Callback>
        requires original::Compare<Callback, TYPE>
        static auto _heapGetPrior(const iterator<TYPE>* begin, const iterator<TYPE>* range,
                                  const iterator<TYPE>* parent, const Callback& compares) -> iterator<TYPE>* {
            return _heapGetPrior(*begin, *range, *parent, compares);
        }
    };
}

    template <typename TYPE>
    auto original::algorithms::distance(const iterator<TYPE>& end, const iterator<TYPE>& begin) -> integer
    {
        auto* it1 = end.clone();
        auto* it2 = begin.clone();
        auto dis = it1->operator-(*it2);
        delete it1;
        delete it2;
        return dis;
    }

    template<typename TYPE>
    original::iterator<TYPE> *original::algorithms::frontOf(const iterator<TYPE> &it, integer steps) {
        return it + steps;
    }

    template<typename TYPE>
    original::iterator<TYPE> *original::algorithms::backOf(const iterator<TYPE> &it, integer steps) {
        return it - steps;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    bool original::algorithms::allOf(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                     const Callback& condition) {
        auto* it = begin.clone();
        for (; distance(*it, end) <= 0; it->next()){
            if (!condition(it->get())){
                delete it;
                return false;
            }
        }
        delete it;
        return true;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    bool original::algorithms::anyOf(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                     const Callback& condition) {
        auto* it = begin.clone();
        for (; distance(*it, end) <= 0; it->next()){
            if (condition(it->get())){
                delete it;
                return true;
            }
        }
        delete it;
        return false;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    bool original::algorithms::noneOf(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                      const Callback& condition) {
        auto* it = begin.clone();
        for (; distance(*it, end) <= 0; it->next()){
            if (condition(it->get())){
                delete it;
                return false;
            }
        }
        delete it;
        return true;
    }

    template <typename TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> iterator<TYPE>* {
        auto it = begin.clone();
        while (it->isValid() && !it->equal(end)) {
            if (it->get() == target) {
                return it;
            }
            it->next();
        }
        delete it;
        return end.clone();
    }

    template <typename TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const u_integer n, const TYPE& target) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (u_integer i = 0; i < n; i += 1, it->next())
        {
            if (it->get() == target) return it;
        }
        return it;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::find(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                    const Callback& condition) -> iterator<TYPE>* {
        auto it = begin.clone();
        while (it->isValid() && !it->equal(end)) {
            if (condition(it->get())) {
                return it;
            }
            it->next();
        }
        delete it;
        return end.clone();
    }

    template <typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const u_integer n, const Callback& condition) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (u_integer i = 0; i < n; i += 1, it->next())
        {
            if (condition(it->get())) return it;
        }
        return it;
    }

    template <typename TYPE>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> u_integer
    {
        u_integer cnt = 0;
        auto it = begin.clone();
        while (it->isValid() && !end.atPrev(it)) {
            if (it->get() == target) {
                cnt += 1;
            }
            it->next();
        }
        delete it;
        return cnt;
    }

    template <typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                     const Callback& condition) -> u_integer
    {
        u_integer cnt = 0;
        auto it = begin.clone();
        while (it->isValid() && !end.atPrev(it)) {
            if (condition(it->get())) {
                cnt += 1;
            }
            it->next();
        }
        delete it;
        return cnt;
    }

    template <typename TYPE>
    auto original::algorithms::equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                                     const iterator<TYPE>& begin2, const iterator<TYPE>& end2) -> bool
    {
        auto it1 = begin1.clone();
        auto it2 = begin2.clone();

        while (it1->isValid() && it2->isValid() && !it1->equal(end1) && !it2->equal(end2)) {
            it1->next();
            it2->next();
        }
        const bool res = it1->equal(end1) && it2->equal(end2) && it1->get() == it2->get();
        delete it1;
        delete it2;
        return res;
    }

    template <typename TYPE, typename Callback>
    requires original::Operation<Callback, TYPE>
    auto original::algorithms::forEach(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                       Callback operation) -> void
    {
        auto it = begin.clone();
        for (; !it->equal(end); it->next()) {
            operation(it->get());
        }
        operation(it->get());
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Operation<Callback, TYPE>
    auto original::algorithms::forEach(const iterator<TYPE>& begin, const u_integer n,
                                       Callback operation) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (u_integer i = 0; i < n; i += 1, it->next())
        {
            operation(it->get());
        }
        return it;
    }

    template<typename TYPE, typename Callback_O, typename Callback_C>
    requires original::Operation<Callback_O, TYPE> && original::Condition<Callback_C, TYPE>
    auto original::algorithms::forEach(const iterator<TYPE> &begin, const iterator<TYPE> &end, Callback_O operation,
                                       const Callback_C &condition) -> void {
        auto it = begin.clone();
        for (; !it->equal(end); it->next()) {
            if (condition(it->get()))
                operation(it->get());
        }
        operation(it->get());
        delete it;
    }

    template<typename TYPE, typename Callback_O, typename Callback_C>
    requires original::Operation<Callback_O, TYPE> && original::Condition<Callback_C, TYPE>
    auto original::algorithms::forEach(const iterator<TYPE> &begin, const u_integer n, Callback_O operation,
                                       const Callback_C &condition) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (u_integer i = 0; i < n; i += 1, it->next())
        {
            if (condition(it->get()))
                operation(it->get());
        }
        return it;
    }

    template<typename TYPE>
    auto original::algorithms::fill(const iterator<TYPE>& begin,
                                    const iterator<TYPE>& end, const TYPE& value) -> void
    {
        auto it = begin.clone();
        while (!it->equal(end)){
            it->set(value);
            it->next();
        }
        it->set(value);
        delete it;
    }

    template<typename TYPE>
    auto original::algorithms::fill(const iterator<TYPE>& begin,
                                    const u_integer n, const TYPE& value) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (u_integer i = 0; i < n; ++i) {
            it->set(value);
            it->next();
        }
        return it;
    }

    template <typename TYPE>
    auto original::algorithms::swap(const iterator<TYPE>& it1, const iterator<TYPE>& it2) noexcept -> void
    {
        auto it_1 = it1.clone();
        auto it_2 = it2.clone();
        TYPE tmp = it_2->get();
        it_2->set(it_1->get());
        it_1->set(tmp);
        delete it_1;
        delete it_2;
    }

    template <typename TYPE>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                        const iterator<TYPE>& begin_tar) -> iterator<TYPE>* {
        auto it_src = begin_src.clone();
        auto it_tar = begin_tar.clone();
        while (!it_src->equal(end_src)){
            it_tar->set(it_src->get());
            it_src->next();
            it_tar->next();
        }
        it_tar->set(it_src->get());
        it_tar->next();
        delete it_src;
        return it_tar;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                               const iterator<TYPE>& begin_tar, Callback condition) -> iterator<TYPE>* {
        auto it_src = begin_src.clone();
        auto it_tar = begin_tar.clone();
        while (!it_src->equal(end_src)){
            if (condition(it_src->get())){
                it_tar->set(it_src->get());
            }
            it_src->next();
            it_tar->next();
        }
        if (condition(it_src->get())){
            it_tar->set(it_src->get());
        }
        it_tar->next();
        delete it_src;
        return it_tar;
    }

    template<typename TYPE>
    auto original::algorithms::reverse(const iterator<TYPE>& begin,
                                       const iterator<TYPE>& end) -> iterator<TYPE>* {
        auto left = begin.clone();
        auto right = end.clone();
        while (!left->equal(right) && !left->atNext(right)){
            swap(left, right);
            left->next();
            right->prev();
        }
        delete right;
        return left;
    }

    template <typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    auto original::algorithms::compare(const iterator<TYPE>& it1, const iterator<TYPE>& it2,
                                       const Callback& compares) -> bool
    {
        return compares(it1.get(), it2.get());
    }

    template <typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    auto original::algorithms::heapAdjustDown(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                              const iterator<TYPE>& current, const Callback& compares) -> void
    {
        if (distance(current, begin) < 0)
            return;

        auto* it = current.clone();
        while ((distance(*it, begin) + 1) * 2 - 1 <= distance(range, begin))
        {
            auto* child = _heapGetPrior(begin, range, *it, compares);
            if (compare(it, child, compares))
            {
                delete child;
                break;
            }
            swap(it, child);
            delete it;
            it = child->clone();
            delete child;
        }
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    auto original::algorithms::heapAdjustUp(const iterator<TYPE>& begin, const iterator<TYPE>& current,
                                            const Callback& compares) -> void
    {
        auto* it = current.clone();
        while (distance(*it, begin) > 0)
        {
            auto* parent = frontOf(begin, (distance(*it, begin) + 1) / 2 - 1);
            if (compare(it, parent, compares))
            {
                swap(it, parent);
                delete it;
                it = parent->clone();
            }else
            {
                delete parent;
                break;
            }
            delete parent;
        }
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    auto original::algorithms::heapInit(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                        const Callback& compares) -> void
    {
        auto* it = frontOf(begin, (distance(end, begin) + 1) / 2 - 1);
        for (; distance(*it, begin) >= 0; it->prev())
        {
            heapAdjustDown(begin, end, *it, compares);
        }
        delete it;
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::sort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                    const Callback &compares, const bool isStable) {
        isStable ? stableSort(begin, end, compares) : introSort(begin, end, compares);
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::introSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                         const Callback &compares) {
        if (const integer dis = distance(end, begin); dis <= 0)
            return;

        u_integer depth_limit = 2 * std::log2(distance(end, begin));
        _introSort(begin, end, compares, depth_limit);
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::stableSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                          const Callback &compares) {
        _stableSort(begin, end, compares);
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::heapSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                        const Callback& compares) {
        if (distance(end, begin) <= 0)
            return;

        heapInit(begin, end, std::not_fn(compares));
        auto* right = end.clone();
        while (distance(*right, begin) > 0){
            swap(begin, *right);
            right->prev();
            heapAdjustDown(begin, *right, begin, std::not_fn(compares));
        }
        delete right;
    }

    template <typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    auto original::algorithms::_heapGetPrior(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                             const iterator<TYPE>& parent, const Callback& compares) -> iterator<TYPE>*
    {
        if ((distance(parent, begin) + 1) * 2 <= distance(range, begin))
        {
            auto* left = frontOf(begin, (distance(parent, begin) + 1) * 2 - 1);
            auto* right = frontOf(begin, (distance(parent, begin) + 1) * 2);
            if (compare(left, right, compares))
            {
                delete right;
                return left;
            }
            delete left;
            return right;
        }
        return frontOf(begin, (distance(parent, begin) + 1) * 2 - 1);
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    original::iterator<TYPE>*
    original::algorithms::_introSortGetPivot(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                             const Callback &compares) {
        auto* mid = frontOf(begin, distance(end, begin) / 2);
        if ((!compare(begin, *mid, compares) && !compare(end, begin, compares))
         || (!compare(*mid, begin, compares) && !compare(begin, end, compares))){
            delete mid;
            return begin.clone();
        }
        if ((!compare(*mid, begin, compares) && !compare(end, *mid, compares))
            || (!compare(begin, *mid, compares) && !compare(*mid, end, compares))){
            return mid;
        }
        delete mid;
        return end.clone();
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    original::iterator<TYPE>*
    original::algorithms::_introSortPartition(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                              const Callback &compares) {
        auto* left = begin.clone();
        auto* right = end.clone();
        auto* pivot = _introSortGetPivot(begin, end, compares);
        TYPE tmp = left->getElem();
        left->set(pivot->getElem());
        delete pivot;
        bool move_right = true;
        while (distance(right, left) > 0) {
            if (move_right){
                if (compare(right, left, std::not_fn(compares))){
                    right->prev();
                } else{
                    swap(left, right);
                    move_right = false;
                }
            } else{
                if (compare(right, left, std::not_fn(compares))){
                    left->next();
                } else{
                    swap(left, right);
                    move_right = true;
                }
            }
        }
        delete right;
        left->set(tmp);
        return left;
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void
    original::algorithms::_introSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                     const Callback &compares, u_integer depth_limit) {
        if (distance(end, begin) <= 16) {
            insertionSort(begin, end, compares);
            return;
        }

        if (depth_limit == 0) {
            heapSort(begin, end, compares);
            return;
        }

        auto* pivot = _introSortPartition(begin, end, compares);
        _introSort(begin, *pivot, compares, depth_limit - 1);
        _introSort(*pivot, end, compares, depth_limit - 1);
        delete pivot;
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::insertionSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                             const Callback &compares) {
        if (distance(end, begin) <= 0)
            return;

        auto* left = frontOf(begin, 1);
        while (distance(end, *left) >= 0) {
            auto* current = left->clone();
            auto* prev = backOf(current, 1);
            while (distance(*current, begin) > 0 && compare(current, prev, compares)){
                swap(current, prev);
                current->prev();
                prev->prev();
            }
            delete prev;
            delete current;
            left->next();
        }
        delete left;
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::_stableSortMerge(const iterator<TYPE> &begin, const iterator<TYPE> &mid,
                                                const iterator<TYPE> &end, const Callback &compares) {
        vector<TYPE> tmp;
        auto* left = begin.clone();
        auto* right = mid.clone();

        while (distance(mid, *left) > 0 && distance(end, *right) > 0){
            if (compare(left, right, compares)){
                tmp.pushEnd(left->get());
                left->next();
            } else{
                tmp.pushEnd(right->get());
                right->next();
            }
        }

        while (distance(mid, *left) > 0){
            tmp.pushEnd(left->get());
            left->next();
        }

        while (distance(end, *right) > 0){
            tmp.pushEnd(right->get());
            right->next();
        }

        copy(tmp.first(), tmp.last(), begin);
        delete right;
        delete left;
    }

    template<typename TYPE, typename Callback>
    requires original::Compare<Callback, TYPE>
    void original::algorithms::_stableSort(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                           const Callback &compares) {
        const integer dis = distance(end, begin);

        if (dis <= 1)
            return;

        if (dis <= 16){
            insertionSort(begin, end, compares);
            return;
        }

        auto* mid = frontOf(begin, dis / 2);
        _stableSort(begin, *mid, compares);
        _stableSort(*mid, end, compares);
        _stableSortMerge(begin, *mid, end, compares);
        delete mid;
    }

#endif // ALGORITHMS_H
