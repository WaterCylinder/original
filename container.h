#ifndef CONTAINER_H
#define CONTAINER_H
#include "iterable.h"
#include <string>
#include "config.h"

namespace original {

    template <typename TYPE>
    class container {
    public:
        virtual iterable<TYPE>* begins() = 0;
        virtual iterable<TYPE>* ends() = 0;
        virtual TYPE get(int index) = 0;
        virtual TYPE operator[](int index) = 0;
        virtual void set(int index, TYPE e) = 0;
        _GLIBCXX_NODISCARD virtual std::string toString(bool enter) = 0;
        virtual ~container() = default;
    };

}

#endif //CONTAINER_H