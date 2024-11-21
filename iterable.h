#ifndef ITERABLE_H
#define ITERABLE_H

#include <functional>
#include <transform.h>

#include "iterator.h"

namespace original{
    template <typename TYPE>
    class iterable{
    public:
        virtual ~iterable() = default;
        virtual iterator<TYPE>* begins() const = 0;
        virtual iterator<TYPE>* ends() const = 0;
        iterator<TYPE> begin();
        iterator<TYPE> end();
        iterator<TYPE> begin() const;
        iterator<TYPE> end() const;

        template<typename Callback = std::function<void(TYPE&)>>
        void forEach(Callback operation = transform<TYPE>());
    };
}

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() -> iterator<TYPE>
    {
        return *this->begins();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() -> iterator<TYPE>
    {
        return *this->ends()->getNext();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() const -> iterator<TYPE>
    {
        return *this->begins();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() const -> iterator<TYPE>
    {
        return *this->ends()->getNext();
    }

    template <typename TYPE>
    template<typename Callback>
    auto original::iterable<TYPE>::forEach(Callback operation) -> void
    {
        callBackChecker<Callback, void, TYPE&>::check();
        for (auto* it = this->begins(); it != nullptr && !it->isNull(); it->next()) {
            operation(it->get());
        }
    }

#endif //ITERABLE_H
