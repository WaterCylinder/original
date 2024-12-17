#ifndef VECTOR_H
#define VECTOR_H

#include "serial.h"
#include "iterationStream.h"
#include "array.h"

namespace original{
    template <typename TYPE>
    class vector final : public serial<TYPE>, public iterationStream<TYPE>{
        size_t size_;
        const size_t INNER_SIZE_INIT = 16;
        size_t max_size;
        size_t inner_begin;
        TYPE* body;

        static TYPE* vectorArrayInit(size_t size);
        static void moveElements(TYPE* old_body, size_t inner_idx,
                          size_t len, TYPE* new_body, int offset);
        [[__nodiscard__]] size_t toInnerIdx(int index) const;
        [[__nodiscard__]] bool outOfMaxSize(size_t increment) const;
        void grow(size_t new_size);
        void adjust(size_t increment);

    public:
        class Iterator final : public randomAccessIterator<TYPE>
        {
                explicit Iterator(TYPE* ptr, const vector* container, long long pos);
            public:
                friend vector;
                Iterator(const Iterator& other);
                Iterator& operator=(const Iterator& other);
                bool atPrev(const iterator<TYPE> *other) const override;
                bool atNext(const iterator<TYPE> *other) const override;
        };

        explicit vector();
        vector(const vector& other);
        vector(std::initializer_list<TYPE> list);
        explicit vector(array<TYPE> arr);
        vector& operator=(const vector& other);
        bool operator==(const vector& other) const;
        [[__nodiscard__]] size_t size() const override;
        TYPE get(int index) const override;
        TYPE& operator[](int index) override;
        void set(int index, TYPE e) override;
        size_t indexOf(TYPE e) const override;
        void pushBegin(TYPE e) override;
        void push(int index, TYPE e) override;
        void pushEnd(TYPE e) override;
        TYPE popBegin() override;
        TYPE pop(int index) override;
        TYPE popEnd() override;
        Iterator* begins() const override;
        Iterator* ends() const override;
        [[__nodiscard__]] std::string className() const override;
        ~vector() override;
    };
}
    template <typename TYPE>
    auto original::vector<TYPE>::vectorArrayInit(const size_t size) -> TYPE* {
        return new TYPE[size];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::moveElements(TYPE* old_body, const size_t inner_idx,
        const size_t len, TYPE* new_body, const int offset) -> void{
        if (offset > 0)
        {
            for (int i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + len - 1 - i] = old_body[inner_idx + len - 1 - i];
            }
        }else
        {
            for (int i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + i] = old_body[inner_idx + i];
            }
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::toInnerIdx(int index) const -> size_t
    {
        return this->inner_begin + index;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::outOfMaxSize(size_t increment) const -> bool
    {
        return this->inner_begin + this->size() + increment > this->max_size || this->inner_begin - increment < 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::grow(const size_t new_size) -> void
    {
        TYPE* new_body = vector::vectorArrayInit(new_size);
        size_t new_begin = (new_size - 1) / 4;
        vector::moveElements(this->body, this->inner_begin,
                             this->size(), new_body, new_begin - this->inner_begin);
        delete[] this->body;
        this->body = new_body;
        this->max_size = new_size;
        this->inner_begin = new_begin;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::adjust(size_t increment) -> void {
        if (!this->outOfMaxSize(increment)) {
            return;
        }
        if (this->max_size >= this->size_ + increment) {
            size_t new_begin = (this->max_size - this->size() - increment) / 2;
            vector::moveElements(this->body, this->inner_begin, this->size(), this->body,
                                 new_begin - this->inner_begin);
            this->inner_begin = new_begin;
        } else {
            const size_t new_max_size = (this->size() + increment) * 2;
            this->grow(new_max_size);
        }
    }

    template <typename TYPE>
    original::vector<TYPE>::Iterator::Iterator(TYPE* ptr, const vector* container, long long pos)
        : randomAccessIterator<TYPE>(ptr, container, pos) {}

    template <typename TYPE>
    original::vector<TYPE>::Iterator::Iterator(const Iterator& other)
        : randomAccessIterator<TYPE>(nullptr, nullptr, 0)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::Iterator::operator=(const Iterator& other) -> Iterator&
    {
        if (this == &other) {
            return *this;
        }
        randomAccessIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return ++this->_ptr == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return ++other_it->_ptr == this->_ptr;
    }

    template <typename TYPE>
    original::vector<TYPE>::vector() : size_(0), max_size(this->INNER_SIZE_INIT),
        inner_begin((this->INNER_SIZE_INIT - 1)/2), body(vector::vectorArrayInit(this->INNER_SIZE_INIT)) {}

    template<typename TYPE>
    original::vector<TYPE>::vector(const vector& other) : vector(){
        this->operator=(other);
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(std::initializer_list<TYPE> list) : vector()
    {
        this->adjust(list.size());
        for (const TYPE& e: list)
        {
            this->body[this->inner_begin + this->size()] = e;
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::vector<TYPE>::operator=(const vector& other) -> vector&
    {
        if (this == &other) return *this;
        delete[] this->body;
        this->max_size = other.max_size;
        this->inner_begin = other.inner_begin;
        this->size_ = other.size_;
        this->body = vector::vectorArrayInit(this->max_size);
        for (int i = 0; i < this->size(); ++i) {
            const TYPE& data = other.body[this->toInnerIdx(i)];
            this->body[this->toInnerIdx(i)] = data;
        }
        return *this;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::operator==(const vector& other) const -> bool
    {
        if (this == &other) return true;
        if (this->size() != other.size()) return false;
        for (int i = 0; i < this->size(); ++i) {
            size_t index = this->toInnerIdx(i);
            if (this->body[index] != other.body[index]){
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(array<TYPE> arr) : vector()
    {
        this->adjust(arr.size());
        for (int i = 0; i < arr.size(); i += 1)
        {
            this->body[this->toInnerIdx(i)] = arr.get(i);
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::size() const -> size_t
    {
        return this->size_;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::get(int index) const -> TYPE
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::operator[](int index) -> TYPE&
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::set(int index, TYPE e) -> void
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        this->body[index] = e;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::indexOf(TYPE e) const -> size_t
    {
        for (int i = 0; i < this->size(); i += 1)
        {
            if (this->get(i) == e)
            {
                return i;
            }
        }
        return this->size();
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushBegin(TYPE e) -> void
    {
        this->adjust(1);
        this->inner_begin -= 1;
        this->body[this->toInnerIdx(0)] = e;
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::push(int index, TYPE e) -> void
    {
        if (this->parseNegIndex(index) == this->size())
        {
            this->pushEnd(e);
        }else if (this->parseNegIndex(index) == 0)
        {
            this->pushBegin(e);
        }else
        {
            if (this->indexOutOfBound(index))
            {
                throw outOfBoundError();
            }
            this->adjust(1);
            index = this->toInnerIdx(this->parseNegIndex(index));
            size_t rel_idx = index - this->inner_begin;
            if (index - this->inner_begin <= (this->size() - 1) / 2)
            {
                vector::moveElements(this->body, this->inner_begin,
                                     rel_idx + 1, this->body, -1);
                this->inner_begin -= 1;
            }else
            {
                vector::moveElements(this->body, index,
                    this->size() - 1 - rel_idx, this->body, 1);
            }
            this->body[index] = e;
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushEnd(TYPE e) -> void
    {
        this->adjust(1);
        this->body[this->toInnerIdx(this->size())] = e;
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::popBegin() -> TYPE
    {
        if (this->size() == 0){
            throw noElementError();
        }
        TYPE res = this->get(0);
        this->inner_begin += 1;
        this->size_ -= 1;
        return res;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pop(int index) -> TYPE
    {
        if (this->parseNegIndex(index) == 0)
        {
            return this->popBegin();
        }
        if (this->parseNegIndex(index) == this->size() - 1)
        {
            return this->popEnd();
        }
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        TYPE res = this->get(index);
        index = this->toInnerIdx(this->parseNegIndex(index));
        size_t rel_idx = index - this->inner_begin;
        if (index - this->inner_begin <= (this->size() - 1) / 2)
        {
            vector::moveElements(this->body, this->inner_begin,
                                 rel_idx, this->body, 1);
            this->inner_begin += 1;
        }else
        {
            vector::moveElements(this->body, index + 1,
                this->size() - 1 - rel_idx, this->body, -1);
        }
        this->size_ -= 1;
        return res;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::popEnd() -> TYPE
    {
        if (this->size() == 0){
            throw noElementError();
        }
        TYPE res = this->get(this->size() - 1);
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->body, this, 0);
    }

    template<typename TYPE>
    auto original::vector<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->body, this, this->size() - 1);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::className() const -> std::string
    {
        return "vector";
    }

    template <typename TYPE>
    original::vector<TYPE>::~vector() {
        delete[] this->body;
    }

#endif //VECTOR_H
