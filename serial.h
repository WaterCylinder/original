#ifndef SERIAL_H
#define SERIAL_H

#include "container.h"

namespace original{
    template <typename TYPE>
    class serial : public container<TYPE>{
    protected:
        _GLIBCXX_NODISCARD bool indexOutOfBound(int index) const;
        _GLIBCXX_NODISCARD int negIndex(int index) const;
    public:
        virtual TYPE get(int index) = 0;
        virtual TYPE operator[](int index) = 0;
        virtual void set(int index, TYPE e) = 0;
        virtual size_t indexOf(TYPE e) = 0;
        bool contains(TYPE e) override;
        virtual void push(int index, TYPE e) = 0;
        virtual TYPE pop(int index) = 0;
        virtual void pushBegin(TYPE e) = 0;
        virtual TYPE popBegin() = 0;
        virtual void pushEnd(TYPE e) = 0;
        virtual TYPE popEnd() = 0;
    };
}

    template<typename TYPE>
    bool original::serial<TYPE>::indexOutOfBound(const int index) const
    {
        return this->negIndex(index) >= this->size();
    }

    template<typename TYPE>
    int original::serial<TYPE>::negIndex(int index) const
    {
        return index >= 0 ? index : this->size() + index;
    }

    template <typename TYPE>
    bool original::serial<TYPE>::contains(TYPE e)
    {
        return this->indexOf(e) != this->size();
    }

#endif //SERIAL_H
