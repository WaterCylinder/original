#ifndef STACK_H
#define STACK_H

#include "chain.h"
#include <string>

namespace original {
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class stack final : public containerAdapter<TYPE, SERIAL> {
    public:
        explicit stack(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});
        stack(const std::initializer_list<TYPE>& lst);
        stack(const stack& other);
        stack& operator=(const stack& other);
        stack(stack&& other) noexcept;
        stack& operator=(stack&& other) noexcept;
        bool operator==(const stack& other) const;
        void push(const TYPE& e);
        TYPE pop();
        TYPE top() const;
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const SERIAL<TYPE>& serial)
        : containerAdapter<TYPE, SERIAL>(serial) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const std::initializer_list<TYPE> &lst)
        : stack(SERIAL<TYPE>(lst)) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::stack<TYPE, SERIAL>::stack(const stack& other)
        : containerAdapter<TYPE, SERIAL>(other.serial_) {}

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::stack<TYPE, SERIAL>::operator=(const stack& other) -> stack& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template <typename TYPE, template <typename> class SERIAL>
    original::stack<TYPE, SERIAL>::stack(stack&& other) noexcept : stack()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class SERIAL>
    auto original::stack<TYPE, SERIAL>::operator=(stack&& other) noexcept -> stack&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        other.serial_ = SERIAL<TYPE>{};
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::stack<TYPE, SERIAL>::operator==(const stack& other) const -> bool {
        return this->serial_ == other.serial_;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::stack<TYPE, SERIAL>::push(const TYPE& e) -> void {
        this->serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::stack<TYPE, SERIAL>::pop() -> TYPE {
        return this->serial_.popEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::stack<TYPE, SERIAL>::top() const -> TYPE {
        return this->serial_.getEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    std::string original::stack<TYPE, SERIAL>::className() const {
        return "stack";
    }

#endif // STACK_H
