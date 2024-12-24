#ifndef ITERABLE_H
#define ITERABLE_H

#include <error.h>
#include <transform.h>

#include "iterator.h"

namespace original{
    template <typename TYPE>
    class iterable{
        class iterAdaptor final : public printable{
            iterator<TYPE>* it_;

            explicit iterAdaptor(iterator<TYPE>* it);
        public:
            friend class iterable;
            TYPE& operator*();
            const TYPE& operator*() const;
            TYPE get() const;
            iterAdaptor& operator++();
            bool operator!=(const iterAdaptor& other) const;
            [[nodiscard]] bool isValid() const;
            [[nodiscard]] std::string className() const override;
            [[nodiscard]] std::string toString(bool enter) const override;
            ~iterAdaptor() override;
        };
    public:
        virtual ~iterable() = default;
        virtual iterator<TYPE>* begins() const = 0;
        virtual iterator<TYPE>* ends() const = 0;

        iterAdaptor begin();
        iterAdaptor end();
        iterAdaptor begin() const;
        iterAdaptor end() const;

        template<typename Callback = transform<TYPE>>
        void forEach(Callback operation = Callback{});
    };
}

    template<typename TYPE>
    original::iterable<TYPE>::iterAdaptor::iterAdaptor(iterator<TYPE>* it) : it_(it) {}

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator*() -> TYPE& {
        return this->it_->operator*();
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator*() const -> const TYPE& {
        return this->it_->operator*();
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::get() const -> TYPE {
        return this->it_->getElem();
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator++() -> iterAdaptor& {
        this->it_->next();
        return *this;
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator!=(const iterAdaptor& other) const -> bool {
        return !this->it_->equal(other.it_);
    }

    template<typename TYPE>
    bool original::iterable<TYPE>::iterAdaptor::isValid() const {
        return this->it_->isValid();
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::className() const -> std::string {
        return "iterAdaptor";
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className();
        ss << "(" << *this->it_ << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

    template<typename TYPE>
    original::iterable<TYPE>::iterAdaptor::~iterAdaptor() {
        delete this->it_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() -> iterAdaptor {
        return iterAdaptor(this->begins());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() -> iterAdaptor {
        auto* it = this->ends();
        it->next();
        return iterAdaptor(it);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() const -> iterAdaptor {
        return iterAdaptor(this->begins());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() const -> iterAdaptor {
        auto* it = this->ends();
        it->next();
        return iterAdaptor(it);
    }

    template <typename TYPE>
    template<typename Callback>
    auto original::iterable<TYPE>::forEach(Callback operation) -> void
    {
        callBackChecker<Callback, void, TYPE&>::check();
        for (auto* it = this->begins(); it->isValid(); it->next()) {
            operation(it->get());
        }
    }

#endif //ITERABLE_H
