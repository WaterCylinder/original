#ifndef DEQUE_H
#define DEQUE_H

#include "chain.h"
#include "containerAdapter.h"

namespace original{
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class deque final : public containerAdapter<TYPE, SERIAL>{
    public:
        explicit deque(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});
        deque(const std::initializer_list<TYPE>& lst);
        deque(const deque& other);
        deque& operator=(const deque& other);
        bool operator==(const deque& other) const;
        deque(deque&& other) noexcept;
        deque& operator=(deque&& other) noexcept;
        void pushBegin(const TYPE &e);
        void pushEnd(const TYPE &e);
        TYPE popBegin();
        TYPE popEnd();
        TYPE head() const;
        TYPE tail() const;
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    original::deque<TYPE, SERIAL>::deque(const SERIAL<TYPE>& serial)
        : containerAdapter<TYPE, SERIAL>(serial) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::deque<TYPE, SERIAL>::deque(const std::initializer_list<TYPE> &lst)
        : deque(SERIAL<TYPE>(lst)) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::deque<TYPE, SERIAL>::deque(const deque& other) : deque() {
        this->operator=(other);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::operator=(const deque& other) -> deque& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::operator==(const deque &other) const -> bool {
        return this->serial_ == other.serial_;
    }

    template <typename TYPE, template <typename> class SERIAL>
    original::deque<TYPE, SERIAL>::deque(deque&& other) noexcept : deque()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class SERIAL>
    auto original::deque<TYPE, SERIAL>::operator=(deque&& other) noexcept -> deque&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        other.serial_ = SERIAL<TYPE>{};
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::pushBegin(const TYPE &e) -> void {
        this->serial_.pushBegin(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::pushEnd(const TYPE &e) -> void {
        this->serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::popBegin() -> TYPE {
        return this->serial_.popBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::popEnd() -> TYPE {
        return this->serial_.popEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::head() const -> TYPE {
        return this->serial_.getBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::tail() const -> TYPE {
        return this->serial_.getEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::className() const -> std::string {
        return "deque";
    }

#endif //DEQUE_H
