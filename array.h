#ifndef ARRAY_H
#define ARRAY_H
#pragma once


#include <initializer_list>
#include <iterationStream.h>
#include <randomAccessIterator.h>

#include "error.h"
#include "serial.h"

namespace original{

    template<typename TYPE>
    class array final : public iterationStream<TYPE>, public serial<TYPE>{
        uint32_t size_;
        TYPE* body;

        public:
        class Iterator final : public randomAccessIterator<TYPE>
        {
            explicit Iterator(TYPE* ptr, const array* container, int64_t pos);
        public:
            friend array;
            Iterator(const Iterator& other);
            Iterator& operator=(const Iterator& other);
            Iterator* clone() const override;
            bool atPrev(const iterator<TYPE> *other) const override;
            bool atNext(const iterator<TYPE> *other) const override;
            [[nodiscard]] std::string className() const override;
        };

        explicit array(uint32_t size = 0);
        array(std::initializer_list<TYPE> lst);
        array(const array& other);
        array& operator=(const array& other);
        bool operator==(const array& other) const;
        [[nodiscard]] uint32_t size() const override;
        TYPE& data() const;
        TYPE get(int64_t index) const override;
        TYPE& operator[](int64_t index) override;
        void set(int64_t index, const TYPE &e) override;
        uint32_t indexOf(const TYPE &e) const override;
        Iterator* begins() const override;
        Iterator* ends() const override;
        [[nodiscard]] std::string className() const override;
        ~array() override;
    };

}

    template <typename TYPE>
    original::array<TYPE>::Iterator::Iterator(TYPE* ptr, const array* container, int64_t pos)
        : randomAccessIterator<TYPE>(ptr, container, pos) {}

    template <typename TYPE>
    original::array<TYPE>::Iterator::Iterator(const Iterator& other)
        : randomAccessIterator<TYPE>(nullptr, nullptr, 0)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::array<TYPE>::Iterator::operator=(const Iterator& other) -> Iterator&
    {
        if (this == &other) {
            return *this;
        }
        randomAccessIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return this->_ptr + 1 == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it->_ptr + 1 == this->_ptr;
    }

    template<typename TYPE>
    auto original::array<TYPE>::Iterator::className() const -> std::string {
        return "array::Iterator";
    }

    template <typename TYPE>
    original::array<TYPE>::array(const uint32_t size)
        : size_(size), body(new TYPE[size_]) {
        for (uint32_t i = 0; i < this->size(); ++i) {
            this->body[i] = TYPE{};
        }
    }

    template <typename TYPE>
    original::array<TYPE>::array(std::initializer_list<TYPE> lst)
        : size_(lst.size()), body(new TYPE[size_]) {
        uint32_t i = 0;
        for (auto e : lst) {
            this->body[i] = e;
            i += 1;
        }
    }

    template <typename TYPE>
    original::array<TYPE>::array(const array& other)
        : size_(other.size_), body(new TYPE[size_]) {
        for (uint32_t i = 0; i < size_; i++) {
            body[i] = other.body[i];
        }
    }

    template <typename TYPE>
    auto original::array<TYPE>::operator=(const array& other) -> array&
    {
        if (this == &other) return *this;

        delete[] this->body;

        this->size_ = other.size_;
        this->body = new TYPE[this->size_];
        for (uint32_t i = 0; i < this->size_; i++) {
            this->body[i] = other.body[i];
        }
        return *this;
    }

    template <typename TYPE>
    auto original::array<TYPE>::operator==(const array& other) const -> bool
    {
        if (this == &other) return true;
        if (this->size() != other.size()) return false;
        for (uint32_t i = 0; i < this->size(); i ++)
        {
            if (this->body[i] != other.body[i])
            {
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    original::array<TYPE>::~array() {
        delete[] this->body;
    }

    template <typename TYPE>
    auto original::array<TYPE>::size() const -> uint32_t
    {
        return this->size_;
    }

    template<typename TYPE>
    TYPE& original::array<TYPE>::data() const {
        return this->body[0];
    }

    template <typename TYPE>
    auto original::array<TYPE>::get(int64_t index) const -> TYPE
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        return this->body[this->parseNegIndex(index)];
    }

    template <typename TYPE>
    auto original::array<TYPE>::operator[](int64_t index) -> TYPE&
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        return this->body[this->parseNegIndex(index)];
    }

    template <typename TYPE>
    auto original::array<TYPE>::set(int64_t index, const TYPE &e) -> void
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        this->body[this->parseNegIndex(index)] = e;
    }

    template <typename TYPE>
    auto original::array<TYPE>::indexOf(const TYPE &e) const -> uint32_t
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

    template<typename TYPE>
    auto original::array<TYPE>::begins() const -> Iterator* {
        return new Iterator(&this->body[0], this, 0);
    }

    template<typename TYPE>
    auto original::array<TYPE>::ends() const -> Iterator* {
        return new Iterator(&this->body[this->size() - 1], this, this->size() - 1);
    }

    template <typename TYPE>
    std::string original::array<TYPE>::className() const
    {
        return "array";
    }

#endif //ARRAY_H
