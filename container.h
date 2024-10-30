#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once

#include "iterable.h"
#include <string>
#include "config.h"

namespace original {

    template <typename TYPE>
    class container : public virtual iterable<TYPE>{
    protected:
        virtual bool indexOutOfBound(int index) = 0;
    public:
        virtual TYPE get(int index) = 0;
        virtual TYPE operator[](int index) = 0;
        virtual void set(int index, TYPE e) = 0;
        virtual ~container() = default;
    };

}

#endif //CONTAINER_H