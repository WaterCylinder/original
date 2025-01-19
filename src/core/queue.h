#ifndef QUEUE_H
#define QUEUE_H

#include "chain.h"
#include <string>

namespace original {
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class queue final : public containerAdapter<TYPE, SERIAL> {
    public:
        explicit queue(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});
        queue(const std::initializer_list<TYPE>& lst);
        queue(const queue& other);
        queue& operator=(const queue& other);
        queue(queue&& other) noexcept;
        queue& operator=(queue&& other) noexcept;
        bool operator==(const queue& other) const;
        void push(const TYPE& e);
        TYPE pop();
        TYPE head() const;
        TYPE tail() const;
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const SERIAL<TYPE>& serial)
        : containerAdapter<TYPE, SERIAL>(serial) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const std::initializer_list<TYPE> &lst)
        : queue(SERIAL<TYPE>(lst)) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const queue& other)
        : containerAdapter<TYPE, SERIAL>(other.serial_) {}

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::operator=(const queue& other) -> queue& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::operator==(const queue& other) const -> bool {
        return this->serial_ == other.serial_;
    }

    template <typename TYPE, template <typename> class SERIAL>
    original::queue<TYPE, SERIAL>::queue(queue&& other) noexcept : queue()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class SERIAL>
    auto original::queue<TYPE, SERIAL>::operator=(queue&& other) noexcept -> queue&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        other.serial_ = SERIAL<TYPE>{};
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::push(const TYPE& e) -> void {
        this->serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::pop() -> TYPE {
        return this->serial_.popBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::head() const -> TYPE {
        return this->serial_.getBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::tail() const -> TYPE {
        return this->serial_.getEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    std::string original::queue<TYPE, SERIAL>::className() const {
        return "queue";
    }

#endif //QUEUE_H
