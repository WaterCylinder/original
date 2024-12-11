#ifndef RANDOMACCESSITERATOR_H
#define RANDOMACCESSITERATOR_H

#include <error.h>

namespace original{
    template<typename TYPE>
    class RandomAccessIterator final : public Iterator<TYPE>{
        mutable TYPE* _ptr;
        mutable const container<TYPE>* _container;
        mutable long long _pos;

        explicit RandomAccessIterator(TYPE* ptr, const container<TYPE>* container, long long pos);
    public:
        RandomAccessIterator(const RandomAccessIterator& other);
        RandomAccessIterator& operator=(const RandomAccessIterator& other);
        _GLIBCXX_NODISCARD bool hasNext() const override;
        _GLIBCXX_NODISCARD bool hasPrev() const override;
        bool atPrev(const Iterator<TYPE>* other) const override;
        bool atNext(const Iterator<TYPE>* other) const override;
        void next() const override;
        void prev() const override;
        void operator+=(long long steps) const;
        void operator-=(long long steps) const;
        std::unique_ptr<Iterator<TYPE>> getNext() override;
        std::unique_ptr<Iterator<TYPE>> getPrev() override;
        TYPE& get() override;
        const TYPE& get() const override;
        void set(TYPE data) override;
        _GLIBCXX_NODISCARD bool isValid() const override;
        _GLIBCXX_NODISCARD std::string className() const override;
    };
}

    template<typename TYPE>
    original::RandomAccessIterator<TYPE>::RandomAccessIterator(TYPE *ptr, const container<TYPE>* container,
                                                               const long long pos)
        : _ptr{ptr}, _container{container}, _pos(pos) {}

    template<typename TYPE>
    original::RandomAccessIterator<TYPE>::RandomAccessIterator(const RandomAccessIterator &other) : _pos(0) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::operator=(
        const RandomAccessIterator &other) -> RandomAccessIterator& {
        if (this != &other) {
            return *this;
        }
        this->_ptr = other._ptr;
        this->_container = other._container;
        this->_pos = other._pos;
        return *this;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::hasNext() const -> bool {
        return this->isValid() && this->_pos <= this->_container->size() - 1;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::hasPrev() const -> bool {
        return this->isValid() && this->_pos >= 1;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::atPrev(const Iterator<TYPE> *other) const -> bool {
        const auto other_it = static_cast<const RandomAccessIterator>(*other);
        return this->_pos + 1 == other_it->_pos;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::atNext(const Iterator<TYPE> *other) const -> bool {
        const auto other_it = static_cast<const RandomAccessIterator>(*other);
        return this->_pos - 1 == other_it._pos;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::next() const -> void {
        ++this->_pos;
        ++this->_ptr;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::prev() const -> void {
        --this->_pos;
        --this->_ptr;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::operator+=(long long steps) const -> void {
        this->_pos += steps;
        this->_ptr += steps;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::operator-=(long long steps) const -> void {
        this->_pos -= steps;
        this->_ptr -= steps;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::getNext() -> std::unique_ptr<Iterator<TYPE> > {
        return std::make_unique<RandomAccessIterator>(this->_ptr + 1, this->_container, this->_pos + 1);
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::getPrev() -> std::unique_ptr<Iterator<TYPE> > {
        return std::make_unique<RandomAccessIterator>(this->_ptr, this->_container, this->_pos - 1);
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::get() -> TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return *this->_ptr;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::set(TYPE data) -> void {
        if (!this->isValid()) throw outOfBoundError();
        *this->_ptr = data;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::get() const -> const TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return *this->_ptr;
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::isValid() const -> bool {
        return this->_pos >= 0 && this->_pos < this->_container->size();
    }

    template<typename TYPE>
    auto original::RandomAccessIterator<TYPE>::className() const -> std::string {
        return "RandomAccessIterator";
    }

#endif //RANDOMACCESSITERATOR_H
