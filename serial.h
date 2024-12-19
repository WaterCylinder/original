#ifndef SERIAL_H
#define SERIAL_H

#include <container.h>

namespace original{
    template <typename TYPE>
    class serial : public container<TYPE>{
    protected:
        [[nodiscard]] bool indexOutOfBound(long long index) const;
        [[nodiscard]] long long parseNegIndex(long long index) const;
    public:
        void add(const TYPE& e) override;
        void clear() override;
        virtual TYPE get(long long index) const = 0;
        virtual TYPE getBegin() const;
        virtual TYPE getEnd() const;
        virtual TYPE operator[](long long index) const;
        virtual TYPE& operator[](long long index) = 0;
        virtual void set(long long index, const TYPE& e) = 0;
        virtual size_t indexOf(const TYPE& e) const = 0;
        bool contains(const TYPE& e) const override;
        virtual void push(long long index, const TYPE& e);
        virtual TYPE pop(long long index);
        virtual void pushBegin(const TYPE& e);
        virtual TYPE popBegin();
        virtual void pushEnd(const TYPE& e);
        virtual TYPE popEnd();
    };
}

    template<typename TYPE>
    auto original::serial<TYPE>::indexOutOfBound(long long index) const -> bool
    {
        int parsedIndex = this->parseNegIndex(index);
        return parsedIndex < 0 || parsedIndex >= this->size();
    }

    template<typename TYPE>
    auto original::serial<TYPE>::parseNegIndex(long long index) const -> long long
        {
        return index >= 0 ? index : this->size() + index;
    }

    template <typename TYPE>
    auto original::serial<TYPE>::add(const TYPE &e) -> void
    {
        this->pushEnd(e);
    }

    template<typename TYPE>
    auto original::serial<TYPE>::clear() -> void {
        while (!this->empty()) {
            this->popEnd();
        }
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
    auto original::serial<TYPE>::operator[](const long long index) const -> TYPE
    {
        return this->get(index);
    }

    template <typename TYPE>
    auto original::serial<TYPE>::contains(const TYPE &e) const -> bool
    {
        return this->indexOf(e) != this->size();
    }

    template<typename TYPE>
    void original::serial<TYPE>::push(long long index, const TYPE &e) {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    TYPE original::serial<TYPE>::pop(long long index) {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    void original::serial<TYPE>::pushBegin(const TYPE &e) {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    TYPE original::serial<TYPE>::popBegin() {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    void original::serial<TYPE>::pushEnd(const TYPE &e) {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    TYPE original::serial<TYPE>::popEnd() {
        throw unSupportedMethodError();
    }

#endif //SERIAL_H
