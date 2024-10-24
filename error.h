#ifndef ERROR_H
#define ERROR_H
#include <exception>

namespace original{

    class indexError final : public std::exception{
        _GLIBCXX_NODISCARD const char* what() const _GLIBCXX_NOEXCEPT override{
            return "Illegal index";
        }
    };

    class valueError final : public  std::exception{
        _GLIBCXX_NODISCARD const char* what() const _GLIBCXX_NOEXCEPT override{
            return "Wrong value given";
        }
    };

}

#endif //ERROR_H
