#ifndef PRINTABLE_H
#define PRINTABLE_H
#pragma once
#include "config.h"

namespace original{
    class printable{
    public:
        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) = 0;
        _GLIBCXX_NODISCARD const char* toCString(bool enter);
    };
}

    _GLIBCXX_NODISCARD const char* original::printable::toCString(bool enter){
        return this->toString(enter).c_str();
    }

#endif // PRINTABLE_H
