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
 *
 * @section Exception_Hierarchy Exception Hierarchy
 *
 * The exception system follows this inheritance structure:
 * @code
 * std::exception
 * └── original::error (implements printable)
 *     ├── outOfBoundError
 *     ├── valueError
 *     ├── nullPointerError
 *     ├── unSupportedMethodError
 *     ├── noElementError
 *     ├── callbackSignatureError
 *     ├── callbackReturnTypeError
 *     ├── allocateError
 *     └── sysError
 * @endcode
 */

namespace original {

    /**
     * @class callBackChecker
     * @brief Static utility for validating callback signatures.
     * @details Provides compile-time and runtime checks for callback function signatures,
     *          ensuring they match expected parameter types and return types.
     *
     * @section Usage_Examples Usage Examples
     *
     * @code
     * // Valid callback check
     * auto valid_cb = [](int x, float y) -> double { return x + y; };
     * callBackChecker::check<decltype(valid_cb), double, int, float>(); // OK
     *
     * // Invalid callback check - throws callbackSignatureError
     * auto invalid_cb = [](int x) { return x; };
     * callBackChecker::check<decltype(invalid_cb), int, std::string>(); // Throws
     *
     * // Invalid return type - throws callbackReturnTypeError
     * auto wrong_return_cb = [](int x) -> float { return x; };
     * callBackChecker::check<decltype(wrong_return_cb), int, int>(); // Throws
     * @endcode
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
     *
     * @section Features Features
     * - Integrates with C++ standard exception handling
     * - Provides formatted string output through printable interface
     * - Supports custom error messages with default fallback
     * - Enables compile-time error checking through staticError
     */
    class error
            : public std::exception,
              public printable {
    #define ORIGINAL_ERROR_MSG "An error thrown"
    protected:
        mutable std::string msg_;

        /**
         * @brief Provides default message when no custom message is supplied.
         * @return Default error message string
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
         * @return Class name identifier
         */
        std::string className() const override {
            return "error";
        }

        /**
         * @brief Generates formatted error message
         * @return Complete error message with class name and description
         */
        std::string message() const noexcept {
            std::stringstream ss;
            ss << "Original::" << this->className() << ": ";
            if (!this->msg_.empty()){
                ss << this->msg_ << ".";
            } else {
                ss << this->defaultMsg() << ".";
            }
            return ss.str();
        }

        /**
         * @brief Returns the full error message.
         * @return C-style string of the error message
         */
        const char* what() const noexcept override {
            this->msg_ = this->message();
            return this->msg_.c_str();
        }
    };

    /**
     * @class staticError
     * @brief Compile-time error triggering utility.
     * @tparam ERR Exception class to represent the error.
     * @tparam TRIGGERING_CONDITION Whether to trigger the static assertion.
     * @requires ERR must inherit from error.
     *
     * @section Usage_Examples Usage Examples
     *
     * @code
     * // Compile-time type checking
     * staticError<valueError, !std::is_integral_v<integer>>::asserts();
     *
     * // Compile-time bounds checking
     * staticError<outOfBoundError, (SIZE > MAX_SIZE)>::asserts();
     *
     * // Compile-time null pointer checking
     * staticError<nullPointerError, (ptr == nullptr)>::asserts();
     * @endcode
     */
    template<typename ERR, const bool TRIGGERING_CONDITION>
    requires ExtendsOf<error, ERR>
    class staticError
    {
    public:
        /**
         * @brief Triggers static assertion if the condition is true.
         * @details When TRIGGERING_CONDITION is true, this method will cause
         * a compilation error with a descriptive message based on the ERR type.
         */
        static void asserts(){
            static_assert(!TRIGGERING_CONDITION);
        }
    };

// ----------------- Exception Classes -----------------

/**
 * @class outOfBoundError
 * @brief Exception for container index out-of-range errors.
 * @details Thrown when accessing elements beyond valid boundaries.
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Accessing array elements with negative indices
 * - Using indices >= container size
 * - Invalid iterator dereferencing
 * - Buffer overflow attempts
 */
class outOfBoundError final : public error {
public:
    #define ORIGINAL_OUT_OF_BOUND_ERROR_MSG "Wrong value given"

    explicit outOfBoundError(std::string msg = "")
        : error(std::move(msg)) {}

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
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Negative sizes for containers
 * - Invalid enum values
 * - Out-of-range numeric parameters
 * - Invalid configuration values
 */
class valueError final : public error {
public:
    #define ORIGINAL_VALUE_ERROR_ERROR_MSG "Wrong value given"

    explicit valueError(std::string msg = "")
    : error(std::move(msg)) {}

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
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Dereferencing null smart pointers
 * - Accessing uninitialized raw pointers
 * - Calling methods on null object references
 */
class nullPointerError final : public error {
public:
    #define ORIGINAL_NULL_POINTER_ERROR_MSG "Attempting to access null pointer"

    explicit nullPointerError(std::string msg = "")
    : error(std::move(msg)) {}

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
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Calling abstract base class methods
 * - Using platform-specific features on wrong platforms
 * - Accessing deprecated or removed functionality
 */
class unSupportedMethodError final : public error {
public:
    #define ORIGINAL_UNSUPPORTED_METHOD_ERROR_MSG "Unsupported Method for the call"

    explicit unSupportedMethodError(std::string msg = "")
    : error(std::move(msg)) {}

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
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Removing from empty containers
 * - Accessing non-existent map keys
 * - Querying empty optional values
 * - Popping from empty stacks/queues
 */
class noElementError final : public error {
public:
    #define ORIGINAL_NO_ELEMENT_ERROR_MSG "No such element"

    explicit noElementError(std::string msg = "")
    : error(std::move(msg)) {}

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
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Wrong number of callback parameters
 * - Incompatible parameter types
 * - Missing required parameters
 */
class callbackSignatureError final : public error {
public:
    #define ORIGINAL_CALLBACK_SIGNATURE_ERROR_MSG "Callback signature mismatch"

    explicit callbackSignatureError(std::string msg = "")
    : error(std::move(msg)) {}

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
 *
 * @section Typical_Scenarios Typical Scenarios
 * - Callback returns wrong type
 * - Return type not convertible to expected type
 * - Void return when value expected
 */
class callbackReturnTypeError final : public error {
public:
    #define ORIGINAL_CALLBACK_RETURN_TYPE_ERROR_MSG "Return type of callback mismatch"

    explicit callbackReturnTypeError(std::string msg = "")
    : error(std::move(msg)) {}

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
*
* @section Typical_Scenarios Typical Scenarios
* - Large container allocations
* - Memory pool exhaustion
* - System out-of-memory conditions
*/
class allocateError final : public error
{
public:
    #define ORIGINAL_ALLOCATE_ERROR_MSG "Can not allocate memory"

    explicit allocateError(std::string msg = "")
    : error(std::move(msg)) {}

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
 * @details Thrown for operating system or low-level failures.
 *
 * @section Typical_Scenarios Typical Scenarios
 * - File system errors
 * - Network operation failures
 * - Hardware access violations
 * - System call failures
 */
class sysError final : public error
{
public:
    #define ORIGINAL_SYS_ERROR_MSG "A system error triggered"

    explicit sysError(std::string msg = "")
    : error(std::move(msg)) {}

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
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::outOfBoundError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_OUT_OF_BOUND_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::valueError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_VALUE_ERROR_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::nullPointerError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_NULL_POINTER_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::unSupportedMethodError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_UNSUPPORTED_METHOD_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::noElementError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_NO_ELEMENT_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::callbackSignatureError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_CALLBACK_SIGNATURE_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::callbackReturnTypeError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_CALLBACK_RETURN_TYPE_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::allocateError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_ALLOCATE_ERROR_MSG);
    }
};

template <const bool TRIGGERING_CONDITION>
class original::staticError<original::sysError, TRIGGERING_CONDITION>
{
public:
    static void asserts(){
        static_assert(!TRIGGERING_CONDITION, ORIGINAL_SYS_ERROR_MSG);
    }
};

#endif // ERROR_H
