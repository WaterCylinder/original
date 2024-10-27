#ifndef WRAPPER_H
#define WRAPPER_H

#include "string"

namespace original
{
    template <typename TYPE>
    class wrapper {
    public:
        virtual TYPE getVal() const = 0;
        virtual wrapper* getPPrev() = 0;
        virtual wrapper* getPNext() = 0;
        virtual ~wrapper() = default;
        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) const = 0;
    };

}

#endif //WRAPPER_H
