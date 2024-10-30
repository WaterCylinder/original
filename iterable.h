#ifndef ITERABLE_H
#define ITERABLE_H

#include "wrapper.h"
#include "iterator.h"

namespace original{
    template <typename TYPE>
    class iterable : public virtual original::printable{
    public:
        virtual iterator<TYPE>* begins() = 0;
        virtual iterator<TYPE>* ends() = 0;
    };
}

#endif //ITERABLE_H
