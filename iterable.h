#ifndef ITERABLE_H
#define ITERABLE_H

#include "sstream"

namespace original{

    template <typename TYPE>
    class iterable{
        TYPE* ptr_;
    public:
        explicit iterable(TYPE* ptr);
        iterable(const iterable& other);
        iterable& operator=(const iterable& other);
        virtual ~iterable() = default;

        TYPE& operator*();
        TYPE* operator->();
        virtual iterable* getNext();
        virtual iterable* getPrev();
        virtual bool hasNext();
        virtual bool hasPrev();
        virtual iterable& operator++();
        iterable operator++(int);
        virtual iterable& operator--();
        iterable operator--(int);
        bool operator==(const iterable& other) const;
        bool operator<(const iterable& other) const;
        bool operator>(const iterable& other) const;
        bool operator!=(const iterable& other) const;

        std::string toString(bool enter = false);

    };

}

    template <typename TYPE>
    original::iterable<TYPE>::iterable(TYPE* ptr) : ptr_(ptr) {}

    template <typename TYPE>
    original::iterable<TYPE>::iterable(const iterable& other) :ptr_(other.ptr_) {}

    template <typename TYPE>
    auto original::iterable<TYPE>::operator=(const iterable& other) -> iterable&
    {
        if (this == &other) return *this;

        this->ptr_ = other.ptr_;
        return *this;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator*() -> TYPE&
    {
        return *this->ptr_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator->() -> TYPE*
    {
        return this->ptr_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::getNext() -> iterable*
    {
        return this->ptr_ + 1;
    }


    template <typename TYPE>
    auto original::iterable<TYPE>::getPrev() -> iterable*
    {
        return this->ptr_ - 1;
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
        this->ptr_ = this->getNext();
        return *this;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator++(int) -> iterable
    {
        iterable it = *this;
        ++*this;
        return it;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator--() -> iterable&
    {
        this->ptr_ = this->getPrev();
        return *this;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator--(int) -> iterable
    {
        iterable it = *this;
        --*this;
        return it;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator==(const iterable& other) const -> bool
    {
        return this->ptr_ == other->ptr_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator<(const iterable& other) const -> bool
    {
        return this->ptr_ < other->ptr_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator>(const iterable& other) const -> bool
    {
        return this->ptr_ > other->ptr_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::operator!=(const iterable& other) const -> bool
    {
        return !(this == other);
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
