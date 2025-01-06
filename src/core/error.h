#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <typeinfo>
#include <string>
#include <sstream>
#include <functional>

namespace original {
    class callBackChecker{
    public:
        template<typename Callback, typename Ret_TYPE, typename ...Args_TYPE>
        static void check();
    };

    class outOfBoundError final : public std::exception{
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Out of the bound of the object.";
        }
    };

    class valueError final : public  std::exception{
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Wrong value given.";
        }
    };

    class nullPointerError final : public std::exception{
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Attempting to access null pointer.";
        }
    };

    class unSupportedMethodError final : public std::exception{
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Unsupported Method for class.";
        }
    };

    class noElementError final : public std::exception{
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "No such element.";
        }
    };

    class CallbackSignatureError : public std::exception {
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Callback signature mismatch.";
        }
    };

    class CallbackReturnTypeError : public std::exception {
    public:
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Return type of callback mismatch.";
        }
    };
}

    template<typename Callback, typename Ret_TYPE, typename... Args_TYPE>
    void original::callBackChecker::check() {
        constexpr bool is_valid = std::is_invocable_v<Callback, Args_TYPE...>;
        if constexpr (!is_valid){
            throw CallbackSignatureError();
        } else{
            using result_type = std::invoke_result_t<Callback, Args_TYPE...>;
            constexpr bool is_valid_return_type = std::is_same_v<result_type, Ret_TYPE>;
            if constexpr (!is_valid_return_type)
                throw CallbackReturnTypeError();
        }
    }

#endif // ERROR_H
