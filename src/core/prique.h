#ifndef PRIQUE_H
#define PRIQUE_H
#include "algorithms.h"
#include "comparator.h"
#include "vector.h"

namespace original
{
    template<typename TYPE,
    template <typename> typename Callback = increaseComparator,
    template <typename> typename SERIAL = vector>
    class prique : public printable
    {
        SERIAL<TYPE> serial_;
        Callback<TYPE> compare_;
        public:
            explicit prique(const SERIAL<TYPE>& serial = SERIAL<TYPE>{}, const Callback<TYPE>& compare = Callback<TYPE>{});
            prique(const std::initializer_list<TYPE>& lst, const Callback<TYPE>& compare = Callback<TYPE>{});
            prique(const prique& other);
            prique& operator=(const prique& other);
            bool operator==(const prique& other) const;
            prique(prique&& other) noexcept;
            prique& operator=(prique&& other) noexcept;
            [[nodiscard]] uint32_t size() const;
            [[nodiscard]] bool empty() const;
            void clear();
            void push(const TYPE& e);
            TYPE pop();
            TYPE top() const;
            [[nodiscard]] std::string className() const override;
            [[nodiscard]] std::string toString(bool enter) const override;
    };
}

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(const SERIAL<TYPE>& serial, const Callback<TYPE>& compare)
        : serial_(serial), compare_(compare)
    {
        algorithms::heapInit(this->serial_.first(), this->serial_.last(), this->compare_);
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(const std::initializer_list<TYPE>& lst, const Callback<TYPE>& compare)
        : prique(SERIAL<TYPE>(lst), compare) {}

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(const prique& other)
        : serial_(other.serial_), compare_(other.compare_) {}

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::operator=(const prique& other) -> prique&
    {
        if (this == &other) return *this;
        serial_ = other.serial_;
        compare_ = other.compare_;
        return *this;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::operator==(const prique& other) const -> bool
    {
        return serial_ == other.serial_ && compare_ == other.compare_;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(prique&& other) noexcept : prique()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::operator=(prique&& other) noexcept -> prique&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        this->compare_ = std::move(other.compare_);
        other.serial_ = {};
        other.compare_ = {};
        return *this;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::size() const -> uint32_t
    {
        return serial_.size();
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::empty() const -> bool
    {
        return serial_.empty();
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::clear() -> void
    {
        serial_.clear();
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::push(const TYPE& e) -> void
    {
        serial_.pushEnd(e);
        algorithms::heapAdjustUp(this->serial_.first(), this->serial_.last(), this->compare_);
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::pop() -> TYPE
    {
        if (this->empty()) throw noElementError();

        algorithms::swap(this->serial_.first(), this->serial_.last());
        TYPE res = serial_.popEnd();
        algorithms::heapAdjustDown(this->serial_.first(), this->serial_.last(), this->serial_.first(), compare_);
        return res;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::top() const -> TYPE
    {
        return serial_.getBegin();
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::className() const -> std::string
    {
        return "prique";
    }

    template <typename TYPE, template <typename> class Callback, template <typename> class SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::toString(const bool enter) const -> std::string
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


#endif //PRIQUE_H
