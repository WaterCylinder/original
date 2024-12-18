#ifndef STEPITERATOR_H
#define STEPITERATOR_H

#include "iterator.h"
#include "wrapper.h"
#include "error.h"

namespace original
{
    template<typename TYPE>
    class stepIterator : public iterator<TYPE>
    {
        protected:
            mutable wrapper<TYPE>* _ptr;

            explicit stepIterator(wrapper<TYPE>* ptr);
            bool equalPtr(const iterator<TYPE> * other) const override;
        public:
            stepIterator(const stepIterator& other);
            stepIterator& operator=(const stepIterator& other);
            stepIterator* clone() const override;
            _GLIBCXX_NODISCARD bool hasNext() const override;
            _GLIBCXX_NODISCARD bool hasPrev() const override;
            bool atPrev(const iterator<TYPE>* other) const override;
            bool atNext(const iterator<TYPE>* other) const override;
            void next() const override;
            void prev() const override;
            stepIterator* getNext() override;
            iterator<TYPE>* getPrev() override;
            TYPE& get() override;
            const TYPE& get() const override;
            void set(const TYPE& data) override;
            _GLIBCXX_NODISCARD bool isValid() const override;
            _GLIBCXX_NODISCARD std::string className() const override;
    };
}

    template <typename TYPE>
    original::stepIterator<TYPE>::stepIterator(wrapper<TYPE>* ptr)
        : _ptr(ptr) {}

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
    auto original::stepIterator<TYPE>::getNext() -> stepIterator* {
        return new stepIterator(this->_ptr->getPNext());
    }

    template<typename TYPE>
    auto original::stepIterator<TYPE>::getPrev() -> iterator<TYPE> *{
        throw unSupportedMethodError();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::get() -> TYPE&
    {
        if (!this->isValid()) throw nullPointerError();
        return this->_ptr->getVal();
    }

    template <typename TYPE>
    auto original::stepIterator<TYPE>::get() const -> const TYPE&
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
