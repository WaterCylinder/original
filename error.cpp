#include "error.h"


namespace original{

    _GLIBCXX_NODISCARD const char* indexError::what() const _GLIBCXX_NOEXCEPT override {
        return "Illegal index";
    }

    _GLIBCXX_NODISCARD const char* valueError::what() const _GLIBCXX_NOEXCEPT override{
        return "Wrong value given";
    }


}