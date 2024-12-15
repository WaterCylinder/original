#ifndef SINGLEDIRECTIONITERATOR_H
#define SINGLEDIRECTIONITERATOR_H

namespace original
{
    template<typename TYPE>
    class singleDirectionIterator<TYPE> : public Iterator<TYPE>
    {
        protected:
            mutable wrapper<TYPE>* _ptr;

            explicit singleDirectionIterator(wrapper<TYPE>* ptr);
        public:
            singleDirectionIterator(const singleDirectionIterator& other);
            singleDirectionIterator& operator=(const singleDirectionIterator& other);
            _GLIBCXX_NODISCARD bool hasNext() const override;
            _GLIBCXX_NODISCARD bool hasPrev() const override = delete;
            bool atPrev(const Iterator<TYPE>* other) const override;
            bool atNext(const Iterator<TYPE>* other) const override;
            void next() const override;
            void prev() const override = delete;
            std::unique_ptr<singleDirectionIterator> getNext() override;
            std::unique_ptr<singleDirectionIterator> getPrev() override = delete;
            TYPE& get() override;
            const TYPE& get() const override;
            void set(const TYPE& data) override;
            _GLIBCXX_NODISCARD bool isValid() const override;
            _GLIBCXX_NODISCARD std::string className() const override;
    };
}

    template <typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(wrapper<TYPE>* ptr)
        : _ptr(ptr) {}

    template <typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(
        const singleDirectionIterator& other)
        : _ptr(nullptr)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::operator=(
        const singleDirectionIterator& other) -> singleDirectionIterator&
    {
        if (this != &other) {
            return *this;
        }
        this->_ptr = other._ptr;
        return *this;
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::hasNext() const -> bool
    {
        return this->isValid();
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::atPrev(const Iterator<TYPE>* other) const -> bool
    {
        if (!this->isValid()) throw nullPointerError();
        const auto other_it = static_cast<const singleDirectionIterator>(*other);
        return this->_ptr->getPNext() == other_it._ptr;
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::atNext(const Iterator<TYPE>* other) const -> bool
    {
        if (!this->isValid()) throw nullPointerError();
        const auto other_it = static_cast<const singleDirectionIterator>(*other);
        return other_it._ptr->getPNext() == this->_ptr;
    }

    template <typename TYPE>
    void original::singleDirectionIterator<TYPE>::next() const
    {
        if (!this->isValid()) throw nullPointerError();
        this->_ptr = this->_ptr->getPNext();
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::getNext() -> std::unique_ptr<singleDirectionIterator>
    {
        return std::make_unique<singleDirectionIterator>(
            singleDirectionIterator(this->_ptr->getPNext()));
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::get() -> TYPE&
    {
        if (!this->isValid()) throw nullPointerError();
        return this->_ptr->getVal();
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::get() const -> const TYPE&
    {
        if (!this->isValid()) throw nullPointerError();
        return this->_ptr->getVal();
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::set(const TYPE& data) -> void
    {
        if (!this->isValid()) throw nullPointerError();
        *this->_ptr = data;
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::isValid() const -> bool
    {
        return this->_ptr != nullptr;
    }

    template <typename TYPE>
    auto original::singleDirectionIterator<TYPE>::className() const -> std::string
    {
        return "singleDirectionIterator";
    }

#endif //SINGLEDIRECTIONITERATOR_H
