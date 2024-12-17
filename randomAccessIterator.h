#ifndef RANDOMACCESSITERATOR_H
#define RANDOMACCESSITERATOR_H

#include <container.h>
#include <error.h>
#include <iterator.h>

namespace original{
    template<typename TYPE>
    class randomAccessIterator : public iterator<TYPE>{
    protected:
        mutable TYPE* _ptr;
        mutable const container<TYPE>* _container;
        mutable long long _pos;

        explicit randomAccessIterator(TYPE* ptr, const container<TYPE>* container, long long pos);
    public:
        randomAccessIterator(const randomAccessIterator& other);
        randomAccessIterator& operator=(const randomAccessIterator& other);
        _GLIBCXX_NODISCARD bool hasNext() const override;
        _GLIBCXX_NODISCARD bool hasPrev() const override;
        bool atPrev(const iterator<TYPE>* other) const override;
        bool atNext(const iterator<TYPE>* other) const override;
        void next() const override;
        void prev() const override;
        void operator+=(long long steps) const;
        void operator-=(long long steps) const;

        randomAccessIterator* getNext() override;
        randomAccessIterator* getPrev() override;
        TYPE& get() override;
        const TYPE& get() const override;
        void set(const TYPE& data) override;
        _GLIBCXX_NODISCARD bool isValid() const override;
        _GLIBCXX_NODISCARD std::string className() const override;
    };
}

    template<typename TYPE>
    original::randomAccessIterator<TYPE>::randomAccessIterator(TYPE* ptr, const container<TYPE>* container,
                                                               const long long pos)
        : _ptr{ptr}, _container{container}, _pos(pos) {}

    template<typename TYPE>
    original::randomAccessIterator<TYPE>::randomAccessIterator(const randomAccessIterator &other) : _pos(0) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator=(
        const randomAccessIterator &other) -> randomAccessIterator& {
        if (this == &other) {
            return *this;
        }
        this->_ptr = other._ptr;
        this->_container = other._container;
        this->_pos = other._pos;
        return *this;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::hasNext() const -> bool {
        return this->isValid() && this->_pos <= this->_container->size() - 1;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::hasPrev() const -> bool {
        return this->isValid() && this->_pos >= 1;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::atPrev(const iterator<TYPE> *other) const -> bool {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::atNext(const iterator<TYPE> *other) const -> bool {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::next() const -> void {
        ++this->_pos;
        ++this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::prev() const -> void {
        --this->_pos;
        --this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator+=(long long steps) const -> void {
        this->_pos += steps;
        this->_ptr += steps;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator-=(long long steps) const -> void {
        this->_pos -= steps;
        this->_ptr -= steps;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::getNext() -> randomAccessIterator* {
        if (!this->isValid()) throw outOfBoundError();
        return new randomAccessIterator(this->_ptr + 1, this->_container, this->_pos + 1);
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::getPrev() -> randomAccessIterator* {
        if (!this->isValid()) throw outOfBoundError();
        return new randomAccessIterator(this->_ptr, this->_container, this->_pos - 1);
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::get() -> TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return *this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::set(const TYPE& data) -> void {
        if (!this->isValid()) throw outOfBoundError();
        *this->_ptr = data;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::get() const -> const TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return *this->_ptr;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::isValid() const -> bool {
        return this->_pos >= 0 && this->_pos < this->_container->size();
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::className() const -> std::string {
        return "RandomAccessIterator";
    }

#endif //RANDOMACCESSITERATOR_H
