#ifndef TUPLE_H
#define TUPLE_H

#include "printable.h"
#include "comparable.h"
#include "couple.h"
#include "types.h"

/**
 * @file tuple.h
 * @brief Heterogeneous tuple container implementation
 *
 * This file implements a type-safe, compile-time fixed-size tuple container with:
 * - Element access by index
 * - Element modification
 * - Equality comparison
 * - Formatted output
 * - Move semantics
 * - Structured binding support
 * - Tuple concatenation and slicing
 *
 * @note Structured binding is enabled via `std::tuple_size` and `std::tuple_element` specializations.
 *       Value type deduction is used to ensure compatibility with `const auto& [a, b, c] = myTuple` syntax.
 *
 * @example
 * original::tuple<int, std::string> t(10, "hi");
 * int x = t.get<0>(); // access
 * t.set<1>(std::string("hello")); // modify
 *
 * const auto& [a, b] = t; // structured binding
 */

namespace original{

    /**
     * @class tuple
     * @tparam TYPES Variadic template parameter list of element types
     * @brief Container for multiple heterogeneous elements
     * @extends printable
     * @extends comparable
     * @details Stores a sequence of elements with type safety. Provides:
     * - Compile-time fixed size container
     * - Element access via index (compile-time checked)
     * - Element modification via index
     * - Deep copy/move operations
     * - Lexicographical comparison
     * - String serialization through printable interface
     * - Comparable interface implementation
     * - Structured binding support via std::tuple_size and std::tuple_element specializations
     * - Tuple concatenation and slicing operations
     */
    template<typename... TYPES>
    class tuple final : public printable, public comparable<tuple<TYPES...>>{
        static constexpr u_integer SIZE = sizeof...(TYPES);

        /**
         * @class tupleImpl
         * @brief Recursive tuple implementation helper
         * @tparam I Current element index
         * @tparam TS Element type sequence
         *
         * This template implements the recursive element storage structure:
         * - Stores current element at index I
         * - Recursively contains next element(s)
         * - Provides indexed element access via template recursion
         */
        template<u_integer I, typename... TS>
        class tupleImpl;

        template<u_integer I, typename T>
        class tupleImpl<I, T> : public printable, public comparable<tupleImpl<I, T>>{
            T cur_elem;  ///< Current element storage

            /**
             * @brief Constructor of tupleImpl that initializes the current element
             * @param cur The current element to be stored
             */
            explicit tupleImpl(const T& cur = T{});

            tupleImpl(tupleImpl<I, T>&& other) noexcept;

            tupleImpl(const tupleImpl<I, T>& other);

            tupleImpl<I, T>& operator=(tupleImpl<I, T>&& other) noexcept;

            template<u_integer I_DIFF>
            const auto& get() const;

            template<u_integer I_DIFF>
            auto& get();

            template<u_integer I_DIFF, typename E>
            void set(const E& e);
        public:
            friend class tuple;
            integer compareTo(const tupleImpl<I, T>& other) const override;

            std::string toString(bool enter) const override;
        };

        template<u_integer I, typename T, typename TS>
        class tupleImpl<I, T, TS> : public printable, public comparable<tupleImpl<I, T, TS>>{
            T cur_elem;                  ///< Current element storage
            tupleImpl<I + 1, TS> next;   ///< Recursive next element storage

            /**
             * @brief Constructor of tupleImpl that initializes the current element
             * @param cur The current element to be stored
             * @param next_elems next element to storage
             */
            explicit tupleImpl(const T& cur = T{}, const TS& next_elems = TS{});

            tupleImpl(tupleImpl<I, T, TS>&& other) noexcept;

            tupleImpl(const tupleImpl<I, T, TS>& other);

            tupleImpl<I, T, TS>& operator=(tupleImpl<I, T, TS>&& other) noexcept;

            template<u_integer I_DIFF>
            const auto& get() const;

            template<u_integer I_DIFF>
            auto& get();

            template<u_integer I_DIFF, typename E>
            void set(const E& e);
        public:
            friend class tuple;
            integer compareTo(const tupleImpl<I, T, TS>& other) const override;

            std::string toString(bool enter) const override;
        };

        template<u_integer I, typename T, typename... TS>
        class tupleImpl<I, T, TS...> : public printable, public comparable<tupleImpl<I, T, TS...>>{
            T cur_elem;                     ///< Current element storage
            tupleImpl<I + 1, TS...> next;   ///< Recursive next element storage

            /**
             * @brief Constructor of tupleImpl that initializes the current element
             * @param cur The current element to be stored
             * @param next_elems next elements to storage
             */
            explicit tupleImpl(const T& cur, const TS&... next_elems);

            explicit tupleImpl(const T& cur = T{}, const tupleImpl<I + 1, TS...>& nt = tupleImpl<I + 1, TS...>{});

            tupleImpl(tupleImpl<I, T, TS...>&& other) noexcept;

            tupleImpl(const tupleImpl<I, T, TS...>& other);

            tupleImpl<I, T, TS...>& operator=(tupleImpl<I, T, TS...>&& other) noexcept;

            template<u_integer I_DIFF>
            const auto& get() const;

            template<u_integer I_DIFF>
            auto& get();

            template<u_integer I_DIFF, typename E>
            void set(const E& e);
        public:
            friend class tuple;
            integer compareTo(const tupleImpl<I, T, TS...>& other) const override;

            std::string toString(bool enter) const override;
        };

        tupleImpl<0, TYPES...> elems;  ///< Recursive element storage

        /**
         * @brief Internal helper to slice a tuple from a specific index with given indices
         * @tparam IDX_S Compile-time index sequence (used to construct elements)
         * @tparam BEGIN_IDX The beginning index of the slice
         * @return A new tuple containing elements from index BEGIN_IDX to BEGIN_IDX + N_ELEMS - 1
         */
        template<u_integer... IDX_S, u_integer BEGIN_IDX>
        auto _slice(std::integer_sequence<u_integer, IDX_S...> indexes,
                    std::integral_constant<u_integer, BEGIN_IDX> begin) const;

        /**
         * @brief Internal helper to concatenate two tuples
         * @tparam O_TYPES Element types of the other tuple
         * @tparam T_SIZE Index sequence for this tuple
         * @tparam O_SIZE Index sequence for the other tuple
         * @param other The other tuple to concatenate
         * @param ts Compile-time index sequence for current tuple
         * @param os Compile-time index sequence for other tuple
         * @return A new tuple containing elements of both tuples
         */
        template<typename... O_TYPES, u_integer... T_SIZE, u_integer... O_SIZE>
        tuple<TYPES..., O_TYPES...> _concat(const tuple<O_TYPES...>& other,
                                            std::integer_sequence<u_integer, T_SIZE...> ts,
                                            std::integer_sequence<u_integer, O_SIZE...> os) const;

    public:

        explicit tuple();

        /**
         * @brief Constructs a tuple with given elements
         * @param e Elements to initialize the tuple with
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
         * @brief Get the number of elements in the tuple
         * @return The size of the tuple (number of elements)
         */
        constexpr u_integer size();

        /**
         * @brief Access element by index (const lvalue-reference version)
         * @tparam IDX Zero-based index of the element
         * @return Const lvalue-reference to the element at the specified index
         *
         * @warning The return type is a reference, which affects structured bindings
         *          if used directly in `tuple_element`. Use value type in `tuple_element` to avoid type deduction issues.
         */
        template<u_integer IDX>
        const auto& get() const;

        /**
         * @brief Access element by index (non const lvalue-reference version)
         * @tparam IDX Zero-based index of the element
         * @return Lvalue-reference to the element at the specified index
         *
         * @warning The return type is a reference, which affects structured bindings
         *          if used directly in `tuple_element`. Use value type in `tuple_element` to avoid type deduction issues.
         */
        template<u_integer IDX>
        auto& get();

        /**
         * @brief Set (modify) the value of the element at the specified index
         * @tparam IDX Index of the element to modify (zero-based)
         * @tparam E Type of the new value (auto-deduced)
         * @param e Value to assign to the element at position `IDX`
         * @return Reference to the current tuple instance (for chaining)
         *
         * @note This method uses a static check to ensure type safety:
         *       the value `e` must be convertible to the element type `T` at index `IDX`.
         * @note Invoking this method on a `const` tuple is not allowed.
         *
         */
        template<u_integer IDX, typename E>
        tuple& set(const E& e);

        /**
         * @brief Slice the tuple from a specific index
         * @tparam BEGIN_IDX The starting index for slicing
         * @tparam N_ELEMS The number of elements to slice
         * @return A new tuple with the sliced elements
         */
        template<u_integer BEGIN_IDX, u_integer N_ELEMS>
        auto slice() const;

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
        integer compareTo(const tuple& other) const override;

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

        ~tuple() override = default;

        /**
         * @brief Create a tuple from a couple
         * @tparam F_TYPE Type of the first element
         * @tparam S_TYPE Type of the second element
         * @param cp The couple to convert
         * @return A new tuple with the couple's elements
         */
        template<typename F_TYPE, typename S_TYPE>
        friend tuple<F_TYPE, S_TYPE> makeTuple(const couple<F_TYPE, S_TYPE>& cp);

        /**
         * @brief Concatenate two tuples (friend function)
         * @tparam L_TYPES Types of the left tuple's elements
         * @tparam R_TYPES Types of the right tuple's elements
         * @param lt The left tuple
         * @param rt The right tuple
         * @return A new tuple combining elements from both tuples
         */
        template<typename... L_TYPES, typename... R_TYPES>
        friend tuple<L_TYPES..., R_TYPES...> operator+(const tuple<L_TYPES...>& lt, const tuple<R_TYPES...>& rt);
    };

    template<typename F_TYPE, typename S_TYPE>
    tuple<F_TYPE, S_TYPE> makeTuple(const couple<F_TYPE, S_TYPE> &cp);

    template<typename... L_TYPES, typename... R_TYPES>
    tuple<L_TYPES..., R_TYPES...> operator+(const tuple<L_TYPES...>& lt, const tuple<R_TYPES...>& rt);
}

namespace std {

    /**
     * @brief Specialization of tuple_size for tuple to enable structured bindings
     * @tparam TYPES Variadic template parameter list of element types
     */
    template<typename... TYPES>
    struct tuple_size<original::tuple<TYPES...>> //NOLINT
            : std::integral_constant<std::size_t, sizeof...(TYPES)> {};

    /**
     * @brief Specialization of tuple_element for tuple to enable structured bindings
     * @tparam I Index of element
     * @tparam TYPES Variadic template parameter list of element types
     *
     * @details This specialization defines the type of the I-th element
     *          in the custom tuple. It relies on std::tuple_element to deduce
     *          the correct base type (not reference type), which is crucial
     *          for structured binding compatibility with const-qualified tuples.
     *
     */
    template<std::size_t I, typename... TYPES>
    struct tuple_element<I, original::tuple<TYPES...>> { //NOLINT
        using type = typename std::tuple_element<I, std::tuple<TYPES...>>::type;
    };

    /**
     * @brief Structured binding support - get for non-const lvalue reference
     * @tparam I Index of element
     * @tparam TYPES Variadic template parameter list of element types
     * @param t tuple to get element from
     * @return Reference to the requested element
     * @note noexcept qualified
     */
    template<std::size_t I, typename... TYPES>
    constexpr auto& get(original::tuple<TYPES...>& t) noexcept; //NOLINT

    /**
     * @brief Structured binding support - get for const lvalue reference
     * @tparam I Index of element
     * @tparam TYPES Variadic template parameter list of element types
     * @param t tuple to get element from
     * @return Const reference to the requested element
     * @note noexcept qualified
     */
    template<std::size_t I, typename... TYPES>
    constexpr const auto& get(const original::tuple<TYPES...>& t) noexcept; //NOLINT

    /**
     * @brief Structured binding support - get for rvalue reference
     * @tparam I Index of element
     * @tparam TYPES Variadic template parameter list of element types
     * @param t tuple to get element from
     * @return Rvalue reference to the requested element
     * @note noexcept qualified
     */
    template<std::size_t I, typename... TYPES>
    constexpr auto&& get(original::tuple<TYPES...>&& t) noexcept; //NOLINT
}


template<typename... TYPES>
template<original::u_integer I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(const T& cur)
    : cur_elem(cur) {}

template<typename... TYPES>
template<original::u_integer I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(tupleImpl<I, T>&& other) noexcept
    : cur_elem(std::move(other.cur_elem)) {}

template<typename... TYPES>
template<original::u_integer I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(const tupleImpl<I, T>& other)
    : cur_elem(other.cur_elem) {}

template<typename... TYPES>
template<original::u_integer I, typename T>
auto original::tuple<TYPES...>::tupleImpl<I, T>::operator=(tupleImpl<I, T>&& other) noexcept -> tupleImpl<I, T>&
{
    if (this == &other)
        return *this;
    cur_elem = std::move(other.cur_elem);
    return *this;
}

template<typename... TYPES>
template<original::u_integer I, typename T>
template<original::u_integer I_DIFF>
const auto& original::tuple<TYPES...>::tupleImpl<I, T>::get() const {
    staticError<outOfBoundError, (I_DIFF > 0)>{};

    return cur_elem;
}

template<typename... TYPES>
template<original::u_integer I, typename T>
template<original::u_integer I_DIFF>
auto& original::tuple<TYPES...>::tupleImpl<I, T>::get() {
    staticError<outOfBoundError, (I_DIFF > 0)>{};

    return cur_elem;
}

template<typename... TYPES>
template<original::u_integer I, typename T>
template<original::u_integer I_DIFF, typename E>
void original::tuple<TYPES...>::tupleImpl<I, T>::set(const E& e) {
    staticError<outOfBoundError, (I_DIFF > 0)>{};
    staticError<valueError, !std::is_convertible_v<E, T>>{};

    cur_elem = static_cast<T>(e);
}

template<typename... TYPES>
template<original::u_integer I, typename T>
auto original::tuple<TYPES...>::tupleImpl<I, T>::compareTo(const tupleImpl<I, T>& other) const -> integer
{
    if constexpr (Comparable<T>){
        if (cur_elem != other.cur_elem)
            return cur_elem < other.cur_elem ? -1 : 1;
    }
    return 0;
}

template<typename... TYPES>
template<original::u_integer I, typename T>
std::string original::tuple<TYPES...>::tupleImpl<I, T>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << formatString(cur_elem);
    return ss.str();
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS>::tupleImpl(const T& cur, const TS& next_elems)
    : cur_elem(cur), next(next_elems) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS>::tupleImpl(tupleImpl<I, T, TS>&& other) noexcept
    : cur_elem(std::move(other.cur_elem)), next(std::move(other.next)) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS>::tupleImpl(const tupleImpl<I, T, TS>& other)
    : cur_elem(other.cur_elem), next(other.next) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
auto original::tuple<TYPES...>::tupleImpl<I, T, TS>::operator=(
    tupleImpl<I, T, TS>&& other) noexcept -> tupleImpl<I, T, TS>&
{
    if (this == &other)
        return *this;
    cur_elem = std::move(other.cur_elem);
    next = std::move(other.next);
    return *this;
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
template<original::u_integer I_DIFF>
const auto& original::tuple<TYPES...>::tupleImpl<I, T, TS>::get() const {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    } else{
        return next.template get<I_DIFF - 1>();
    }
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
template<original::u_integer I_DIFF>
auto& original::tuple<TYPES...>::tupleImpl<I, T, TS>::get() {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    } else{
        return next.template get<I_DIFF - 1>();
    }
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
template<original::u_integer I_DIFF, typename E>
void original::tuple<TYPES...>::tupleImpl<I, T, TS>::set(const E& e) {
    if constexpr (I_DIFF == 0){
        staticError<valueError, !std::is_convertible_v<E, T>>{};

        cur_elem = static_cast<T>(e);
    } else{
        next.template set<I_DIFF - 1, E>(e);
    }
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
auto original::tuple<TYPES...>::tupleImpl<I, T, TS>::compareTo(
    const tupleImpl<I, T, TS>& other) const -> integer
{
    if constexpr (Comparable<T>){
        if (cur_elem != other.cur_elem)
            return cur_elem < other.cur_elem ? -1 : 1;
    }
    return next.compareTo(other.next);
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename TS>
std::string original::tuple<TYPES...>::tupleImpl<I, T, TS>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << formatString(cur_elem);
    ss << formatString(next);
    return ss.str();
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(const T& cur, const TS&... next_elems)
    : cur_elem(cur), next(next_elems...) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(const T& cur, const tupleImpl<I + 1, TS...>& nt)
    : cur_elem(cur), next(nt) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(tupleImpl<I, T, TS...> &&other) noexcept
    : cur_elem(std::move(other.cur_elem)), next(std::move(other.next)) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(const tupleImpl<I, T, TS...>& other)
    : cur_elem(other.cur_elem), next(other.next) {}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
auto original::tuple<TYPES...>::tupleImpl<I, T, TS...>::operator=(
    tupleImpl<I, T, TS...>&& other) noexcept -> tupleImpl<I, T, TS...>&
{
    if (this == &other)
        return *this;
    cur_elem = std::move(other.cur_elem);
    next = std::move(other.next);
    return *this;
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
template<original::u_integer I_DIFF>
const auto& original::tuple<TYPES...>::tupleImpl<I, T, TS...>::get() const {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    } else{
        return next.template get<I_DIFF - 1>();
    }
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
template<original::u_integer I_DIFF>
auto& original::tuple<TYPES...>::tupleImpl<I, T, TS...>::get() {
    if constexpr (I_DIFF == 0){
        return cur_elem;
    } else{
        return next.template get<I_DIFF - 1>();
    }
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
template<original::u_integer I_DIFF, typename E>
void original::tuple<TYPES...>::tupleImpl<I, T, TS...>::set(const E& e) {
    if constexpr (I_DIFF == 0){
        staticError<valueError, !std::is_convertible_v<E, T>>{};

        cur_elem = static_cast<T>(e);
    } else{
        next.template set<I_DIFF - 1, E>(e);
    }
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
original::integer
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::compareTo(const tupleImpl<I, T, TS...>& other) const {
    if constexpr (Comparable<T>){
        if (cur_elem != other.cur_elem)
            return cur_elem < other.cur_elem ? -1 : 1;
    }
    return next.compareTo(other.next);
}

template<typename... TYPES>
template<original::u_integer I, typename T, typename... TS>
std::string original::tuple<TYPES...>::tupleImpl<I, T, TS...>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << formatString(cur_elem);
    ss << formatString(next);
    return ss.str();
}

template<typename... TYPES>
template<original::u_integer... IDX_S, original::u_integer BEGIN_IDX>
auto original::tuple<TYPES...>::_slice(std::integer_sequence<u_integer, IDX_S...>,
                                       std::integral_constant<u_integer, BEGIN_IDX>) const {
    return tuple<decltype(this->get<BEGIN_IDX + IDX_S>())...>(this->get<BEGIN_IDX + IDX_S>()...);
}

template<typename... TYPES>
template<typename... O_TYPES, original::u_integer... T_SIZE, original::u_integer... O_SIZE>
original::tuple<TYPES..., O_TYPES...>
original::tuple<TYPES...>::_concat(const tuple<O_TYPES...> &other,
                                   std::integer_sequence<u_integer, T_SIZE...>,
                                   std::integer_sequence<u_integer, O_SIZE...>) const {
    return tuple<TYPES..., O_TYPES...>{this->get<T_SIZE>()..., other.template get<O_SIZE>()...};
}

template<typename... TYPES>
original::tuple<TYPES...>::tuple() : elems() {}

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
constexpr original::u_integer original::tuple<TYPES...>::size()
{
    return SIZE;
}

template<typename... TYPES>
template<original::u_integer IDX>
const auto& original::tuple<TYPES...>::get() const {
    return elems.template get<IDX>();
}

template<typename... TYPES>
template<original::u_integer IDX>
auto& original::tuple<TYPES...>::get() {
    return elems.template get<IDX>();
}

template<typename... TYPES>
template<original::u_integer IDX, typename E>
original::tuple<TYPES...>& original::tuple<TYPES...>::set(const E &e) {
    elems.template set<IDX, E>(e);
    return *this;
}

template<typename... TYPES>
template<original::u_integer BEGIN_IDX, original::u_integer N_ELEMS>
auto original::tuple<TYPES...>::slice() const {
    constexpr bool out_of_bound = BEGIN_IDX >= SIZE || BEGIN_IDX + N_ELEMS > SIZE;
    staticError<outOfBoundError, out_of_bound>{};

    return this->_slice(
            std::make_integer_sequence<u_integer, N_ELEMS>{},
            std::integral_constant<u_integer, BEGIN_IDX>{}
    );
}

template<typename... TYPES>
original::integer original::tuple<TYPES...>::compareTo(const tuple& other) const {
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

template<typename F_TYPE, typename S_TYPE>
original::tuple<F_TYPE, S_TYPE> original::makeTuple(const couple<F_TYPE, S_TYPE> &cp) {
    return original::tuple<F_TYPE, S_TYPE>{cp.template get<0>(), cp.template get<1>()};
}

template<typename... L_TYPES, typename... R_TYPES>
original::tuple<L_TYPES..., R_TYPES...>
original::operator+(const tuple<L_TYPES...> &lt, const tuple<R_TYPES...> &rt) {
    return lt._concat(rt,
        std::make_integer_sequence<u_integer, sizeof...(L_TYPES)>{},
        std::make_integer_sequence<u_integer, sizeof...(R_TYPES)>{});
}

template<std::size_t I, typename... TYPES>
constexpr auto& std::get(original::tuple<TYPES...> &t) noexcept { //NOLINT
    return t.template get<I>();
}

template<std::size_t I, typename... TYPES>
constexpr const auto& std::get(const original::tuple<TYPES...> &t) noexcept { //NOLINT
    return t.template get<I>();
}

template<std::size_t I, typename... TYPES>
constexpr auto&& std::get(original::tuple<TYPES...> &&t) noexcept { //NOLINT
    return std::move(t.template get<I>());
}

#endif //TUPLE_H
