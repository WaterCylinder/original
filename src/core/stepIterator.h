#ifndef STEPITERATOR_H
#define STEPITERATOR_H

#include <limits>

#include "iterator.h"
#include "wrapper.h"
#include "error.h"

namespace original
{
    template<typename TYPE>
    class stepIterator : public baseIterator<TYPE>
    {
        protected:
            mutable wrapper<TYPE>* _ptr;

            explicit stepIterator(wrapper<TYPE>* ptr);
            static int64_t ptrDistance(wrapper<TYPE>* start, wrapper<TYPE>* end);
            bool equalPtr(const iterator<TYPE>* other) const override;
        public:
            stepIterator(const stepIterator& other);
            stepIterator& operator=(const stepIterator& other);
            stepIterator* clone() const override;
            [[nodiscard]] bool hasNext() const override;
            [[nodiscard]] bool hasPrev() const override;
            bool atPrev(const iterator<TYPE>* other) const override;
            bool atNext(const iterator<TYPE>* other) const override;
            void next() const override;
            void prev() const override;
            void operator+=(int64_t steps) const override;
            void operator-=(int64_t steps) const override;
            int64_t operator-(const iterator<TYPE>& other) const override;
            stepIterator* getNext() const override;
            iterator<TYPE>* getPrev() const override;
            TYPE& get() override;
            TYPE get() const override;
            void set(const TYPE& data) override;
            [[nodiscard]] bool isValid() const override;
            [[nodiscard]] std::string className() const override;
    };
}

    template <typename TYPE>
    original::stepIterator<TYPE>::stepIterator(wrapper<TYPE>* ptr)
        : _ptr(ptr) {}

    template <typename TYPE>
    auto original::stepIterator<TYPE>::ptrDistance(wrapper<TYPE>* start, wrapper<TYPE>* end) -> int64_t
    {
        int64_t dis = 0;
        auto* cur = start;
        while (cur != nullptr)
        {
            if (cur == end)
            {
                return dis;
            }
            cur = cur->getPNext();
            dis += 1;
        }
        return std::numeric_limits<int64_t>::max();
    }

    template<typename TYPE>
    auto original::stepIterator<TYPE>::equalPtr(const iterator<TYPE> *other) const -> bool {
        auto* other_it = dynamic_cast<const stepIterator*>(other);
        return other_it != nullptr && _ptr == other_it->_ptr;
    }

    template <typename TYPE>
    original::stepIterator<TYPE>::stepIterator(const stepIterator& other)
        : _ptr(nullptr)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::operator=(
        const stepIterator& other) -> stepIterator&
    {
        if (this == &other) {
            return *this;
        }
        this->_ptr = other._ptr;
        return *this;
    }

    template<typename TYPE>
    auto original::stepIterator<TYPE>::clone() const -> stepIterator* {
        return new stepIterator(*this);
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::hasNext() const -> bool
    {
        return this->isValid();
    }

    template<typename TYPE>
    auto original::stepIterator<TYPE>::hasPrev() const -> bool {
        throw unSupportedMethodError();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::atPrev(const iterator<TYPE>* other) const -> bool
    {
        throw unSupportedMethodError();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::atNext(const iterator<TYPE>* other) const -> bool
    {
        throw unSupportedMethodError();
    }

    template <typename TYPE>
    void original::stepIterator<TYPE>::next() const
    {
        if (!this->isValid()) throw nullPointerError();
        this->_ptr = this->_ptr->getPNext();
    }

    template<typename TYPE>
    auto original::stepIterator<TYPE>::prev() const -> void {
        throw unSupportedMethodError();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::operator+=(const int64_t steps) const -> void
    {
        if (steps < 0) {
            this->operator-=(abs(steps));
            return;
        }

        for (int64_t i = 0; i < steps; i++)
        {
            this->next();
        }
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::operator-=(const int64_t steps) const -> void
    {
        if (steps < 0) {
            this->operator+=(abs(steps));
            return;
        }

        for (int64_t i = 0; i < steps; i++)
        {
            this->prev();
        }
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::operator-(const iterator<TYPE>& other) const -> int64_t
    {
        auto* other_it = dynamic_cast<const stepIterator*>(&other);
        if (other_it == nullptr)
            return this > &other ?
                std::numeric_limits<int64_t>::max() :
                std::numeric_limits<int64_t>::min();
        if (const int64_t pos_dis = ptrDistance(other_it->_ptr, this->_ptr);
            pos_dis != std::numeric_limits<int64_t>::max()) return pos_dis;
        if (const int64_t neg_dis = ptrDistance(this->_ptr, other_it->_ptr);
            neg_dis != std::numeric_limits<int64_t>::max()) return -neg_dis;
        return this->_ptr > other_it->_ptr ?
            std::numeric_limits<int64_t>::max() :
            std::numeric_limits<int64_t>::min();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::getNext() const -> stepIterator* {
        return new stepIterator(this->_ptr->getPNext());
    }

    template<typename TYPE>
    auto original::stepIterator<TYPE>::getPrev() const -> iterator<TYPE>* {
        throw unSupportedMethodError();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::get() -> TYPE&
    {
        if (!this->isValid()) throw nullPointerError();
        return this->_ptr->getVal();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::get() const -> TYPE
    {
        if (!this->isValid()) throw nullPointerError();
        return this->_ptr->getVal();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::set(const TYPE& data) -> void
    {
        if (!this->isValid()) throw nullPointerError();
        this->_ptr->setVal(data);
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::isValid() const -> bool
    {
        return this->_ptr != nullptr;
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::className() const -> std::string
    {
        return "stepIterator";
    }

#endif //STEPITERATOR_H
