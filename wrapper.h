#ifndef WRAPPER_H
#define WRAPPER_H
#pragma once
#include "string"
#include "config.h"
#include "printable.h"

namespace original
{
    template <typename TYPE>
    class wrapper : public printable{
    public:
        virtual TYPE& getVal() = 0;
        virtual void setVal(TYPE data) = 0;
        virtual wrapper* getPPrev() = 0;
        virtual wrapper* getPNext() = 0;
        ~wrapper() override = default;
        _GLIBCXX_NODISCARD std::string toString(bool enter) override;
    };

}
    template<typename TYPE>
    std::string original::wrapper<TYPE>::toString(const bool enter){
        std::stringstream ss;
        ss << this->getVal();
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }
#endif //WRAPPER_H
