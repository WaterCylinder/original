#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once

#include "config.h"

namespace original {

    template <typename TYPE>
    class container{
    public:
        _GLIBCXX_NODISCARD virtual size_t size() const = 0;
        virtual bool contains(TYPE e) const = 0 ;
        virtual void add(TYPE e) = 0;
        virtual ~container() = default;
    };

}

#endif //CONTAINER_H