#ifndef ITERABLE_H
#define ITERABLE_H
#pragma once
#include "sstream"
#include "wrapper.h"
#include "config.h"
#include "printable.h"

namespace original{

    template <typename TYPE>
    class iterable : public virtual printable{
        wrapper<TYPE>* wrapper_;
    public:
        explicit iterable(wrapper<TYPE>* ptr);
        iterable(const iterable& other);
        iterable& operator=(const iterable& other);
        virtual ~iterable() = default;

        TYPE operator*();
        virtual iterable* getNext() = 0;
        virtual iterable* getPrev() = 0;
        virtual bool hasNext();
        virtual bool hasPrev();
        virtual iterable& operator++();
        virtual iterable& operator--();
        bool operator==(const iterable& other) const;
        bool operator<(const iterable& other) const;
        bool operator>(const iterable& other) const;
        bool operator!=(const iterable& other) const;

        std::string toString(bool enter) override;

    };

}

    template <typename TYPE>
    original::iterable<TYPE>::iterable(wrapper<TYPE>* ptr) : wrapper_(ptr) {}

    template <typename TYPE>
    original::iterable<TYPE>::iterable(const iterable& other) :wrapper_(other.wrapper_) {}

    template <typename TYPE>
    auto original::iterable<TYPE>::operator=(const iterable& other) -> iterable&
    {
        if (this == &other) return *this;

        this->wrapper_ = other.wrapper_;
        return *this;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator*() -> TYPE
    {
        return this->wrapper_->getVal();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::hasNext() -> bool
    {
        return this->getNext() == nullptr;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::hasPrev() -> bool
    {
        return this->getPrev() == nullptr;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator++() -> iterable&
    {
        iterable* next = this->getNext();
        return *next;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator--() -> iterable&
    {
        iterable* prev = this->getPrev();
        return *prev;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator==(const iterable& other) const -> bool
    {
        return this->wrapper_ == other->wrapper_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator<(const iterable& other) const -> bool
    {
        return this->wrapper_ < other->wrapper_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator>(const iterable& other) const -> bool
    {
        return this->wrapper_ > other->wrapper_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator!=(const iterable& other) const -> bool
    {
        return this->wrapper_ != other.wrapper_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::toString(const bool enter) -> std::string
    {
        std::stringstream ss;
        ss  << "iterator" << "(" << "#"  << this << ")";
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }

#endif //ITERABLE_H
