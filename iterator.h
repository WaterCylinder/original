#ifndef ITERATOR_H
#define ITERATOR_H

#include "printable.h"

namespace original {
    template<typename TYPE>
    class iterator : public printable {
        protected:
            virtual bool equalPtr(const iterator* other) const = 0;
        public:
            TYPE& operator*();
            const TYPE& operator*() const;
            iterator& operator++();
            iterator& operator++(int);
            iterator& operator--();
            iterator& operator--(int);
            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;
            virtual iterator* clone() const = 0;
            explicit operator bool() const;
            [[nodiscard]] virtual bool hasNext() const = 0;
            [[nodiscard]] virtual bool hasPrev() const = 0;
            virtual bool atPrev(const iterator* other) const = 0;
            virtual bool atNext(const iterator* other) const = 0;
            bool atPrev(const iterator& other) const;
            bool atNext(const iterator& other) const;
            virtual void next() const = 0;
            virtual void prev() const = 0;
            virtual iterator* getNext() = 0;
            virtual iterator* getPrev() = 0;
            virtual TYPE& get() = 0;
            virtual const TYPE& get() const = 0;
            virtual void set(const TYPE& data) = 0;
            bool equal(const iterator* other) const;
            bool equal(const iterator& other) const;
            [[nodiscard]] virtual bool isValid() const = 0;
            [[nodiscard]] std::string className() const override;
            [[nodiscard]] std::string toString(bool enter) const override;
            ~iterator() override = default;
    };

    template<typename TYPE>
    class iterAdaptor final : public printable{
            iterator<TYPE>* it_;
        public:
            explicit iterAdaptor(iterator<TYPE>* it);
            TYPE& operator*();
            const TYPE& operator*() const;
            iterAdaptor& operator++();
            bool operator!=(const iterAdaptor& other) const;
            [[nodiscard]] bool isValid() const;
            [[nodiscard]] std::string className() const override;
            [[nodiscard]] std::string toString(bool enter) const override;
            ~iterAdaptor() override;
    };
}

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() -> TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() const -> const TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++() -> iterator& {
        this->next();
        return *this;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++(int) -> iterator& {
        iterator* it = this;
        this->next();
        return *it;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator--() -> iterator& {
        this->prev();
        return *this;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator--(int) -> iterator& {
        iterator* it = this;
        this->prev();
        return *it;
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator==(const iterator &other) const -> bool {
        return this->equal(other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator!=(const iterator &other) const -> bool {
        return !this->equal(other);
    }

    template<typename TYPE>
    original::iterator<TYPE>::operator bool() const {
        return this->isValid();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atPrev(const iterator &other) const -> bool {
        return this->atPrev(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atNext(const iterator &other) const -> bool {
        return this->atNext(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::equal(const iterator *other) const -> bool {
        return this->equalPtr(other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::equal(const iterator &other) const -> bool {
        return this->equal(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::className() const -> std::string {
        return "iterator";
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className() << "(" << formatString(&this->get());
        if (this->isValid()) ss << ", " << formatString(this->get());
        ss << ")";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }

    template<typename TYPE>
    original::iterAdaptor<TYPE>::iterAdaptor(iterator<TYPE>* it) : it_(it) {}

    template<typename TYPE>
    auto original::iterAdaptor<TYPE>::operator*() -> TYPE& {
        return this->it_->operator*();
    }

    template<typename TYPE>
    auto original::iterAdaptor<TYPE>::operator*() const -> const TYPE& {
        return this->it_->operator*();
    }

    template<typename TYPE>
    auto original::iterAdaptor<TYPE>::operator++() -> iterAdaptor& {
        this->it_->next();
        return *this;
    }

    template<typename TYPE>
    auto original::iterAdaptor<TYPE>::operator!=(const iterAdaptor& other) const -> bool {
        return !this->it_->equal(other.it_);
    }

    template<typename TYPE>
    bool original::iterAdaptor<TYPE>::isValid() const {
        return this->it_->isValid();
    }

    template<typename TYPE>
    auto original::iterAdaptor<TYPE>::className() const -> std::string {
        return "iterAdaptor";
    }

    template<typename TYPE>
    auto original::iterAdaptor<TYPE>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className();
        ss << "(" << *this->it_ << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

    template<typename TYPE>
    original::iterAdaptor<TYPE>::~iterAdaptor() {
        delete this->it_;
    }

#endif //ITERATOR_H
