#ifndef PRINTABLE_H
#define PRINTABLE_H
#pragma once
#include "config.h"

namespace original{
    class printable{
    public:
        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) = 0;
    };
}

#endif // PRINTABLE_H
