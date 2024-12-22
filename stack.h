#ifndef STACK_H
#define STACK_H

#include "chain.h"
#include <string>

namespace original {
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class stack : public iterationStream<TYPE> {
        SERIAL<TYPE> serial_;
    public:
        explicit stack(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});
        stack(const stack& other);
        stack& operator=(const stack& other);
        bool operator==(const stack& other) const;
        [[nodiscard]] uint32_t size() const;
        [[nodiscard]] bool empty() const;
        void clear();
        void push(const TYPE& e);
        TYPE pop();
        TYPE top() const;
        iterator<TYPE>* begins() const override;
        iterator<TYPE>* ends() const override;
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE, template <typename> class SERIAL>
    original::stack<TYPE, SERIAL>::stack(const SERIAL<TYPE>& serial) : serial_{serial} {}

    template<typename TYPE, template <typename> class SERIAL>
    original::stack<TYPE, SERIAL>::stack(const stack& other) : serial_{other.serial_} {}

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::operator=(const stack& other) -> stack& {
        if (this == &other) return *this;
        serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::operator==(const stack& other) const -> bool {
        return serial_ == other.serial_;
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::size() const -> uint32_t {
        return serial_.size();
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::empty() const -> bool {
        return serial_.empty();
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::push(const TYPE& e) -> void {
        serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::pop() -> TYPE {
        return serial_.popEnd();
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::top() const -> TYPE {
        return serial_.getEnd();
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::begins() const -> iterator<TYPE>* {
        return serial_.begins();
    }

    template<typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::ends() const -> iterator<TYPE>* {
        return serial_.ends();
    }

    template<typename TYPE, template <typename> class SERIAL>
    std::string original::stack<TYPE, SERIAL>::className() const {
        return "stack";
    }

#endif // STACK_H
