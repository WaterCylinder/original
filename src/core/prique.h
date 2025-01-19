#ifndef PRIQUE_H
#define PRIQUE_H
#include "algorithms.h"
#include "blocksList.h"
#include "comparator.h"
#include "containerAdapter.h"

namespace original
{
    template<typename TYPE,
    template <typename> typename Callback = increaseComparator,
    template <typename> typename SERIAL = blocksList>
    class prique final : public containerAdapter<TYPE, SERIAL>
    {
        Callback<TYPE> compare_;
        public:
            explicit prique(const SERIAL<TYPE>& serial = SERIAL<TYPE>{}, const Callback<TYPE>& compare = Callback<TYPE>{});
            prique(const std::initializer_list<TYPE>& lst, const Callback<TYPE>& compare = Callback<TYPE>{});
            prique(const prique& other);
            prique& operator=(const prique& other);
            bool operator==(const prique& other) const;
            prique(prique&& other) noexcept;
            prique& operator=(prique&& other) noexcept;
            void push(const TYPE& e);
            TYPE pop();
            TYPE top() const;
            [[nodiscard]] std::string className() const override;
    };
}

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(const SERIAL<TYPE>& serial, const Callback<TYPE>& compare)
        : containerAdapter<TYPE, SERIAL>(serial), compare_(compare)
    {
        algorithms::heapInit(this->serial_.begin(), this->serial_.last(), this->compare_);
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(const std::initializer_list<TYPE>& lst, const Callback<TYPE>& compare)
        : prique(SERIAL<TYPE>(lst), compare) {}

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(const prique& other)
        : containerAdapter<TYPE, SERIAL>(other.serial_), compare_(other.compare_) {}

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::operator=(const prique& other) -> prique&
    {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        compare_ = other.compare_;
        return *this;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::operator==(const prique& other) const -> bool
    {
        return this->serial_ == other.serial_ && compare_ == other.compare_;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    original::prique<TYPE, Callback, SERIAL>::prique(prique&& other) noexcept : prique()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
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

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::push(const TYPE& e) -> void
    {
        this->serial_.pushEnd(e);
        algorithms::heapAdjustUp(this->serial_.begin(), this->serial_.last(), this->compare_);
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::pop() -> TYPE
    {
        if (this->empty()) throw noElementError();

        algorithms::swap(this->serial_.begin(), this->serial_.last());
        TYPE res = this->serial_.popEnd();
        algorithms::heapAdjustDown(this->serial_.begin(), this->serial_.last(), this->serial_.begin(), compare_);
        return res;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::top() const -> TYPE
    {
        return this->serial_.getBegin();
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    auto original::prique<TYPE, Callback, SERIAL>::className() const -> std::string
    {
        return "prique";
    }


#endif //PRIQUE_H
