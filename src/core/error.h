#ifndef ERROR_H
#define ERROR_H
#include <exception>

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
         * callBackChecker::check<decltype(callback), int, int>();    // Throws CallbackReturnTypeError
         * @endcode
         */
        template<typename Callback, typename Ret_TYPE, typename ...Args_TYPE>
        static void check();
    };

// ----------------- Exception Classes -----------------

/**
 * @class outOfBoundError
 * @brief Exception for container index out-of-range errors.
 * @details Thrown when accessing elements beyond valid boundaries.
 */
class outOfBoundError final : public std::exception {
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
class valueError final : public std::exception {
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
class nullPointerError final : public std::exception {
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
class unSupportedMethodError final : public std::exception {
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
class noElementError final : public std::exception {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "No such element.";
    }
};

/**
 * @class CallbackSignatureError
 * @brief Exception for callback argument mismatch.
 * @details Thrown when callback parameters don't match expected types.
 */
class CallbackSignatureError final : public std::exception {
public:
    [[nodiscard]] auto what() const noexcept -> const char* override
    {
        return "Callback signature mismatch.";
    }
};

/**
 * @class CallbackReturnTypeError
 * @brief Exception for callback return type mismatch.
 * @details Thrown when callback return type differs from expected type.
 */
class CallbackReturnTypeError final : public std::exception {
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
        throw CallbackSignatureError();
    } else{
        using result_type = std::invoke_result_t<Callback, Args_TYPE...>;
        if constexpr (constexpr bool is_valid_return_type = std::is_same_v<result_type, Ret_TYPE>;
            !is_valid_return_type)
            throw CallbackReturnTypeError();
    }
}

#endif // ERROR_H