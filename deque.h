#ifndef DEQUE_H
#define DEQUE_H

#include "chain.h"
#include <string>

namespace original{
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class deque : public iterationStream<TYPE>{
        SERIAL<TYPE> serial_;
    public:
        explicit deque(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});
        deque(const std::initializer_list<TYPE>& lst);
        deque(const deque& other);
        deque& operator=(const deque& other);
        bool operator==(const deque& other) const;
        [[nodiscard]] uint32_t size() const;
        [[nodiscard]] bool empty() const;
        void clear();
        void pushBegin(const TYPE &e);
        void pushEnd(const TYPE &e);
        TYPE popBegin();
        TYPE popEnd();
        TYPE head() const;
        TYPE tail() const;
        baseIterator<TYPE>* begins() const override;
        baseIterator<TYPE>* ends() const override;
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    original::deque<TYPE, SERIAL>::deque(const SERIAL<TYPE>& serial) : serial_{serial} {}

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
        serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::operator==(const deque &other) const -> bool {
        return serial_ == other.serial_;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::size() const -> uint32_t {
        return serial_.size();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::empty() const -> bool {
        return serial_.empty();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::pushBegin(const TYPE &e) -> void {
        serial_.pushBegin(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::pushEnd(const TYPE &e) -> void {
        serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::popBegin() -> TYPE {
        return serial_.popBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::popEnd() -> TYPE {
        return serial_.popEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::head() const -> TYPE {
        return serial_.getBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::tail() const -> TYPE {
        return serial_.getEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::begins() const -> baseIterator<TYPE>* {
        return serial_.begins();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::deque<TYPE, SERIAL>::ends() const -> baseIterator<TYPE>* {
        return serial_.ends();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    std::string original::deque<TYPE, SERIAL>::className() const
    {
        return "deque";
    }

#endif //DEQUE_H
