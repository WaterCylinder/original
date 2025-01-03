#ifndef VECTOR_H
#define VECTOR_H

#include "serial.h"
#include "iterationStream.h"
#include "array.h"

namespace original{
    template <typename TYPE>
    class vector final : public serial<TYPE>, public iterationStream<TYPE>{
        uint32_t size_;
        const uint32_t INNER_SIZE_INIT = 16;
        uint32_t max_size;
        uint32_t inner_begin;
        TYPE* body;

        static TYPE* vectorArrayInit(uint32_t size);
        static void moveElements(TYPE* old_body, uint32_t inner_idx,
                                 uint32_t len, TYPE* new_body, int64_t offset);
        [[nodiscard]] uint32_t toInnerIdx(int64_t index) const;
        [[nodiscard]] bool outOfMaxSize(uint32_t increment) const;
        void grow(uint32_t new_size);
        void adjust(uint32_t increment);

    public:
        class Iterator final : public randomAccessIterator<TYPE>
        {
                explicit Iterator(TYPE* ptr, const vector* container, int64_t pos);
            public:
                friend vector;
                Iterator(const Iterator& other);
                Iterator& operator=(const Iterator& other);
                Iterator* clone() const override;
                bool atPrev(const iterator<TYPE> *other) const override;
                bool atNext(const iterator<TYPE> *other) const override;
                [[nodiscard]] std::string className() const override;
        };

        explicit vector();
        vector(const vector& other);
        vector(const std::initializer_list<TYPE>& list);
        explicit vector(const array<TYPE>& arr);
        vector& operator=(const vector& other);
        bool operator==(const vector& other) const;
        [[nodiscard]] uint32_t size() const override;
        TYPE& data() const;
        TYPE get(int64_t index) const override;
        TYPE& operator[](int64_t index) override;
        void set(int64_t index, const TYPE &e) override;
        uint32_t indexOf(const TYPE &e) const override;
        void pushBegin(const TYPE &e) override;
        void push(int64_t index, const TYPE &e) override;
        void pushEnd(const TYPE &e) override;
        TYPE popBegin() override;
        TYPE pop(int64_t index) override;
        TYPE popEnd() override;
        Iterator* begins() const override;
        Iterator* ends() const override;
        [[nodiscard]] std::string className() const override;
        ~vector() override;
    };
}
    template <typename TYPE>
    auto original::vector<TYPE>::vectorArrayInit(const uint32_t size) -> TYPE* {
        auto arr = new TYPE[size];
        for (uint32_t i = 0; i < size; i++) {
            arr[i] = TYPE{};
        }
        return arr;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::moveElements(TYPE* old_body, const uint32_t inner_idx,
                                              const uint32_t len, TYPE* new_body, const int64_t offset) -> void{
        if (offset > 0)
        {
            for (uint32_t i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + len - 1 - i] = old_body[inner_idx + len - 1 - i];
            }
        }else
        {
            for (uint32_t i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + i] = old_body[inner_idx + i];
            }
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::toInnerIdx(int64_t index) const -> uint32_t
    {
        return this->inner_begin + index;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::outOfMaxSize(uint32_t increment) const -> bool
    {
        return this->inner_begin + this->size() + increment > this->max_size - 1 || this->inner_begin - increment < 0;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::grow(const uint32_t new_size) -> void
    {
        TYPE* new_body = vector::vectorArrayInit(new_size);
        uint32_t new_begin = (new_size - 1) / 4;
        const int64_t offset = static_cast<int64_t>(new_begin) - static_cast<int64_t>(this->inner_begin);
        vector::moveElements(this->body, this->inner_begin,
                             this->size(), new_body, offset);
        delete[] this->body;
        this->body = new_body;
        this->max_size = new_size;
        this->inner_begin = new_begin;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::adjust(uint32_t increment) -> void {
        if (!this->outOfMaxSize(increment)) {
            return;
        }
        if (this->max_size >= this->size_ + increment) {
            uint32_t new_begin = (this->max_size - this->size() - increment) / 2;
            const int64_t offset = static_cast<int64_t>(new_begin) - static_cast<int64_t>(this->inner_begin);
            vector::moveElements(this->body, this->inner_begin, this->size(),
                                 this->body, offset);
            this->inner_begin = new_begin;
        } else {
            const uint32_t new_max_size = (this->size() + increment) * 2;
            this->grow(new_max_size);
        }
    }

    template <typename TYPE>
    original::vector<TYPE>::Iterator::Iterator(TYPE* ptr, const vector* container, int64_t pos)
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
    auto original::vector<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return this->_ptr + 1 == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it->_ptr + 1 == this->_ptr;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::className() const -> std::string {
        return "vector::Iterator";
    }

    template <typename TYPE>
    original::vector<TYPE>::vector() : size_(0), max_size(this->INNER_SIZE_INIT),
        inner_begin((this->INNER_SIZE_INIT - 1)/2), body(vector::vectorArrayInit(this->INNER_SIZE_INIT)) {}

    template<typename TYPE>
    original::vector<TYPE>::vector(const vector& other) : vector(){
        this->operator=(other);
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(const std::initializer_list<TYPE>& list) : vector()
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
        for (uint32_t i = 0; i < this->size(); ++i) {
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
        for (uint32_t i = 0; i < this->size(); ++i) {
            uint32_t index = this->toInnerIdx(i);
            if (this->body[index] != other.body[index]){
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(const array<TYPE>& arr) : vector()
    {
        this->adjust(arr.size());
        for (uint32_t i = 0; i < arr.size(); i += 1)
        {
            this->body[this->toInnerIdx(i)] = arr.get(i);
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::size() const -> uint32_t
    {
        return this->size_;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::data() const -> TYPE& {
        return this->body[this->toInnerIdx(0)];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::get(int64_t index) const -> TYPE
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::operator[](int64_t index) -> TYPE&
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::set(int64_t index, const TYPE &e) -> void
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        this->body[index] = e;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::indexOf(const TYPE &e) const -> uint32_t
    {
        for (uint32_t i = 0; i < this->size(); i += 1)
        {
            if (this->get(i) == e)
            {
                return i;
            }
        }
        return this->size();
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushBegin(const TYPE &e) -> void
    {
        this->adjust(1);
        this->inner_begin -= 1;
        this->body[this->toInnerIdx(0)] = e;
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::push(int64_t index, const TYPE &e) -> void
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
            uint32_t rel_idx = index - this->inner_begin;
            if (index - this->inner_begin <= (this->size() - 1) / 2)
            {
                vector::moveElements(this->body, this->inner_begin,
                                     rel_idx + 1, this->body, -1);
                this->inner_begin -= 1;
            }else
            {
                vector::moveElements(this->body, index,
                                     this->size() - rel_idx, this->body, 1);
            }
            this->body[index] = e;
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushEnd(const TYPE &e) -> void
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
    auto original::vector<TYPE>::pop(int64_t index) -> TYPE
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
        uint32_t rel_idx = index - this->inner_begin;
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
        return new Iterator(&this->body[this->toInnerIdx(0)], this, 0);
    }

    template<typename TYPE>
    auto original::vector<TYPE>::ends() const -> Iterator* {
        return new Iterator(&this->body[this->toInnerIdx(this->size() - 1)], this, this->size() - 1);
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
