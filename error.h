#ifndef ERROR_H
#define ERROR_H
#include <exception>

namespace original{

    class indexError final : public std::exception{
        [[nodiscard]] const char* what() const _GLIBCXX_NOEXCEPT override;
    };

    class valueError final : public  std::exception{
        [[nodiscard]] const char* what() const _GLIBCXX_NOEXCEPT override;
    };

}

#endif //ERROR_H
