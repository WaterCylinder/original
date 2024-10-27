#ifndef WRAPPER_H
#define WRAPPER_H

#include "string"
#include "config.h"

namespace original
{
    template <typename TYPE>
    class wrapper {
    protected:
        TYPE data;
    public:
        explicit wrapper(TYPE data);
        virtual TYPE getVal() const = 0;
        virtual wrapper* getPPrev() = 0;
        virtual wrapper* getPNext() = 0;
        virtual ~wrapper() = default;
        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) const;
    };

}
    template <typename TYPE>
    original::wrapper<TYPE>::wrapper(TYPE data) : data(data) {}

    template<typename TYPE>
    std::string original::wrapper<TYPE>::toString(bool enter) const {
        std::stringstream ss;
        ss << this->data;
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }
#endif //WRAPPER_H
