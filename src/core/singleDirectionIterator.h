#ifndef SINGLEDIRECTIONITERATOR_H
#define SINGLEDIRECTIONITERATOR_H

#include "stepIterator.h"

namespace original {
    template<typename TYPE>
    class singleDirectionIterator : public stepIterator<TYPE> {
        protected:
            explicit singleDirectionIterator(wrapper<TYPE>* ptr);
        public:
            singleDirectionIterator(const singleDirectionIterator& other);
            singleDirectionIterator& operator=(const singleDirectionIterator& other);
            singleDirectionIterator* clone() const override;
    };
}

    template<typename TYPE>
    original::singleDirectionIterator<TYPE>::singleDirectionIterator(wrapper<TYPE>* ptr) : stepIterator<TYPE>(ptr) {}

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

    template<typename TYPE>
    auto original::singleDirectionIterator<TYPE>::clone() const -> singleDirectionIterator* {
        return new singleDirectionIterator(*this);
    }

#endif //SINGLEDIRECTIONITERATOR_H
