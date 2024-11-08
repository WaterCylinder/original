#ifndef ITERATOR_H
#define ITERATOR_H

#include "printable.h"
#include "wrapper.h"
#include <memory>

namespace original {
    template<typename TYPE>
    class iterator final : public printable {
    protected:
        wrapper<TYPE>* ptr_;
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
        _GLIBCXX_NODISCARD bool hasNext() const;
        _GLIBCXX_NODISCARD bool hasPrev() const;
        bool atPrev(const iterator* other) const;
        bool atNext(const iterator* other) const;
        void next();
        void prev();
        std::unique_ptr<iterator> getNext();
        std::unique_ptr<iterator> getPrev();
        TYPE& get();
        const TYPE& get() const;
        void set(TYPE data);
        bool equal(const iterator* other) const;
        _GLIBCXX_NODISCARD bool isNull() const;
        std::string toString(bool enter) override;
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
            ptr_ = other.ptr_;
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
        return this->equal(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator!=(const iterator& other) const -> bool {
        return !this->equal(&other);
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
    std::unique_ptr<original::iterator<TYPE>> original::iterator<TYPE>::getNext() {
        if (this->isNull()) {
            throw nullPointerError();
        }
        return std::make_unique<iterator>(ptr_->getPNext());
    }

    template<typename TYPE>
    std::unique_ptr<original::iterator<TYPE>> original::iterator<TYPE>::getPrev() {
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
    bool original::iterator<TYPE>::isNull() const {
        return ptr_ == nullptr;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::toString(const bool enter) -> std::string {
        std::stringstream ss;
        ss << "iterator" << "(" << "#" << this->ptr_ << ")";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }

#endif // ITERATOR_H
