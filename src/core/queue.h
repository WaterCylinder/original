#ifndef QUEUE_H
#define QUEUE_H

#include "chain.h"
#include <string>

namespace original {
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class queue : public printable {
        SERIAL<TYPE> serial_;
    public:
        explicit queue(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});
        queue(const std::initializer_list<TYPE>& lst);
        queue(const queue& other);
        queue& operator=(const queue& other);
        bool operator==(const queue& other) const;
        [[nodiscard]] uint32_t size() const;
        [[nodiscard]] bool empty() const;
        void clear();
        void push(const TYPE& e);
        TYPE pop();
        TYPE head() const;
        TYPE tail() const;
        [[nodiscard]] std::string className() const override;
        [[nodiscard]] std::string toString(bool enter) const override;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const SERIAL<TYPE>& serial) : serial_{serial} {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const std::initializer_list<TYPE> &lst)
            : queue(SERIAL<TYPE>(lst)) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const queue& other) : serial_{other.serial_} {}

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::operator=(const queue& other) -> queue& {
        if (this == &other) return *this;
        serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::operator==(const queue& other) const -> bool {
        return serial_ == other.serial_;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::size() const -> uint32_t {
        return serial_.size();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::empty() const -> bool {
        return serial_.empty();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::push(const TYPE& e) -> void {
        serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::pop() -> TYPE {
        return serial_.popBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::head() const -> TYPE {
        return serial_.getBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::tail() const -> TYPE {
        return serial_.getEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    std::string original::queue<TYPE, SERIAL>::className() const {
        return "queue";
    }

    template <typename TYPE, template <typename> class SERIAL>
    auto original::queue<TYPE, SERIAL>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << this->className() << "(";
        bool first = true;
        for (const auto e : this->serial_)
        {
            if (!first) ss << ", ";
            ss << printable::formatString(e);
            first = false;
        }
        ss << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //QUEUE_H
