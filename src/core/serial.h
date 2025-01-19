#ifndef SERIAL_H
#define SERIAL_H

#include "container.h"

namespace original{
    template <typename TYPE>
    class serial : public container<TYPE>{
    protected:
        [[nodiscard]] bool indexOutOfBound(int64_t index) const;
        [[nodiscard]] int64_t parseNegIndex(int64_t index) const;
    public:
        virtual TYPE get(int64_t index) const = 0;
        virtual TYPE getBegin() const;
        virtual TYPE getEnd() const;
        virtual TYPE operator[](int64_t index) const;
        virtual TYPE& operator[](int64_t index) = 0;
        virtual void set(int64_t index, const TYPE& e) = 0;
        virtual uint32_t indexOf(const TYPE& e) const = 0;
        bool contains(const TYPE& e) const override;
    };
}

    template<typename TYPE>
    auto original::serial<TYPE>::indexOutOfBound(const int64_t index) const -> bool
    {
        int64_t parsed_index = this->parseNegIndex(index);
        return parsed_index < 0 || parsed_index >= this->size();
    }

    template<typename TYPE>
    auto original::serial<TYPE>::parseNegIndex(int64_t index) const -> int64_t {
        return index >= 0 ? index : this->size() + index;
    }

    template<typename TYPE>
    auto original::serial<TYPE>::getBegin() const -> TYPE {
        return this->get(0);
    }

    template<typename TYPE>
    auto original::serial<TYPE>::getEnd() const -> TYPE {
        return this->get(-1);
    }

    template <typename TYPE>
    auto original::serial<TYPE>::operator[](const int64_t index) const -> TYPE
    {
        return this->get(index);
    }

    template <typename TYPE>
    auto original::serial<TYPE>::contains(const TYPE &e) const -> bool
    {
        return this->indexOf(e) != this->size();
    }

#endif //SERIAL_H
