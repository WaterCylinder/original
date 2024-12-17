#ifndef SINGLEDIRECTIONITERATOR_H
#define SINGLEDIRECTIONITERATOR_H

#include <stepIterator.h>

namespace original {
    template<typename TYPE>
    class singleDirectionIterator : public stepIterator<TYPE> {
        protected:
            explicit singleDirectionIterator(TYPE *ptr);
        public:
            singleDirectionIterator(const singleDirectionIterator& other);
            singleDirectionIterator& operator=(const singleDirectionIterator& other);
            [[__nodiscard__]] bool hasPrev() const override = delete;
            void prev() const override = delete;
            std::unique_ptr<stepIterator<TYPE>> getPrev() override = delete;
    };
}

    template<typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(TYPE *ptr) : stepIterator<TYPE>(ptr) {}

    template<typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(const singleDirectionIterator &other)
        : stepIterator<TYPE>(nullptr) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::singleDirectionIterator<TYPE>::operator=(
        const singleDirectionIterator &other) -> singleDirectionIterator& {
        if (this == &other) {
            return *this;
        }
        stepIterator<TYPE>::operator=(other);
        return *this;
    }

#endif //SINGLEDIRECTIONITERATOR_H
