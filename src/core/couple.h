#ifndef COUPLE_H
#define COUPLE_H

/**
 * @file couple.h
 * @brief Generic pair container implementation
 * @details Provides a template class for storing two elements of different types
 * with printing capabilities. Supports equality comparison, formatted output,
 * move semantics, and structured binding support.
 */

#include "printable.h"
#include "comparable.h"
#include "types.h"
#include "error.h"


namespace original
{
    /**
     * @class couple
     * @tparam F_TYPE Type of the first element
     * @tparam S_TYPE Type of the second element
     * @brief Container for two heterogeneous elements
     * @extends printable
     * @extends comparable
     * @details Stores a pair of elements with type safety. Provides:
     * - Element access and copy operations
     * - Move construction and assignment
     * - Lexicographical comparison (first element precedence)
     * - Formatted string output through printable interface
     * - Comparable interface implementation
     * - Structured binding support via std::tuple_size and std::tuple_element specializations
     */
    template<typename F_TYPE, typename S_TYPE>
    class couple final : public printable, public comparable<couple<F_TYPE, S_TYPE>>
    {
        F_TYPE first_;  ///< Storage for the first element
        S_TYPE second_; ///< Storage for the second element

    public:
        /**
         * @brief Default constructs both elements
         */
        explicit couple();

        /**
         * @brief Constructs from pointer elements
         * @param first Pointer to first element
         * @param second Pointer to second element
         */
        couple(F_TYPE* first, S_TYPE* second);

        /**
         * @brief Constructs from element copies
         * @param first First element to copy
         * @param second Second element to copy
         */
        couple(const F_TYPE& first, const S_TYPE& second);

        /**
         * @brief Copy constructor
         * @param other couple to copy from
         */
        couple(const couple& other);

        /**
         * @brief Copy assignment operator
         * @param other couple to copy from
         * @return Reference to this object
         */
        couple& operator=(const couple& other);

        /**
         * @brief Move constructor
         * @param other couple to move from
         * @note noexcept qualified for compatibility with STL containers
         */
        couple(couple&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other couple to move from
         * @return Reference to this object
         * @note noexcept qualified for compatibility with STL containers
         */
        couple& operator=(couple&& other) noexcept;

        /**
         * @brief Element access template method (non-const version)
         * @tparam IDX element index (0=1st element, 1=2nd element)
         * @return Reference to the corresponding element
         * @throws outOfBoundError if IDX > 1
         * @note For non-const objects, returns a mutable reference
         */
        template<u_integer IDX>
        auto& get();

        /**
         * @brief Element access template method (const version)
         * @tparam IDX element index (0=1st element, 1=2nd element)
         * @return Const reference to the corresponding element
         * @throws outOfBoundError if IDX > 1
         * @note For const objects, returns a read-only reference
         */
        template<u_integer IDX>
        const auto& get() const;

        /**
         * @brief element modifies the template method
         * @tparam IDX element index (0=1st element, 1=2nd element)
         * @tparam T Element type (auto-derivation)
         * @param e new element value
         */
        template<u_integer IDX, typename T>
        void set(const T& e);

        /**
         * @brief Lexicographical comparison operation
         * @param other couple to compare with
         * @return integer Negative if less, positive if greater, zero if equal
         * @details Compares first elements first, then second elements if firsts are equal
         */
        integer compareTo(const couple &other) const override;

        /**
         * @brief Access first element
         * @return Mutable reference to first element
         */
        F_TYPE& first();

        /**
         * @brief Access second element
         * @return Mutable reference to second element
         */
        S_TYPE& second();

        /**
         * @brief Access first element (const version)
         * @return Mutable reference to first element
         */
        const F_TYPE& first() const;

        /**
         * @brief Access second element (const version)
         * @return Mutable reference to second element
         */
        const S_TYPE& second() const;

        /**
         * @brief Default destructor
         */
        ~couple() override;

        /**
         * @brief Gets class name identifier
         * @return "couple" string constant
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Formats pair elements as string
         * @param enter Add newline at end if true
         * @return Formatted string "(first: value, second: value)"
         */
        [[nodiscard]] std::string toString(bool enter) const override;
    };
}


namespace std {
    /**
     * @brief Specialization of tuple_size for couple to enable structured bindings
     * @tparam F Type of first element
     * @tparam S Type of second element
     */
    template<typename F, typename S>
    struct tuple_size<original::couple<F, S>> : std::integral_constant<std::size_t, 2> {}; // NOLINT

    /**
     * @brief Specialization of tuple_element for couple to enable structured bindings
     * @tparam I Index of element (0 or 1)
     * @tparam F Type of first element
     * @tparam S Type of second element
     */
    template<std::size_t I, typename F, typename S>
    struct tuple_element<I, original::couple<F, S>> { // NOLINT
        using type = std::conditional_t<I == 0, F, S>;
    };

    /**
     * @brief Structured binding support - get for non-const lvalue reference
     * @tparam I Index of element (0 or 1)
     * @tparam F Type of first element
     * @tparam S Type of second element
     * @param c couple to get element from
     * @return Reference to the requested element
     * @note noexcept qualified
     */
    template<std::size_t I, typename F, typename S>
    constexpr auto& get(original::couple<F, S>& c) noexcept;  // NOLINT

    /**
     * @brief Structured binding support - get for const lvalue reference
     * @tparam I Index of element (0 or 1)
     * @tparam F Type of first element
     * @tparam S Type of second element
     * @param c couple to get element from
     * @return Const reference to the requested element
     * @note noexcept qualified
     */
    template<std::size_t I, typename F, typename S>
    constexpr const auto& get(const original::couple<F, S>& c) noexcept;  // NOLINT

    /**
     * @brief Structured binding support - get for rvalue reference
     * @tparam I Index of element (0 or 1)
     * @tparam F Type of first element
     * @tparam S Type of second element
     * @param c couple to get element from
     * @return Rvalue reference to the requested element
     * @note noexcept qualified
     */
    template<std::size_t I, typename F, typename S>
    constexpr auto&& get(original::couple<F, S>&& c) noexcept;  // NOLINT
}


    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple() : first_(), second_() {}

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(F_TYPE* first, S_TYPE* second)
        : first_(*first), second_(*second) {}

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(const F_TYPE& first, const S_TYPE& second)
        : first_(first), second_(second) {}

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(const couple& other)
        : first_(other.first_), second_(other.second_) {}

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::operator=(const couple& other) -> couple&
    {
        if (this == &other) return *this;
        first_ = other.first_;
        second_ = other.second_;
        return *this;
    }

    template<typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(couple &&other) noexcept
        : first_(std::move(other.first_)), second_(std::move(other.second_)) {}

    template<typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>&
    original::couple<F_TYPE, S_TYPE>::operator=(couple &&other) noexcept {
        if (this == &other) return *this;

        first_ = std::move(other.first_);
        second_ = std::move(other.second_);
        return *this;
    }

    template<typename F_TYPE, typename S_TYPE>
    template<original::u_integer IDX>
    auto& original::couple<F_TYPE, S_TYPE>::get() {
        staticError<outOfBoundError, (IDX > 1)>{};
        if constexpr (IDX == 0){
            return this->first_;
        }else {
            return this->second_;
        }
    }

    template<typename F_TYPE, typename S_TYPE>
    template<original::u_integer IDX>
    const auto& original::couple<F_TYPE, S_TYPE>::get() const {
        staticError<outOfBoundError, (IDX > 1)>{};
        if constexpr (IDX == 0){
            return this->first_;
        }else {
            return this->second_;
        }
    }

    template<typename F_TYPE, typename S_TYPE>
    template<original::u_integer IDX, typename T>
    void original::couple<F_TYPE, S_TYPE>::set(const T &e) {
        staticError<outOfBoundError, (IDX > 1)>{};
        if constexpr (IDX == 0){
            staticError<valueError, !std::is_convertible_v<T, F_TYPE>>{};
            this->first_ = static_cast<F_TYPE>(e);
        } else{
            staticError<valueError, !std::is_convertible_v<T, S_TYPE>>{};
            this->second_ = static_cast<S_TYPE>(e);
        }
    }

    template<typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::compareTo(const couple& other) const -> integer
    {
        if constexpr (Comparable<F_TYPE>){
            if (this->first_ < other.first_)
                return -1;
            if (this->first_ > other.first_)
                return 1;
        }
        if constexpr (Comparable<S_TYPE>){
            if (this->second_ < other.second_)
                return -1;
            if (this->second_ > other.second_)
                return 1;
        }
        return 0;
    }

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::first() -> F_TYPE&
    {
        return this->first_;
    }

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::second() -> S_TYPE&
    {
        return this->second_;
    }

    template<typename F_TYPE, typename S_TYPE>
    const F_TYPE & original::couple<F_TYPE, S_TYPE>::first() const {
        return this->first_;
    }

    template<typename F_TYPE, typename S_TYPE>
    const S_TYPE & original::couple<F_TYPE, S_TYPE>::second() const {
        return this->second_;
    }

template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::~couple() = default;

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::className() const -> std::string
    {
        return "couple";
    }

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << this->className() << "(" << formatString(this->first_)
           << ", " << formatString(this->second_) << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

    template<std::size_t I, typename F, typename S>
    constexpr auto& std::get(original::couple<F, S> &c) noexcept {  // NOLINT
        return c.template get<I>();
    }

    template<std::size_t I, typename F, typename S>
    constexpr const auto& std::get(const original::couple<F, S> &c) noexcept {  // NOLINT
        return c.template get<I>();
    }

    template<std::size_t I, typename F, typename S>
    constexpr auto&& std::get(original::couple<F, S> &&c) noexcept {  // NOLINT
        return std::move(c.template get<I>());
    }

#endif //COUPLE_H
