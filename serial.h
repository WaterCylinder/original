#ifndef SERIAL_H
#define SERIAL_H

#include "container.h"

namespace original{
    template <typename TYPE>
    class serial : public container{
    protected:
        bool indexOutOfBound(int index);
        int negIndex(int index);
    public:
        virtual TYPE get(int index) = 0;
        virtual TYPE operator[](int index) = 0;
        virtual void set(int index, TYPE e) = 0;
        virtual void push(int index, TYPE e) = 0;
        virtual TYPE pop(int index) = 0;
        virtual void pushBegin(TYPE e) = 0;
        virtual TYPE popBegin() = 0;
        virtual void pushEnd(TYPE e) = 0;
        virtual TYPE popEnd() = 0;
    };
}

    template<typename TYPE>
    bool original::serial<TYPE>::indexOutOfBound(int index) {
        return this->negIndex(index) >= this->size();
    }

    template<typename TYPE>
    int original::serial<TYPE>::negIndex(int index) {
        return index >= 0 ? index : this->size() + index;
    }

#endif //SERIAL_H
