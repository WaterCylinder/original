#ifndef ITERABLE_H
#define ITERABLE_H

#include <functional>
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

        template<typename Callback>
        void forEach(Callback callback);
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
    auto original::iterable<TYPE>::forEach(Callback callback) -> void
    {
        original::callBackChecker<Callback, void, TYPE&>::check();
        for (auto* it = this->begins(); it != nullptr && !it->isNull(); it->next()) {
            callback(it->get());
        }
    }

#endif //ITERABLE_H
