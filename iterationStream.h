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
        std::string toString(bool enter) const override;

        template<typename U>
        friend std::ostream& operator<<(std::ostream& os, const iterationStream<U>& i);
    };

    template<typename TYPE>
    auto operator<<(std::ostream& os, const iterationStream<TYPE>& i) -> std::ostream&
    {
        os << i.toString(false);
        return os;
    }
}

    template<typename TYPE>
    auto original::iterationStream<TYPE>::elementsString() const -> std::string
    {
        std::stringstream ss;
        ss << "(";
        bool first = true;
        for (const iterator<TYPE> it = this->begin(); !it.isNull(); it.next()) {
            if (!first) {
                ss << ", ";
            }
            ss << formatElement(it.get());
            first = false;
        }
        ss << ")";
        return ss.str();
    }

    template<typename TYPE>
    auto original::iterationStream<TYPE>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << "iterationStream" << this->elementsString();
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //ITERATIONSTREAM_H
