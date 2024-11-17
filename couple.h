#ifndef COUPLE_H
#define COUPLE_H
#include <variant>

#include "printable.h"

namespace original
{
    template<typename F_TYPE, typename S_TYPE>
    class couple final : public printable
    {
        F_TYPE first_;
        S_TYPE second_;
    public:
        explicit couple();
        couple(F_TYPE* first, S_TYPE* second);
        couple(const F_TYPE& first, const S_TYPE& second);
        couple(const couple& other);
        couple& operator=(const couple& other);
        bool operator==(const couple& other) const;
        F_TYPE& first();
        S_TYPE& second();
        ~couple() override;
        _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
    };
}

template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple() : first_(), second_() {}

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(F_TYPE* first, S_TYPE* second)
        : first_(*first), second_(*second) {}

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(const F_TYPE& first, const S_TYPE& second)
        : first_(first), second_(second) {}

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::couple(const couple& other)
        : first_(other.first_), second_(other.second_) {}

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::operator=(const couple& other) -> couple&
    {
        if (this == &other) return *this;
        first_ = other.first_;
        second_ = other.second_;
        return *this;
    }

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::operator==(const couple& other) const -> bool
    {
        return first_ == other.first_ && second_ == other.second_;
    }

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::first() -> F_TYPE&
    {
        return this->first_;
    }

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::second() -> S_TYPE&
    {
        return this->second_;
    }

    template <typename F_TYPE, typename S_TYPE>
    original::couple<F_TYPE, S_TYPE>::~couple() = default;

    template <typename F_TYPE, typename S_TYPE>
    auto original::couple<F_TYPE, S_TYPE>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << "couple" << "(" << "first: " << formatString(this->first_)
           << ", " << "second: " << formatString(this->second_) << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //COUPLE_H
