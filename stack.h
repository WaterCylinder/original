#ifndef STACK_H
#define STACK_H

#include "chain.h"
#include <sstream>
#include <string>

namespace original {
    template<typename TYPE, typename SERIAL = chain<TYPE>>
    class stack : public iterationStream<TYPE> {
        SERIAL serial_;
    public:
        explicit stack(const SERIAL& serial = chain<TYPE>{});
        explicit stack(const stack<TYPE>& other);
        stack& operator=(const stack<TYPE>& other);
        _GLIBCXX_NODISCARD size_t size() const;
        _GLIBCXX_NODISCARD bool empty() const;
        void clear();
        void push(TYPE e);
        TYPE pop();
        TYPE top() const;
        iterator<TYPE>* begins() const override;
        iterator<TYPE>* ends() const override;
        _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
    };
} // namespace original

    template<typename TYPE, typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const SERIAL& serial) : serial_{serial} {}

    template<typename TYPE, typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const stack<TYPE> &other) : stack() {
        this->operator=(other);
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::operator=(const stack<TYPE> &other) -> stack& {
        if (this == &other) return *this;
        serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::size() const -> size_t {
        return serial_.size();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::empty() const -> bool {
        return serial_.empty();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::push(TYPE e) -> void {
        serial_.pushEnd(e);
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::pop() -> TYPE {
        return serial_.popEnd();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::top() const -> TYPE {
        return serial_[-1];
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::begins() const -> iterator<TYPE>* {
        return serial_.begins();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::ends() const -> iterator<TYPE>* {
        return serial_.ends();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << "stack" << this->elementsString();;
        if (enter) ss << "\n";
        return ss.str();
    }

#endif // STACK_H
