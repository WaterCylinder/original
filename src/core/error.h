#ifndef ERROR_H
#define ERROR_H
#include <exception>
#include "printable.h"
#include "types.h"

/**
 * @file error.h
 * @brief Custom exception classes and callback validation utilities.
 * @details This header defines domain-specific exception types for the Original project,
 *          along with compile-time and runtime validation utilities for callback signatures.
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
         * @tparam Callback Type of the callback to check.
         * @tparam Ret_TYPE Expected return type.
         * @tparam Args_TYPE Expected argument types.
         * @throw callbackSignatureError If arguments don't match.
         * @throw callbackReturnTypeError If return type doesn't match.
         *
         * @code
         * auto cb = [](int x) { return x + 1.0f; };
         * callBackChecker::check<decltype(cb), float, int>(); // OK
         * callBackChecker::check<decltype(cb), int, int>();   // Throws callbackReturnTypeError
         * @endcode
         */
        template<typename Callback, typename Ret_TYPE, typename ...Args_TYPE>
        static void check();
    };

    /**
     * @class error
     * @brief Base class for all exceptions in the Original project.
     * @details Inherits from std::exception and implements
     * the printable interface for custom formatting.
     *
     * @note All exceptions in Original should inherit from this class to
     * maintain consistent error handling behavior across the codebase.
     */
    class error
            : public std::exception,
              public printable {
    #define ORIGINAL_ERROR_MSG "An error thrown"
    protected:
        const std::string msg_;

        /**
         * @brief Provides default message when no custom message is supplied.
         */
        virtual std::string defaultMsg() const {
            return ORIGINAL_ERROR_MSG;
        }
    public:
        /**
         * @brief Constructs an exception with an optional message.
         * @param msg Optional custom error message.
         */
        explicit error(std::string msg) : msg_(std::move(msg)) {}

        /**
         * @brief Returns the class name as string.
         */
        std::string className() const override {
            return "error";
        }

        /**
         * @brief Returns the full error message.
         */
        const char* what() const noexcept override {
            std::stringstream ss;
            ss << "Original::" << this->className() << ": ";
            if (!this->msg_.empty()){
                ss << this->msg_ << ".";
            } else {
                ss << this->defaultMsg() << ".";
            }
            return ss.str().c_str();
        }
    };

    /**
     * @class staticError
     * @tparam ERR Exception type to associate with the static assertion (must inherit from error)
     * @tparam TRIGGERING_CONDITION Boolean condition triggering the assertion when true
     * @brief Compile-time error assertion utility
     * @details Enforces compile-time constraints by triggering static assertions with
     * exception-specific error messages. The assertion fires when TRIGGERING_CONDITION evaluates to true.
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
    template<typename ERR, const bool TRIGGERING_CONDITION>
    requires ExtendsOf<error, ERR>
    class staticError
    {
        static_assert(!TRIGGERING_CONDITION);
    };

// ----------------- Exception Classes -----------------

/**
 * @class outOfBoundError
 * @brief Exception for container index out-of-range errors.
 * @details Thrown when accessing elements beyond valid boundaries.
 */
class outOfBoundError final : public error {
public:
    #define ORIGINAL_OUT_OF_BOUND_ERROR_MSG "Wrong value given"

    explicit outOfBoundError(std::string msg = "")
        : error(msg) {}

    std::string className() const override{
        return "outOfBoundError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_OUT_OF_BOUND_ERROR_MSG;
    }
};

/**
 * @class valueError
 * @brief Exception for invalid parameter values.
 * @details Thrown when receiving logically incorrect values (e.g., negative size).
 */
class valueError final : public error {
public:
    #define ORIGINAL_VALUE_ERROR_ERROR_MSG "Wrong value given"

    explicit valueError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "valueError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_VALUE_ERROR_ERROR_MSG;
    }
};

/**
 * @class nullPointerError
 * @brief Exception for null pointer dereference attempts.
 * @details Thrown when accessing resources through null pointers.
 */
class nullPointerError final : public error {
public:
    #define ORIGINAL_NULL_POINTER_ERROR_MSG "Attempting to access null pointer"

    explicit nullPointerError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "nullPointerError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_NULL_POINTER_ERROR_MSG;
    }
};

/**
 * @class unSupportedMethodError
 * @brief Exception for unimplemented method calls.
 * @details Thrown when calling methods not supported by the target class.
 */
class unSupportedMethodError final : public error {
public:
    #define ORIGINAL_UNSUPPORTED_METHOD_ERROR_MSG "Unsupported Method for the call"

    explicit unSupportedMethodError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "unSupportedMethodError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_UNSUPPORTED_METHOD_ERROR_MSG;
    }
};

/**
 * @class noElementError
 * @brief Exception for missing element requests.
 * @details Thrown when querying non-existent elements in containers.
 */
class noElementError final : public error {
public:
    #define ORIGINAL_NO_ELEMENT_ERROR_MSG "No such element"

    explicit noElementError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "noElementError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_NO_ELEMENT_ERROR_MSG;
    }
};

/**
 * @class callbackSignatureError
 * @brief Exception for callback argument mismatch.
 * @details Thrown when callback parameters don't match expected types.
 */
class callbackSignatureError final : public error {
public:
    #define ORIGINAL_CALLBACK_SIGNATURE_ERROR_MSG "Callback signature mismatch"

    explicit callbackSignatureError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "callbackSignatureError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_CALLBACK_SIGNATURE_ERROR_MSG;
    }
};

/**
 * @class callbackReturnTypeError
 * @brief Exception for callback return type mismatch.
 * @details Thrown when callback return type differs from expected type.
 */
class callbackReturnTypeError final : public error {
public:
    #define ORIGINAL_CALLBACK_RETURN_TYPE_ERROR_MSG "Return type of callback mismatch"

    explicit callbackReturnTypeError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "callbackReturnTypeError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_CALLBACK_RETURN_TYPE_ERROR_MSG;
    }
};

/**
* @class allocateError
* @brief Exception for memory allocation failures.
* @details Thrown when memory allocation requests cannot be fulfilled by the system.
*
* This exception typically indicates:
* - Insufficient available memory
* - Memory fragmentation preventing large block allocation
* - System-imposed memory limits being reached
*
* @note Consider checking system memory status before large allocations
*       when this exception might be expected.
*/
class allocateError final : public error
{
public:
    #define ORIGINAL_ALLOCATE_ERROR_MSG "Can not allocate memory"

    explicit allocateError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "allocateError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_ALLOCATE_ERROR_MSG;
    }
};

/**
 * @class sysError
 * @brief Exception for generic system failure.
 */
class sysError final : public error
{
public:
    #define ORIGINAL_SYS_ERROR_MSG "A system error triggered"

    explicit sysError(std::string msg = "")
    : error(msg) {}

    std::string className() const override{
        return "sysError";
    }

    std::string defaultMsg() const override {
        return ORIGINAL_SYS_ERROR_MSG;
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

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::error, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "A static assert triggered");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::outOfBoundError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Out of the bound of the object");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::valueError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Wrong value given");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::nullPointerError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Attempting to access null pointer");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::unSupportedMethodError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Unsupported Method for class");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::noElementError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "No such element");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::callbackSignatureError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Return type of callback mismatch");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::callbackReturnTypeError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Callback signature mismatch");
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::allocateError, TRIGGERING_CONDITION>
{
    static_assert(!TRIGGERING_CONDITION, "Can not allocate memory");
};

#endif // ERROR_H
