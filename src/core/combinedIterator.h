#ifndef COMBINEDITERATOR_H
#define COMBINEDITERATOR_H


namespace original {
    // todo
    template<typename E_TYPE, typename B_TYPE>
    class combinedIterator : public baseIterator<E_TYPE> {
        protected:
            mutable randomAccessIterator<B_TYPE>* base_;
            mutable iterator<E_TYPE>* cur_it_;
            mutable iterator<B_TYPE>* start_it_;

            combinedIterator(randomAccessIterator<B_TYPE>* base, iterator<E_TYPE>* elem_it);
            bool equalPtr(const iterator<E_TYPE> *other) const override;
            virtual void setElemItr() = 0;
            virtual iterator<E_TYPE>* convertElemItr() const = 0;
        public:
            combinedIterator(const combinedIterator& other);
            combinedIterator& operator=(const combinedIterator& other);
            [[nodiscard]] bool hasNext() const override;
            [[nodiscard]] bool hasPrev() const override;
            bool atPrev(const iterator<E_TYPE> *other) const override;
            bool atNext(const iterator<E_TYPE> *other) const override;
            void next() const override;
            void prev() const override;
            E_TYPE& get() override;
            TYPE get() const override;
            void set(const E_TYPE &data) override;
            [[nodiscard]] bool isValid() const override;
            ~combinedIterator() override;
    };

}

    template<typename E_TYPE, typename B_TYPE>
    original::combinedIterator<E_TYPE, B_TYPE>::combinedIterator(
        randomAccessIterator<B_TYPE> *base, iterator<E_TYPE> *elem_it)
        : base_(base), cur_it_(elem_it), start_it_(elem_it) {}

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::equalPtr(const iterator<E_TYPE> *other) const -> bool {
        auto* other_it = dynamic_cast<const combinedIterator*>(other);
        return other_it != nullptr && this->base_ == other->base_ && this->cur_it_ == other->cur_it_;
    }

    template<typename E_TYPE, typename B_TYPE>
    original::combinedIterator<E_TYPE, B_TYPE>::combinedIterator(const combinedIterator &other) : start_it_(nullptr) {
        this->operator=(other);
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::operator=(const combinedIterator &other) -> combinedIterator & {
        if (this == &other) return *this;
        base_ = other.base_;
        cur_it_ = other.cur_it_;
        start_it_ = other.start_it_;
        return *this;
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::next() const -> void {
        if (this->cur_it_.isValid()) {
            this->cur_it_.next();
        }else {
            this->base_->next();
            delete this->cur_it_;
            this.setElemItr();
        }
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::prev() const -> void {
        throw unSupportedMethodError();
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::get() -> E_TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return this->cur_it_->get();
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::get() const -> TYPE
    {
        if (!this->isValid()) throw outOfBoundError();
        return this->cur_it_->get();
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::set(const E_TYPE &data) -> void {
        if (!this->isValid()) throw outOfBoundError();
        this->cur_it_->set(data);
    }

    template<typename E_TYPE, typename B_TYPE>
    auto original::combinedIterator<E_TYPE, B_TYPE>::isValid() const -> bool {
        return this->base_->isValid();
    }

    template<typename E_TYPE, typename B_TYPE>
    original::combinedIterator<E_TYPE, B_TYPE>::~combinedIterator() {
        delete base_;
        delete cur_it_;
    }

#endif //COMBINEDITERATOR_H
