#ifndef ITERATOR_H
#define ITERATOR_H

#include <cloneable.h>
#include <error.h>

#include "printable.h"

namespace original {
    template<typename TYPE>
    class iterator : public printable, public cloneable {
        protected:
            virtual bool equalPtr(const iterator* other) const = 0;
        public:
            TYPE& operator*();
            TYPE operator*() const;
            void operator++() const;
            void operator++(int) const;
            void operator--() const;
            void operator--(int) const;
            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const;
            iterator* clone() const override = 0;
            explicit operator bool() const;
            [[nodiscard]] virtual bool hasNext() const = 0;
            [[nodiscard]] virtual bool hasPrev() const = 0;
            virtual bool atPrev(const iterator* other) const = 0;
            virtual bool atNext(const iterator* other) const = 0;
            bool atPrev(const iterator& other) const;
            bool atNext(const iterator& other) const;
            virtual void next() const = 0;
            virtual void prev() const = 0;
            virtual iterator* getNext() const;
            virtual iterator* getPrev() const = 0;
            virtual TYPE& get() = 0;
            virtual TYPE get() const = 0;
            virtual TYPE getElem() const;
            virtual void set(const TYPE& data) = 0;
            bool equal(const iterator* other) const;
            bool equal(const iterator& other) const;
            [[nodiscard]] virtual bool isValid() const = 0;
            [[nodiscard]] std::string className() const override;
            [[nodiscard]] std::string toString(bool enter) const override;
            ~iterator() override = default;
    };

    template <typename TYPE>
    class baseIterator : public iterator<TYPE>
    {
        public:
            baseIterator* clone() const override = 0;
            ~baseIterator() override = default;
    };
}

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() -> TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() const -> TYPE {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++() const -> void {
        this->next();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++(int) const -> void {
        this->operator++();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator--() const -> void {
        this->prev();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator--(int) const -> void {
        this->operator--();
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
    auto original::iterator<TYPE>::getNext() const -> iterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto it = this->clone();
        it->next();
        return it;
    }

    template <typename TYPE>
    auto original::iterator<TYPE>::getElem() const -> TYPE
    {
        return this->get();
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
        ss << this->className() << "(";
        if (this->isValid()) ss << formatString(this->get());
        ss << ")";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }

#endif //ITERATOR_H
