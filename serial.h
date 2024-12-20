#ifndef SERIAL_H
#define SERIAL_H

#include <container.h>

namespace original{
    template <typename TYPE>
    class serial : public container<TYPE>{
    protected:
        [[nodiscard]] bool indexOutOfBound(int64_t index) const;
        [[nodiscard]] int64_t parseNegIndex(int64_t index) const;
    public:
        void add(const TYPE& e) override;
        void clear() override;
        virtual TYPE get(int64_t index) const = 0;
        virtual TYPE getBegin() const;
        virtual TYPE getEnd() const;
        virtual TYPE operator[](int64_t index) const;
        virtual TYPE& operator[](int64_t index) = 0;
        virtual void set(int64_t index, const TYPE& e) = 0;
        virtual size_t indexOf(const TYPE& e) const = 0;
        bool contains(const TYPE& e) const override;
        virtual void push(int64_t index, const TYPE& e);
        virtual TYPE pop(int64_t index);
        virtual void pushBegin(const TYPE& e);
        virtual TYPE popBegin();
        virtual void pushEnd(const TYPE& e);
        virtual TYPE popEnd();
    };
}

    template<typename TYPE>
    auto original::serial<TYPE>::indexOutOfBound(const int64_t index) const -> bool
    {
        int64_t parsedIndex = this->parseNegIndex(index);
        return parsedIndex < 0 || parsedIndex >= this->size();
    }

    template<typename TYPE>
    auto original::serial<TYPE>::parseNegIndex(int64_t index) const -> int64_t {
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
    auto original::serial<TYPE>::operator[](const int64_t index) const -> TYPE
    {
        return this->get(index);
    }

    template <typename TYPE>
    auto original::serial<TYPE>::contains(const TYPE &e) const -> bool
    {
        return this->indexOf(e) != this->size();
    }

    template<typename TYPE>
    void original::serial<TYPE>::push(int64_t index, const TYPE &e) {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    TYPE original::serial<TYPE>::pop(int64_t index) {
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
