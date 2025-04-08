#ifndef TYPES_H
#define TYPES_H

#include "type_traits"
#include <concepts>

/**
 * @file types.h
 * @brief Type system foundations and concept definitions.
 * @details This header defines core type traits and concepts used for template
 *          metaprogramming and interface constraints throughout the library.
 *          These concepts enforce compile-time interface requirements.
 */

namespace original {

    template<typename... ARGS>
    concept NotNull = sizeof...(ARGS) > 0;
    
/**
 * @concept Comparable
 * @brief Requires type to support all comparison operators.
 * @tparam TYPE The type to check.
 * @details Enforces that the type provides all standard comparison operators
 *          (==, !=, <, <=, >, >=) returning boolean results.
 *
 * @code{.cpp}
 * static_assert(Comparable<int>);  // Succeeds
 * @endcode
 */
template <typename TYPE>
concept Comparable = requires(const TYPE& t1, const TYPE& t2) {
    { t1 == t2 } -> std::same_as<bool>;
    { t1 != t2 } -> std::same_as<bool>;
    { t1 < t2 } -> std::same_as<bool>;
    { t1 <= t2 } -> std::same_as<bool>;
    { t1 > t2 } -> std::same_as<bool>;
    { t1 >= t2 } -> std::same_as<bool>;
};

/**
 * @concept CallbackOf
 * @brief Validates callback signature compatibility.
 * @tparam Callback Callable type to check
 * @tparam ReturnType Expected return type
 * @tparam Args Expected argument types
 * @details Ensures the callback can be invoked with specified arguments
 *          and returns the specified type.
 *
 * @code{.cpp}
 * auto lambda = [](int x) { return x * 2; };
 * static_assert(CallbackOf<decltype(lambda), double, int>);  // Fails (return type mismatch)
 * @endcode
 */
template <typename Callback, typename ReturnType, typename... Args>
concept CallbackOf = requires(Callback callback, Args&&... args){
    { callback(std::forward<Args>(args)...) } -> std::same_as<ReturnType>;
};

/**
 * @concept Compare
 * @brief Combines Comparable and CallbackOf for comparison callbacks.
 * @tparam Callback Comparison function type
 * @tparam TYPE Type being compared
 * @details Requires a callback that takes two TYPE references and returns bool,
 *          while TYPE itself must satisfy Comparable.
 *
 * @code{.cpp}
 * // Validates custom comparator
 * auto int_comparator = [](const int& a, const int& b) { return a < b; };
 * static_assert(Compare<decltype(int_comparator), int>);  // Passes
 *
 * // Invalid example (wrong return type)
 * auto bad_comp = [](const int& a, const int& b) { return a - b; };
 * static_assert(!Compare<decltype(bad_comp), int>);  // Fails: returns int instead of bool
 * @endcode
 */
template <typename Callback, typename TYPE>
concept Compare =
    Comparable<TYPE> && CallbackOf<Callback, bool, const TYPE&, const TYPE&>;

/**
 * @concept Condition
 * @brief Constraint for predicate callbacks.
 * @tparam Callback Predicate function type
 * @tparam TYPE Input type for predicate
 * @details Requires a callback that takes a TYPE reference and returns bool.
 *
 * @code{.cpp}
 * // Validates positive number checker
 * auto is_positive = [](const auto& x) { return x > 0; };
 * static_assert(Condition<decltype(is_positive), double>);  // Passes
 *
 * // Invalid example (parameter type mismatch)
 * auto string_check = [](std::string s) { return !s.empty(); };
 * static_assert(!Condition<decltype(string_check), const std::string&>);  // Fails: requires const reference
 * @endcode
 */
template <typename Callback, typename TYPE>
concept Condition = CallbackOf<Callback, bool, const TYPE&>;

/**
 * @concept Operation
 * @brief Constraint for mutating operations.
 * @tparam Callback Mutation function type
 * @tparam TYPE Type being modified
 * @details Requires a callback that takes a mutable TYPE reference and returns void.
 *
 * @code{.cpp}
 * // Validates increment operation
 * auto increment = [](auto& x) { x += 1; };
 * static_assert(Operation<decltype(increment), int>);  // Passes
 *
 * // Invalid example (wrong return type)
 * auto bad_op = [](int& x) { return x++; };
 * static_assert(!Operation<decltype(bad_op), int>);  // Fails: returns int instead of void
 * @endcode
 */
template <typename Callback, typename TYPE>
concept Operation = CallbackOf<Callback, void, TYPE&>;

/**
 * @concept SuperOf
 * @brief Checks inheritance or type equality.
 * @tparam Base Base class or type
 * @tparam Derive Derived class or same type
 * @details Combines std::is_base_of_v and std::is_same_v to validate
 *          "is-a" relationships.
 *
 * @code{.cpp}
 * struct Base {};
 * struct Derived : Base {};
 *
 * static_assert(SuperOf<Base, Derived>);    // Passes: inheritance
 * static_assert(SuperOf<int, int>);         // Passes: same type
 * static_assert(!SuperOf<Derived, Base>);   // Fails: reverse inheritance
 * @endcode
 */
template <typename Base, typename Derive>
concept SuperOf = std::is_base_of_v<Base, Derive> || std::is_same_v<Base, Derive>;

/**
 * @concept ExtendsOf
 * @brief Checks derivation or type identity.
 * @tparam Base Base class or same type
 * @tparam Derive Derived class or same type
 * @details Uses std::derived_from for explicit inheritance checking,
 *          allowing for clearer intent than SuperOf in some contexts.
 *
 * @code{.cpp}
 * struct Animal {};
 * struct Cat : Animal {};
 *
 * static_assert(ExtendsOf<Animal, Cat>);    // Passes: valid inheritance
 * static_assert(ExtendsOf<int, int>);       // Passes: same type
 * static_assert(ExtendsOf<Cat, int>);   // Fails: invalid inheritance
 * @endcode
 */
template <typename Base, typename Derive>
concept ExtendsOf = std::derived_from<Derive, Base> || std::is_same_v<Base, Derive>;

} // namespace original

#endif // TYPES_H