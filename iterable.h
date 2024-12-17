#ifndef ITERABLE_H
#define ITERABLE_H

#include <error.h>
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

        iterAdaptor<TYPE> begin();
        iterAdaptor<TYPE> end();
        iterAdaptor<TYPE> begin() const;
        iterAdaptor<TYPE> end() const;

        template<typename Callback = std::function<void(TYPE&)>>
        void forEach(Callback operation = transform<TYPE>());
    };
}

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() -> iterAdaptor<TYPE> {
        return iterAdaptor(this->begins());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() -> iterAdaptor<TYPE> {
        auto* it = this->ends();
        it->next();
        return iterAdaptor(it);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() const -> iterAdaptor<TYPE> {
        return iterAdaptor(this->begins());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() const -> iterAdaptor<TYPE> {
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
