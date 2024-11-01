#ifndef ITERABLE_H
#define ITERABLE_H

#include <functional>
#include "iterator.h"

namespace original{
    template <typename TYPE>
    class iterable{
    public:
        virtual ~iterable() = default;
        virtual iterator<TYPE>* begins() = 0;
        virtual iterator<TYPE>* ends() = 0;
        iterator<TYPE> begin();
        iterator<TYPE> end();
        iterator<TYPE> begin() const;
        iterator<TYPE> end() const;
        void foreach(const std::function<void(TYPE&)>& callback);
    };
}

    template <typename TYPE>
    original::iterator<TYPE> original::iterable<TYPE>::begin(){
        return *this->begins();
    }

    template <typename TYPE>
    original::iterator<TYPE> original::iterable<TYPE>::end(){
        return *this->ends();
    }

    template <typename TYPE>
    original::iterator<TYPE> original::iterable<TYPE>::begin() const{
        return *this->begins();
    }

    template <typename TYPE>
    original::iterator<TYPE> original::iterable<TYPE>::end() const{
        return *this->ends();
    }

    template <typename TYPE>
    void original::iterable<TYPE>::foreach(const std::function<void(TYPE&)>& callback) {
        for (auto* it = this->begins(); it != nullptr && !it->isNull(); it->next()) {
            callback(it->get());
        }
    }

#endif //ITERABLE_H
