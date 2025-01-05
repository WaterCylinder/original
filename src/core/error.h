#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <typeinfo>
#include <string>

namespace original {

    template<typename Callback, typename Ret_TYPE, typename ...Args_TYPE>
    class callBackChecker{
        template<class TYPE>
        static std::string getTypeName();

    public:
        static void check();
    };

    class outOfBoundError final : public std::exception{
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Out of the bound of the object.";
        }
    };

    class valueError final : public  std::exception{
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Wrong value given.";
        }
    };

    class nullPointerError final : public std::exception{
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Attempting to access null pointer.";
        }
    };

    class unSupportedMethodError final : public std::exception{
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "Unsupported Method for class.";
        }
    };

    class noElementError final : public std::exception{
        [[nodiscard]] auto what() const noexcept -> const char* override
        {
            return "No such element.";
        }
    };
}

    template<typename Callback, typename Ret_TYPE, typename... Args_TYPE>
    template<class TYPE>
    std::string original::callBackChecker<Callback, Ret_TYPE, Args_TYPE...>::getTypeName() {
        std::string name = typeid(TYPE).name();
        return name;
    }

    template<typename Callback, typename Ret_TYPE, typename... Args_TYPE>
    void original::callBackChecker<Callback, Ret_TYPE, Args_TYPE...>::check() {
        constexpr bool is_valid = std::is_invocable_r_v<Ret_TYPE, Callback, Args_TYPE...>;
        if constexpr (is_valid) {
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

#endif // ERROR_H
