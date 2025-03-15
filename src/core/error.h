#ifndef ERROR_H
#define ERROR_H
#include <exception>
#include "types.h"

/**
 * @file error.h
 * @brief Custom exception classes and callback validation utilities.
 * @details This header defines a collection of exception types for error handling
 *          and a template-based callback signature checker for interface validation.
 */

namespace original {

    /**
     * @class callBackChecker
     * @brief Static utility for validating callback signatures.
     * @details Provides compile-time and runtime checks for callback function signatures,
     *          ensuring they match expected parameter types and return types.
     */
    class callBackChecker {
    public:
        /**
         * @brief Validates a callback's signature and return type.
         * @tparam Callback Type of the callback to check
         * @tparam Ret_TYPE Expected return type
         * @tparam Args_TYPE Expected argument types
         * @throw CallbackSignatureError If arguments don't match
         * @throw CallbackReturnTypeError If return type doesn't match
         *
         * @code{.cpp}
         * auto callback = [](int x) { return x * 1.5f; };
         * callBackChecker::check<decltype(callback), float, int>();  // Valid
         * callBackChecker::check<decltype(callback), int, int>();    // Throws callbackReturnTypeError
         * @endcode
         */
        template<typename Callback, typename Ret_TYPE, typename ...Args_TYPE>
        static void check();
    };

    /**
     * @class error
     * @brief Base interface for all exception types in Original
     * @details Inherits from std::exception to provide standard exception handling integration.
     * Serves as the root type for all domain-specific exceptions in Original.
     *
     * @note All exceptions in Original should inherit from this class to maintain consistent
     * error handling behavior across the codebase.
     */
    class error : public std::exception{};

    /**
     * @class staticError
     * @tparam ERR Exception type to associate with the static assertion (must inherit from error)
     * @tparam FALSE_CONDITION Boolean condition triggering the assertion when true
     * @brief Compile-time error assertion utility
     * @details Enforces compile-time constraints by triggering static assertions with
     * exception-specific error messages. The assertion fires when FALSE_CONDITION evaluates to true.
     *
     * @requires ExtendsOf<error, ERR> (ERR must inherit from error)
     *
     * @code{.cpp}
     * template<typename T>
     * void validate() {
     *     staticError<valueError, !std::is_arithmetic_v<T>>();
     * }
     * // Triggers "Wrong value given" static assertion if T isn't arithmetic
     * @endcode
     */
    template<typename ERR, const bool FALSE_CONDITION>
    requires ExtendsOf<error, ERR>
    class staticError
    {
        static_assert(!FALSE_CONDITION);
    };

// ----------------- Exception Classes -----------------

/**
 * @class outOfBoundError
 * @brief Exception for container index out-of-range errors.
 * @details Thrown when accessing elements beyond valid boundaries.
 */
class outOfBoundError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Out of the bound of the object.";
    }
};

/**
 * @class valueError
 * @brief Exception for invalid parameter values.
 * @details Thrown when receiving logically incorrect values (e.g., negative size).
 */
class valueError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Wrong value given.";
    }
};

/**
 * @class nullPointerError
 * @brief Exception for null pointer dereference attempts.
 * @details Thrown when accessing resources through null pointers.
 */
class nullPointerError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Attempting to access null pointer.";
    }
};

/**
 * @class unSupportedMethodError
 * @brief Exception for unimplemented method calls.
 * @details Thrown when calling methods not supported by the target class.
 */
class unSupportedMethodError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Unsupported Method for class.";
    }
};

/**
 * @class noElementError
 * @brief Exception for missing element requests.
 * @details Thrown when querying non-existent elements in containers.
 */
class noElementError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "No such element.";
    }
};

/**
 * @class callbackSignatureError
 * @brief Exception for callback argument mismatch.
 * @details Thrown when callback parameters don't match expected types.
 */
class callbackSignatureError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Callback signature mismatch.";
    }
};

/**
 * @class callbackReturnTypeError
 * @brief Exception for callback return type mismatch.
 * @details Thrown when callback return type differs from expected type.
 */
class callbackReturnTypeError final : public error {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Return type of callback mismatch.";
    }
};
} // namespace original

// ----------------- Definitions of error.h -----------------

template<typename Callback, typename Ret_TYPE, typename... Args_TYPE>
void original::callBackChecker::check() {
    if constexpr (constexpr bool is_valid = std::is_invocable_v<Callback, Args_TYPE...>;
        !is_valid){
        throw callbackSignatureError();
    } else{
        using result_type = std::invoke_result_t<Callback, Args_TYPE...>;
        if constexpr (constexpr bool is_valid_return_type = std::is_same_v<result_type, Ret_TYPE>;
            !is_valid_return_type)
            throw callbackReturnTypeError();
    }
}

template <const bool FALSE_CONDITION>
class original::staticError<original::error, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "A static assert triggered");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::outOfBoundError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "Out of the bound of the object");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::valueError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "Wrong value given");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::nullPointerError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "Attempting to access null pointer");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::unSupportedMethodError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "Unsupported Method for class");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::noElementError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "No such element");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::callbackSignatureError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "Return type of callback mismatch");
};

template <const bool FALSE_CONDITION>
class original::staticError<original::callbackReturnTypeError, FALSE_CONDITION>
{
    static_assert(!FALSE_CONDITION, "Callback signature mismatch");
};

#endif // ERROR_H
