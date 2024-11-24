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
        explicit stack(const SERIAL& serial = SERIAL{});
        stack(const stack& other);
        stack& operator=(const stack& other);
        _GLIBCXX_NODISCARD size_t size() const;
        _GLIBCXX_NODISCARD bool empty() const;
        void clear();
        void push(TYPE e);
        TYPE pop();
        TYPE top() const;
        iterator<TYPE>* begins() const override;
        iterator<TYPE>* ends() const override;
        _GLIBCXX_NODISCARD std::string className() const override;
    };
} // namespace original

    template<typename TYPE, typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const SERIAL& serial) : serial_{serial} {}

    template<typename TYPE, typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const stack& other) : stack() {
        this->operator=(other);
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::operator=(const stack& other) -> stack& {
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
        return serial_.get(-1);
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::begins() const -> iterator<TYPE>* {
        return serial_.begins();
    }

    template<typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::ends() const -> iterator<TYPE>* {
        return serial_.ends();
    }

    template <typename TYPE, typename SERIAL>
    auto original::stack<TYPE, SERIAL>::className() const -> std::string
    {
        return "stack";
    }

#endif // STACK_H
