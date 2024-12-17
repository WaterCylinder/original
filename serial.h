#ifndef SERIAL_H
#define SERIAL_H

#include "container.h"

namespace original{
    template <typename TYPE>
    class serial : public container<TYPE>{
    protected:
        [[__nodiscard__]] bool indexOutOfBound(int index) const;
        [[__nodiscard__]] int parseNegIndex(int index) const;
    public:
        void add(TYPE e) override;
        void clear() override;
        virtual TYPE get(int index) const = 0;
        virtual TYPE operator[](int index) const;
        virtual TYPE& operator[](int index) = 0;
        virtual void set(int index, TYPE e) = 0;
        virtual size_t indexOf(TYPE e) const = 0;
        bool contains(TYPE e) const override;
        virtual void push(int index, TYPE e) = 0;
        virtual TYPE pop(int index) = 0;
        virtual void pushBegin(TYPE e) = 0;
        virtual TYPE popBegin() = 0;
        virtual void pushEnd(TYPE e) = 0;
        virtual TYPE popEnd() = 0;
    };
}

    template<typename TYPE>
    auto original::serial<TYPE>::indexOutOfBound(const int index) const -> bool
    {
        int parsedIndex = this->parseNegIndex(index);
        return parsedIndex < 0 || parsedIndex >= this->size();
    }

    template<typename TYPE>
    auto original::serial<TYPE>::parseNegIndex(int index) const -> int
    {
        return index >= 0 ? index : this->size() + index;
    }

    template <typename TYPE>
    auto original::serial<TYPE>::add(TYPE e) -> void
    {
        this->pushEnd(e);
    }

    template<typename TYPE>
    auto original::serial<TYPE>::clear() -> void {
        while (!this->empty()) {
            this->popEnd();
        }
    }

    template <typename TYPE>
    auto original::serial<TYPE>::operator[](const int index) const -> TYPE
    {
        return this->get(index);
    }

    template <typename TYPE>
    auto original::serial<TYPE>::contains(TYPE e) const -> bool
    {
        return this->indexOf(e) != this->size();
    }

#endif //SERIAL_H
