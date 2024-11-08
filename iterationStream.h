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
    std::ostream& operator<<(std::ostream& os, const original::iterationStream<TYPE>& i){
        os << i.toString(false);
        return os;
    }
}

    template<typename TYPE>
    std::string original::iterationStream<TYPE>::elementsString() const {
        std::stringstream ss;
        ss << "(";
        bool first = true;
        for (const original::iterator<TYPE> it = this->begin(); !it.isNull(); it.next()) {
            if (!first) {
                ss << ", ";
            }
            ss << it.get();
            first = false;
        }
        ss << ")";
        return ss.str();
    }

    template<typename TYPE>
    std::string original::iterationStream<TYPE>::toString(bool enter) const{
        std::stringstream ss;
        ss << "iterationStream" << this->elementsString();
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //ITERATIONSTREAM_H
