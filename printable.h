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

        static const char* boolean(bool b);

        friend std::ostream& operator<<(std::ostream& os, const printable& p);
    };

    std::ostream& operator<<(std::ostream& os, const printable& p);
}

    _GLIBCXX_NODISCARD inline auto original::printable::toCString(const bool enter) -> const char*
    {
        this->cachedString = this->toString(enter);
        return this->cachedString.c_str();
    }

    const char* original::printable::boolean(bool b) {
        return b != 0 ? "true" : "false";
    }

    std::ostream& original::operator<<(std::ostream& os, const printable& p){
        os << "printable";
        os << "(" << "#" << &p << ")";
        return os;
    }


#endif // PRINTABLE_H
