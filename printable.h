#ifndef PRINTABLE_H
#define PRINTABLE_H
#pragma once
#include "config.h"

namespace original {
    class printable {
        std::string cachedString;
    public:
        virtual ~printable() = default;

        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) = 0;
        _GLIBCXX_NODISCARD const char* toCString(bool enter);
    };
}

    _GLIBCXX_NODISCARD inline auto original::printable::toCString(const bool enter) -> const char*
    {
        this->cachedString = this->toString(enter);
        return this->cachedString.c_str();
    }

#endif // PRINTABLE_H
