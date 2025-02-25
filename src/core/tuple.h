#ifndef TUPLE_H
#define TUPLE_H

#include "printable.h"
#include "comparable.h"
#include "types.h"

/**
 * @file tuple.h
 * @brief Heterogeneous tuple container implementation
 *
 * This file implements a type-safe, compile-time fixed-size tuple container with:
 * - Value semantics
 * - Deep copy/move operations
 * - Index-based element access
 * - Lexicographical comparison
 * - String serialization
 *
 * @note The implementation uses recursive template instantiation for element storage
 */

namespace original{

    /**
     * @class tuple
     * @brief A fixed-size heterogeneous container that stores elements of different types
     * @tparam TYPES... Variadic template parameter list of element types
     * @inherits printable : Provides string representation capabilities
     * @inherits comparable : Provides comparison operations between tuples
     * @details This tuple implementation:
     * - Supports element access via compile-time index
     * - Provides element modification via set() method
     * - Implements lexicographical comparison
     * - Supports deep copying and move semantics
     * - Provides type-safe element storage
     */
    template<typename... TYPES>
    class tuple final : public printable, public comparable<tuple<TYPES...>>{

        /**
         * @class tupleImpl
         * @brief Recursive tuple implementation helper
         * @tparam I Current element index
         * @tparam TS... Element type sequence
         *
         * This template implements the recursive element storage structure:
         * - Stores current element at index I
         * - Recursively contains next element(s)
         * - Provides indexed element access via template recursion
         */
        template<uint32_t I, typename... TS>
        class tupleImpl;

        template<uint32_t I, typename T>
        class tupleImpl<I, T> : public printable, public comparable<tupleImpl<I, T>>{
            T cur_elem;  ///< Current element storage

            /**
             * @brief Constructor of tupleImpl that initializes the current element
             * @param cur The current element to be stored
             */
            explicit tupleImpl(T cur);

            tupleImpl(const tupleImpl<I, T>& other);

            template<uint32_t I_DIFF>
            auto get() const;

            template<uint32_t I_DIFF, typename E>
            void set(const E& e);
        public:
            friend class tuple;
            int64_t compareTo(const tupleImpl<I, T>& other) const override;

            std::string toString(bool enter) const override;
        };

        template<uint32_t I, typename T, typename TS>
        class tupleImpl<I, T, TS> : public printable, public comparable<tupleImpl<I, T, TS>>{
            T cur_elem;                  ///< Current element storage
            tupleImpl<I + 1, TS> next;   ///< Recursive next element storage

            /**
             * @brief Constructor of tupleImpl that initializes the current element
             * @param cur The current element to be stored
             * @param next_elems next element to storage
             */
            explicit tupleImpl(T cur, TS next_elems);

            tupleImpl(const tupleImpl<I, T, TS>& other);

            template<uint32_t I_DIFF>
            auto get() const;

            template<uint32_t I_DIFF, typename E>
            void set(const E& e);
        public:
            friend class tuple;
            int64_t compareTo(const tupleImpl<I, T, TS>& other) const override;

            std::string toString(bool enter) const override;
        };

        template<uint32_t I, typename T, typename... TS>
        class tupleImpl<I, T, TS...> : public printable, public comparable<tupleImpl<I, T, TS...>>{
            T cur_elem;                     ///< Current element storage
            tupleImpl<I + 1, TS...> next;   ///< Recursive next element storage

            /**
             * @brief Constructor of tupleImpl that initializes the current element
             * @param cur The current element to be stored
             * @param next_elems next elements to storage
             */
            explicit tupleImpl(T cur, TS... next_elems);

            tupleImpl(const tupleImpl<I, T, TS...>& other);

            template<uint32_t I_DIFF>
            auto get() const;

            template<uint32_t I_DIFF, typename E>
            void set(const E& e);
        public:
            friend class tuple;
            int64_t compareTo(const tupleImpl<I, T, TS...>& other) const override;

            std::string toString(bool enter) const override;
        };

        tupleImpl<0, TYPES...> elems;  ///< Recursive element storage

    public:
        /**
         * @brief Default constructor
         * @details Value-initializes all elements using their default constructors
         */
        explicit tuple();

        /**
         * @brief Constructs a tuple with given elements
         * @param e... Elements to initialize the tuple with
         */
        explicit tuple(const TYPES&... e);

        /**
         * @brief Copy constructor
         * @param other Tuple to copy from
         */
        tuple(const tuple& other);

        /**
         * @brief Copy assignment operator
         * @param other Tuple to copy from
         * @return Reference to this tuple
         */
        tuple& operator=(const tuple& other);

        /**
         * @brief Move constructor
         * @param other Tuple to move from
         */
        tuple(tuple&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Tuple to move from
         * @return Reference to this tuple
         */
        tuple& operator=(tuple&& other) noexcept;

        /**
         * @brief Get element by index
         * @tparam IDX Zero-based index of the element to retrieve
         * @return Copy of the element at specified index
         * @throw outOfBoundError If index exceeds tuple size
         *
         * @example auto val = myTuple.get<2>();
         */
        template<uint32_t IDX>
        auto get() const;

        /**
         * @brief Set element by index
         * @tparam IDX Zero-based index of the element to modify
         * @tparam E Type of the new value (auto-deduced)
         * @param e New value to assign
         * @throw outOfBoundError If index exceeds tuple size
         * @throw valueError If type E is incompatible with target element type
         *
         * @example myTuple.set<1>(42);
         */
        template<uint32_t IDX, typename E>
        void set(const E& e);

        /**
         * @brief Compare two tuples lexicographically
         * @param other Tuple to compare with
         * @return Comparison result:
         *         - Negative if this tuple is smaller
         *         - Positive if this tuple is larger
         *         - Zero if tuples are equal
         *
         * @details Comparison rules:
         * 1. Compares elements pairwise from first to last
         * 2. Stops at first unequal element pair
         * 3. Elements must be comparable using < operator
         */
        int64_t compareTo(const tuple& other) const override;

        /**
         * @brief Generate formatted string representation
         * @param enter Whether to add newline (implementation specific)
         * @return String in format "tuple(element1, element2, ...)"
         */
        std::string toString(bool enter) const override;

        /**
         * @brief Get class name identifier
         * @return "tuple" string literal
         */
        std::string className() const override;
    };
}

template<typename... TYPES>
template<uint32_t I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(T cur)
    : cur_elem(cur) {}

template<typename... TYPES>
template<uint32_t I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(const tupleImpl<I, T>& other)
    : cur_elem(other.cur_elem) {}

template<typename... TYPES>
template<uint32_t I, typename T>
template<uint32_t I_DIFF>
auto original::tuple<TYPES...>::tupleImpl<I, T>::get() const {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    }
    throw outOfBoundError();
}

template<typename... TYPES>
template<uint32_t I, typename T>
template<uint32_t I_DIFF, typename E>
void original::tuple<TYPES...>::tupleImpl<I, T>::set(const E& e) {
    if constexpr (I_DIFF == 0 && std::same_as<T, E>){
        cur_elem = e;
    }
    if constexpr (I_DIFF != 0)
        throw outOfBoundError();
    throw valueError();
}

template<typename... TYPES>
template<uint32_t I, typename T>
int64_t original::tuple<TYPES...>::tupleImpl<I, T>::compareTo(const tupleImpl<I, T>& other) const {
    if constexpr (Comparable<T>){
        if (cur_elem != other.cur_elem)
            return cur_elem < other.cur_elem ? -1 : 1;
    }
    return 0;
}

template<typename... TYPES>
template<uint32_t I, typename T>
std::string original::tuple<TYPES...>::tupleImpl<I, T>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << cur_elem;
    return ss.str();
}

template<typename... TYPES>
template<uint32_t I, typename T, typename TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS>::tupleImpl(T cur, TS next_elems)
    : cur_elem(cur), next(next_elems) {}

template<typename... TYPES>
template<uint32_t I, typename T, typename TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS>::tupleImpl(const tupleImpl<I, T, TS>& other)
    : cur_elem(other.cur_elem), next(other.next) {}

template<typename... TYPES>
template<uint32_t I, typename T, typename TS>
template<uint32_t I_DIFF>
auto original::tuple<TYPES...>::tupleImpl<I, T, TS>::get() const {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    } else{
        return next.template get<I_DIFF - 1>();
    }
}

template<typename... TYPES>
template<uint32_t I, typename T, typename TS>
template<uint32_t I_DIFF, typename E>
void original::tuple<TYPES...>::tupleImpl<I, T, TS>::set(const E& e) {
    if constexpr (I_DIFF == 0){
        if constexpr (!std::same_as<T, E>)
            throw valueError();
        cur_elem = e;
        return;
    }
    next.template set<I_DIFF - 1, E>(e);
}

template<typename... TYPES>
template<uint32_t I, typename T, typename TS>
int64_t
original::tuple<TYPES...>::tupleImpl<I, T, TS>::compareTo(const tupleImpl<I, T, TS>& other) const {
    if constexpr (Comparable<T>){
        if (cur_elem != other.cur_elem)
            return cur_elem < other.cur_elem ? -1 : 1;
    }
    return next.compareTo(other.next);
}

template<typename... TYPES>
template<uint32_t I, typename T, typename TS>
std::string original::tuple<TYPES...>::tupleImpl<I, T, TS>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << cur_elem;
    ss << next;
    return ss.str();
}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(T cur, TS... next_elems)
    : cur_elem(cur), next(next_elems...) {}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(const tupleImpl<I, T, TS...>& other)
    : cur_elem(other.cur_elem), next(other.next) {}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
template<uint32_t I_DIFF>
auto original::tuple<TYPES...>::tupleImpl<I, T, TS...>::get() const {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    } else{
        return next.template get<I_DIFF - 1>();
    }
}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
template<uint32_t I_DIFF, typename E>
void original::tuple<TYPES...>::tupleImpl<I, T, TS...>::set(const E& e) {
    if constexpr (I_DIFF == 0){
        if constexpr (!std::same_as<T, E>)
            throw valueError();
        cur_elem = e;
        return;
    }
    next.template set<I_DIFF - 1, E>(e);
}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
int64_t
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::compareTo(const tupleImpl<I, T, TS...>& other) const {
    if constexpr (Comparable<T>){
        if (cur_elem != other.cur_elem)
            return cur_elem < other.cur_elem ? -1 : 1;
    }
    return next.compareTo(other.next);
}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
std::string original::tuple<TYPES...>::tupleImpl<I, T, TS...>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << cur_elem;
    ss << next;
    return ss.str();
}

template<typename... TYPES>
original::tuple<TYPES...>::tuple() : tuple(TYPES{}...) {}

template<typename... TYPES>
original::tuple<TYPES...>::tuple(const TYPES&... e) : elems(e...) {}

template<typename... TYPES>
original::tuple<TYPES...>::tuple(const tuple& other) : elems(other.elems) {}

template<typename... TYPES>
original::tuple<TYPES...>& original::tuple<TYPES...>::operator=(const tuple& other) {
    if (this == &other) return *this;
    this->elems = other.elems;
    return *this;
}

template<typename... TYPES>
original::tuple<TYPES...>::tuple(tuple&& other) noexcept
    : elems(std::move(other.elems)) {}

template<typename... TYPES>
original::tuple<TYPES...>& original::tuple<TYPES...>::operator=(tuple&& other) noexcept {
    if (this == &other) return *this;
    this->elems = std::move(other.elems);
    return *this;
}

template<typename... TYPES>
template<uint32_t IDX>
auto original::tuple<TYPES...>::get() const {
    return elems.template get<IDX>();
}

template<typename... TYPES>
template<uint32_t IDX, typename E>
void original::tuple<TYPES...>::set(const E &e) {
    elems.template set<IDX, E>(e);
}

template<typename... TYPES>
int64_t original::tuple<TYPES...>::compareTo(const tuple& other) const {
    return elems.compareTo(other.elems);
}

template<typename... TYPES>
std::string original::tuple<TYPES...>::toString(bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(" << elems << ")";
    return ss.str();
}

template<typename... TYPES>
std::string original::tuple<TYPES...>::className() const {
    return "tuple";
}

#endif //TUPLE_H
