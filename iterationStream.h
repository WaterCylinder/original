#ifndef ITERATIONSTREAM_H
#define ITERATIONSTREAM_H

#include <sstream>
#include "printable.h"
#include "iterable.h"

namespace original{
    template<typename TYPE>
    class iterationStream : public printable, public iterable<TYPE>{
    protected:
        _GLIBCXX_NODISCARD std::string elementsString() const;
    public:
        _GLIBCXX_NODISCARD std::string className() const override;
        _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
    };
}

    template<typename TYPE>
    auto original::iterationStream<TYPE>::elementsString() const -> std::string
    {
        std::stringstream ss;
        ss << "(";
        bool first = true;
        for (auto it = this->begin(); it.isValid(); ++it) {
            if (!first) {
                ss << ", ";
            }
            ss << formatString(*it);
            first = false;
        }
        ss << ")";
        return ss.str();
    }

    template <typename TYPE>
    std::string original::iterationStream<TYPE>::className() const
    {
        return "iterationStream";
    }

    template<typename TYPE>
    auto original::iterationStream<TYPE>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << this->className() << this->elementsString();
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //ITERATIONSTREAM_H
