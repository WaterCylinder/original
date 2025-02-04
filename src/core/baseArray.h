#ifndef BASEARRAY_H
#define BASEARRAY_H
#include "serial.h"

namespace original{
    template <typename TYPE>
    class baseArray : public serial<TYPE> {
        protected:
            baseArray() = default;
            ~baseArray() override = default;
    };
}

#endif //BASEARRAY_H
