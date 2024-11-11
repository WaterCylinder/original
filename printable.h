#ifndef PRINTABLE_H
#define PRINTABLE_H
#pragma once
#include "config.h"
#include "sstream"

namespace original {
    class printable {
        mutable std::string cachedString;
    public:
        virtual ~printable() = 0;

        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) const;
        _GLIBCXX_NODISCARD const char* toCString(bool enter) const;

        static const char* boolean(bool b);

        friend std::ostream& operator<<(std::ostream& os, const printable& p);
    };

    std::ostream& operator<<(std::ostream& os, const printable& p);
}

    inline original::printable::~printable() = default;

    inline std::string original::printable::toString(bool enter) const
    {
        std::stringstream ss;
        ss << typeid(this).name() << "(#" << this << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

    _GLIBCXX_NODISCARD inline auto original::printable::toCString(const bool enter) const -> const char*
    {
        this->cachedString = this->toString(enter);
        return this->cachedString.c_str();
    }

    inline const char* original::printable::boolean(bool b) {
        return b != 0 ? "true" : "false";
    }

    inline std::ostream& original::operator<<(std::ostream& os, const printable& p){
        os << p.toString(false);
        return os;
    }

#endif // PRINTABLE_H