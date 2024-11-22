#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once

#include "config.h"

namespace original {

    template <typename TYPE>
    class container{
    public:
        _GLIBCXX_NODISCARD virtual size_t size() const = 0;
        _GLIBCXX_NODISCARD bool empty() const;
        virtual bool contains(TYPE e) const = 0 ;
        virtual void add(TYPE e) = 0;
        virtual void clear() = 0;
        virtual ~container() = default;
    };
}

    template <typename TYPE>
    bool original::container<TYPE>::empty() const {
        return this->size() == 0;
    }

#endif //CONTAINER_H