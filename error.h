#ifndef ERROR_H
#define ERROR_H
#include <exception>
#include "config.h"

namespace original{

    class indexError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Illegal index";
        }
    };

    class valueError final : public  std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Wrong value given";
        }
    };

    class nullPointerError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Attempting to access null pointer";
        }
    };

    class unSupportedMethodError final : public std::exception{
        _GLIBCXX_NODISCARD auto what() const _GLIBCXX_NOEXCEPT -> const char* override
        {
            return "Unsupported Method for class";
        }
    };
}

#endif //ERROR_H
