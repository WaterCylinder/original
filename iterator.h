#ifndef ITERATOR_H
#define ITERATOR_H

#include "printable.h"
#include "error.h"
#include "wrapper.h"
#include <memory>
#include <sstream>

namespace original {
    template<typename TYPE>
    class iterator final : public printable {
    protected:
        mutable wrapper<TYPE>* ptr_;
    public:
        explicit iterator(wrapper<TYPE>* wrapper);
        iterator(const iterator& other);
        TYPE& operator*();
        const TYPE& operator*() const;
        iterator& operator=(const iterator& other);
        iterator& operator=(const TYPE& data);
        iterator& operator++();
        iterator operator++(int);
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        explicit operator bool() const;
        _GLIBCXX_NODISCARD bool hasNext() const;
        _GLIBCXX_NODISCARD bool hasPrev() const;
        bool atPrev(const iterator* other) const;
        bool atNext(const iterator* other) const;
        bool atPrev(const iterator& other) const;
        bool atNext(const iterator& other) const;
        void next();
        void prev();
        void next() const;
        void prev() const;
        std::unique_ptr<iterator> getNext();
        std::unique_ptr<iterator> getPrev();
        TYPE& get();
        const TYPE& get() const;
        void set(TYPE data);
        bool equal(const iterator* other) const;
        bool equal(const iterator& other) const;
        _GLIBCXX_NODISCARD bool isNull() const;
        _GLIBCXX_NODISCARD std::string className() const override;
        std::string toString(bool enter) const override;
        ~iterator() override = default;
    };
}

    template<typename TYPE>
    original::iterator<TYPE>::iterator(wrapper<TYPE>* wrapper) : ptr_(wrapper) {}

    template<typename TYPE>
    original::iterator<TYPE>::iterator(const iterator& other)
        : ptr_(other.ptr_) {}

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() -> TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() const -> const TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator=(const iterator& other) -> iterator& {
        if (this != &other) {
            this->ptr_ = other.ptr_;
        }
        return *this;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator=(const TYPE& data) -> iterator& {
        this->set(data);
        return *this;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++() -> iterator& {
        this->next();
        return *this;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++(int) -> iterator {
        iterator next = *this;
        this->next();
        return next;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator==(const iterator& other) const -> bool {
        return this->equal(other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator!=(const iterator& other) const -> bool {
        return !this->equal(other);
    }

    template<typename TYPE>
    original::iterator<TYPE>::operator bool() const{
        return ptr_ != nullptr;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::hasNext() const -> bool {
        return !this->isNull() && ptr_->getPNext() != nullptr;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::hasPrev() const -> bool {
        return !this->isNull() && ptr_->getPPrev() != nullptr;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atPrev(const iterator* other) const -> bool {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return ptr_->getPNext() == other->ptr_;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atNext(const iterator* other) const -> bool {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return ptr_->getPPrev() == other->ptr_;
    }

    template <typename TYPE>
    auto original::iterator<TYPE>::atPrev(const iterator& other) const -> bool
    {
        return this->atPrev(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atNext(const iterator& other) const -> bool
    {
        return this->atNext(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::next() -> void {
        if (this->isNull()) {
            throw nullPointerError();
        }
        ptr_ = ptr_->getPNext();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::prev() -> void {
        if (this->isNull()) {
            throw nullPointerError();
        }
        ptr_ = ptr_->getPPrev();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::next() const -> void {
        if (this->isNull()) {
            throw nullPointerError();
        }
        ptr_ = ptr_->getPNext();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::prev() const -> void {
        if (this->isNull()) {
            throw nullPointerError();
        }
        ptr_ = ptr_->getPPrev();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::getNext() -> std::unique_ptr<iterator>
    {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return std::make_unique<iterator>(ptr_->getPNext());
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::getPrev() -> std::unique_ptr<iterator>
    {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return std::make_unique<iterator>(ptr_->getPPrev());
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::get() -> TYPE& {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return ptr_->getVal();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::get() const -> const TYPE& {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return ptr_->getVal();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::set(TYPE data) -> void {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return ptr_->setVal(data);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::equal(const iterator* other) const -> bool {
        return this->ptr_ == other->ptr_;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::equal(const iterator& other) const -> bool {
        return this->ptr_ == other.ptr_;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::isNull() const {
        return this->ptr_ == nullptr;
    }

    template <typename TYPE>
    std::string original::iterator<TYPE>::className() const
    {
        return "iterator";
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className() << "(" << formatString(this->ptr_);
        if (!this->isNull()) ss << ", " << formatString(this->get());
        ss << ")";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }

#endif // ITERATOR_H
