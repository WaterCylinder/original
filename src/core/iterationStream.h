#ifndef ITERATIONSTREAM_H
#define ITERATIONSTREAM_H

#include <sstream>

#include "comparable.h"
#include "printable.h"
#include "iterable.h"

namespace original{
    template<typename TYPE, typename DERIVED>
    class iterationStream : public printable, public iterable<TYPE>, public comparable<iterationStream<TYPE, DERIVED>>{
    protected:
        [[nodiscard]] std::string elementsString() const;
    public:
        int compareTo(const iterationStream& other) const override;
        [[nodiscard]] std::string className() const override;
        [[nodiscard]] std::string toString(bool enter) const override;
    };
}

    template<typename TYPE, typename DERIVED>
    auto original::iterationStream<TYPE, DERIVED>::elementsString() const -> std::string
    {
        std::stringstream ss;
        ss << "(";
        bool first = true;
        for (const auto it = this->begin(); it.isValid(); ++it) {
            if (!first) {
                ss << ", ";
            }
            ss << formatString(it.get());
            first = false;
        }
        ss << ")";
        return ss.str();
    }

    template<typename TYPE, typename DERIVED>
    auto original::iterationStream<TYPE, DERIVED>::compareTo(const iterationStream &other) const -> int {
        const auto this_it = this->begin();
        const auto other_it = other.begin();
        for (;this_it.isValid() && other_it.isValid(); ++this_it, ++other_it) {
            if (*this_it != *other_it)
                return *this_it < *other_it ? -1 : 1;
        }
        return this_it.isValid() - other_it.isValid();
    }

    template<typename TYPE, typename DERIVED>
    auto original::iterationStream<TYPE, DERIVED>::className() const -> std::string {
        return "iterationStream";
    }

    template<typename TYPE, typename DERIVED>
    auto original::iterationStream<TYPE, DERIVED>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << this->className() << this->elementsString();
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //ITERATIONSTREAM_H
