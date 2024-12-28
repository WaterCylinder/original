#ifndef RANDOMACCESSITERATOR_H
#define RANDOMACCESSITERATOR_H

#include <container.h>
#include <error.h>
#include <iterator.h>

namespace original{
    template<typename TYPE>
    class randomAccessIterator : public baseIterator<TYPE>{
    protected:
        mutable TYPE* _ptr;
        mutable const container<TYPE>* _container;
        mutable int64_t _pos;

        explicit randomAccessIterator(TYPE* ptr, const container<TYPE>* container, int64_t pos);
        bool equalPtr(const iterator<TYPE> * other) const override;
    public:
        randomAccessIterator(const randomAccessIterator& other);
        randomAccessIterator& operator=(const randomAccessIterator& other);
        randomAccessIterator* clone() const override;
        [[nodiscard]] bool hasNext() const override;
        [[nodiscard]] bool hasPrev() const override;
        bool atPrev(const iterator<TYPE>* other) const override;
        bool atNext(const iterator<TYPE>* other) const override;
        void next() const override;
        void prev() const override;
        void operator+=(int64_t steps) const;
        void operator-=(int64_t steps) const;
        randomAccessIterator* getNext() const override;
        randomAccessIterator* getPrev() const override;
        TYPE& get() override;
        TYPE get() const override;
        void set(const TYPE& data) override;
        [[nodiscard]] bool isValid() const override;
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE>
    original::randomAccessIterator<TYPE>::randomAccessIterator(TYPE* ptr, const container<TYPE>* container,
                                                               const int64_t pos)
        : _ptr{ptr}, _container{container}, _pos(pos) {}

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::equalPtr(const iterator<TYPE> *other) const -> bool {
        auto* other_it = dynamic_cast<const randomAccessIterator*>(other);
        return other_it != nullptr && _ptr == other_it->_ptr;
    }

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
    auto original::randomAccessIterator<TYPE>::clone() const -> randomAccessIterator* {
        return new randomAccessIterator{_ptr, _container, _pos};
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
    auto original::randomAccessIterator<TYPE>::operator+=(int64_t steps) const -> void {
        this->_pos += steps;
        this->_ptr += steps;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::operator-=(int64_t steps) const -> void {
        this->_pos -= steps;
        this->_ptr -= steps;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::getNext() const -> randomAccessIterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto it = this->clone();
        it->next();
        return it;
    }

    template<typename TYPE>
    auto original::randomAccessIterator<TYPE>::getPrev() const -> randomAccessIterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto it = this->clone();
        it->prev();
        return it;
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
    auto original::randomAccessIterator<TYPE>::get() const -> TYPE
    {
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
