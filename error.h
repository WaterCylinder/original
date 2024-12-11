#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <cxxabi.h>
#include "config.h"
#include "type_traits"

namespace original{

    template<typename Callback, typename Ret_TYPE, typename ...Args_TYPE>
    class callBackChecker{
            template<class TYPE>
            static std::string getTypeName();
        public:
            static void check();
    };

    class outOfBoundError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Out of the bound of thw object.";
        }
    };

    class valueError final : public  std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Wrong value given.";
        }
    };

    class nullPointerError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Attempting to access null pointer.";
        }
    };

    class unSupportedMethodError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Unsupported Method for class.";
        }
    };

    class noElementError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "No such element.";
        }
    };
}

    template<typename CallBack, typename Ret_TYPE, typename... Args_TYPE>
    template<class TYPE>
    std::string original::callBackChecker<CallBack, Ret_TYPE, Args_TYPE...>::getTypeName() {
        int status;
        char* demangled = abi::__cxa_demangle(typeid(TYPE).name(),
                                              nullptr, nullptr, &status);
        std::string name = (status == 0) ? demangled : typeid(TYPE).name();
        std::free(demangled);
        return name;
    }

    template<typename Callback, typename Ret_TYPE, typename... Args_TYPE>
    void original::callBackChecker<Callback, Ret_TYPE, Args_TYPE...>::check() {
        constexpr bool is_valid = std::is_invocable_r<Ret_TYPE, Callback, Args_TYPE...>::value;
        if constexpr (is_valid){
            return;
        }
        std::string message =
                "Callback must be a callable with return type '"
                + callBackChecker::getTypeName<Ret_TYPE>() +
                "' and argument type(s) (";
        ((message += callBackChecker::getTypeName<Args_TYPE>() + ", "), ...);
        message.pop_back();
        message.pop_back();
        message += ").";

        throw std::runtime_error(message);
    }

#endif //ERROR_H
