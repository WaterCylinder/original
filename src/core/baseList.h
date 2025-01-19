#ifndef BASELIST_H
#define BASELIST_H
#include "serial.h"

namespace original {
    template <typename TYPE>
    class baseList : public serial<TYPE>{
    public:
        virtual void add(const TYPE& e);
        virtual TYPE remove(const TYPE& e);
        virtual void clear();
        virtual void push(int64_t index, const TYPE& e) = 0;
        virtual TYPE pop(int64_t index) = 0;
        virtual void pushBegin(const TYPE& e) = 0;
        virtual TYPE popBegin() = 0;
        virtual void pushEnd(const TYPE& e) = 0;
        virtual TYPE popEnd() = 0;
    };
}

    template <typename TYPE>
    auto original::baseList<TYPE>::add(const TYPE &e) -> void
    {
        this->pushEnd(e);
    }

    template <typename TYPE>
    auto original::baseList<TYPE>::remove(const TYPE& e) -> TYPE
    {
        const int64_t index = this->indexOf(e);
        return this->pop(index);
    }

    template<typename TYPE>
    auto original::baseList<TYPE>::clear() -> void {
        while (!this->empty()) {
            this->popEnd();
        }
    }

#endif //BASELIST_H
