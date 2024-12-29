#ifndef ITERABLE_H
#define ITERABLE_H

#include <error.h>
#include <limits>
#include <transform.h>

#include "iterator.h"

namespace original{
    template <typename TYPE>
    class iterable{
    public:
        class iterAdaptor final : public iterator<TYPE>{
            baseIterator<TYPE>* it_;
            explicit iterAdaptor(baseIterator<TYPE>* it);
        protected:
            bool equalPtr(const iterator<TYPE>* other) const override;
            iterAdaptor* clone() const override;
            iterAdaptor* getPrev() const override;
            iterAdaptor* getNext() const override;
        public:
            friend class iterable;
            iterAdaptor(const iterAdaptor& other);
            iterAdaptor& operator=(const iterAdaptor& other);
            const iterator<TYPE>& getIt() const;
            [[nodiscard]] bool hasNext() const override;
            [[nodiscard]] bool hasPrev() const override;
            bool atPrev(const iterator<TYPE>* other) const override;
            bool atNext(const iterator<TYPE>* other) const override;
            void next() const override;
            void prev() const override;
            void operator+=(int64_t steps) const override;
            void operator-=(int64_t steps) const override;
            int64_t operator-(const iterator<TYPE>& other) const override;
            TYPE& get() override;
            void set(const TYPE& data) override;
            TYPE get() const override;
            bool operator!=(const iterAdaptor& other) const;
            [[nodiscard]] bool isValid() const override;
            [[nodiscard]] std::string className() const override;
            [[nodiscard]] std::string toString(bool enter) const override;
            ~iterAdaptor() override;
        };

        virtual ~iterable() = default;
        virtual baseIterator<TYPE>* begins() const = 0;
        virtual baseIterator<TYPE>* ends() const = 0;

        iterAdaptor begin();
        iterAdaptor end();
        iterAdaptor begin() const;
        iterAdaptor end() const;

        iterAdaptor first();
        iterAdaptor last();
        iterAdaptor first() const;
        iterAdaptor last() const;

        template<typename Callback = transform<TYPE>>
        void forEach(Callback operation = Callback{});
    };
}

    template <typename TYPE>
    original::iterable<TYPE>::iterAdaptor::iterAdaptor(baseIterator<TYPE>* it) : it_(it) {}

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::equalPtr(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(other);
        return other_it != nullptr && this->it_->equal(other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::clone() const -> iterAdaptor*
    {
        return new iterAdaptor(*this);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::getPrev() const -> iterAdaptor*
    {
        auto* it = this->clone();
        it->next();
        return it;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::getNext() const -> iterAdaptor*
    {
        auto* it = this->clone();
        it->prev();
        return it;
    }

    template <typename TYPE>
    original::iterable<TYPE>::iterAdaptor::iterAdaptor(const iterAdaptor& other) : iterAdaptor(nullptr)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator=(const iterAdaptor& other) -> iterAdaptor&
    {
        if (this == &other) return *this;

        delete this->it_;
        this->it_ = other.it_->clone();
        return *this;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::getIt() const -> const iterator<TYPE>&
    {
        return *this->it_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::hasNext() const -> bool
    {
        return this->it_->hasNext();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::hasPrev() const -> bool
    {
        return this->it_->hasPrev();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::atPrev(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(other);
        return other_it != nullptr && this->it_->atPrev(other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::atNext(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(other);
        return other_it != nullptr && this->it_->atNext(other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::next() const -> void
    {
        this->it_->next();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::prev() const -> void
    {
        this->it_->prev();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator+=(int64_t steps) const -> void
    {
        this->it_->operator+=(steps);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator-=(int64_t steps) const -> void
    {
        this->it_->operator-=(steps);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator-(const iterator<TYPE>& other) const -> int64_t
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(&other);
        if (other_it == nullptr)
            return this > &other ?
                std::numeric_limits<int64_t>::max() :
                std::numeric_limits<int64_t>::min();
        return this->it_->operator-(*other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::get() -> TYPE&
    {
        return this->it_->get();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::set(const TYPE& data) -> void
    {
        this->it_->set(data);
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::get() const -> TYPE
    {
        return this->it_->getElem();
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
    auto original::iterable<TYPE>::end() const -> iterAdaptor{
        auto* it = this->ends();
        it->next();
        return iterAdaptor(it);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::first() -> iterAdaptor
    {
        return this->begin();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::last() -> iterAdaptor
    {
        return iterAdaptor(this->ends());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::first() const -> iterAdaptor
    {
        return this->begin();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::last() const -> iterAdaptor
    {
        return iterAdaptor(this->ends());
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
