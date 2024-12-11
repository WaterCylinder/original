#ifndef _ITERATOR_H
#define _ITERATOR_H

namespace original {
    template<typename TYPE>
    class Iterator : public printable {
        public:
            TYPE& operator*();
            const TYPE& operator*() const;
            Iterator& operator++();
            Iterator& operator++(int);
            Iterator& operator--();
            Iterator& operator--(int);
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;
            explicit operator bool() const;
            _GLIBCXX_NODISCARD virtual bool hasNext() const = 0;
            _GLIBCXX_NODISCARD virtual bool hasPrev() const = 0;
            virtual bool atPrev(const Iterator* other) const = 0;
            virtual bool atNext(const Iterator* other) const = 0;
            bool atPrev(const Iterator& other) const;
            bool atNext(const Iterator& other) const;
            virtual void next() const = 0;
            virtual void prev() const = 0;
            virtual std::unique_ptr<Iterator> getNext() = 0;
            virtual std::unique_ptr<Iterator> getPrev() = 0;
            virtual TYPE& get() = 0;
            virtual const TYPE& get() const = 0;
            virtual void set(TYPE data) = 0;
            bool equal(const Iterator* other) const;
            bool equal(const Iterator& other) const;
            _GLIBCXX_NODISCARD virtual bool isValid() const = 0;
            _GLIBCXX_NODISCARD std::string className() const override;
            _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
            ~Iterator() override = default;
    };
}

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator*() -> TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator*() const -> const TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator++() -> Iterator& {
        this->next();
        return *this;
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator++(int) -> Iterator& {
        Iterator* it = this;
        this->next();
        return *it;
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator--() -> Iterator& {
        this->prev();
        return *this;
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator--(int) -> Iterator& {
        Iterator* it = this;
        this->prev();
        return *it;
    }

template<typename TYPE>
    auto original::Iterator<TYPE>::operator==(const Iterator &other) const -> bool {
        return this->equal(other);
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::operator!=(const Iterator &other) const -> bool {
        return !this->equal(other);
    }

    template<typename TYPE>
    original::Iterator<TYPE>::operator bool() const {
        return this->isValid();
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::atPrev(const Iterator &other) const -> bool {
        return this->atPrev(&other);
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::atNext(const Iterator &other) const -> bool {
        return this->atNext(&other);
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::equal(const Iterator *other) const -> bool {
        return &this->get() == &other->get();
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::equal(const Iterator &other) const -> bool {
        return this->equal(&other);
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::className() const -> std::string {
        return "iterator";
    }

    template<typename TYPE>
    auto original::Iterator<TYPE>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className() << "(" << formatString(&this->get());
        if (!this->isValid()) ss << ", " << formatString(this->get());
        ss << ")";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }

#endif //_ITERATOR_H
