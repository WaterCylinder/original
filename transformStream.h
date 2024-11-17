#ifndef TRANSFORMSTREAM_H
#define TRANSFORMSTREAM_H

#include "transform.h"
#include "chain.h"
#include <memory>

namespace original {

    template<typename TYPE>
    class transformStream {
        chain<std::shared_ptr<transform<TYPE>>> stream;

        void pushEnd(const transform<TYPE>& t);
    protected:
        explicit transformStream();

    public:
        void operator()(TYPE& t);
        ~transformStream() = default;
        transformStream& operator+(const transform<TYPE>& t);
        transformStream& operator+(const transformStream& ts);

        template<typename T>
        friend transformStream<T> operator+(const transform<T>& t1, const transform<T>& t2);
        template<typename T>
        friend transformStream<T> operator+(const transform<T>& t, const transformStream<T>& ots);
    };

    template<typename T>
    transformStream<T> operator+(const transform<T>& t1, const transform<T>& t2);
    template<typename T>
    transformStream<T> operator+(const transform<T>& t, const transformStream<T>& ots);
}

    template<typename TYPE>
    original::transformStream<TYPE>::transformStream() : stream() {}

    template<typename TYPE>
    auto original::transformStream<TYPE>::pushEnd(const transform<TYPE>& t) -> void
    {
        this->stream.pushEnd(std::shared_ptr<transform<TYPE>>(t.clone()));
    }

    template<typename TYPE>
    void original::transformStream<TYPE>::operator()(TYPE& t) {
        for (const auto& transform : this->stream) {
            (*transform)(t);
        }
    }

    template <typename TYPE>
    auto original::transformStream<TYPE>::operator+(const transform<TYPE>& t) -> transformStream&
    {
        this->pushEnd(t);
        return *this;
    }

    template <typename TYPE>
    auto original::transformStream<TYPE>::operator+(const transformStream& ts) -> transformStream&
    {
        for (const auto& transform: ts.stream)
        {
            this->pushEnd(*transform->clone());
        }
        return *this;
    }

    template<typename TYPE>
    auto original::operator+(const transform<TYPE>& t1, const transform<TYPE>& t2) -> transformStream<TYPE> {
        transformStream<TYPE> ts;
        ts + t1;
        ts + t2;
        return ts;
    }

    template <typename TYPE>
    auto original::operator+(const transform<TYPE>& t, const transformStream<TYPE>& ots) -> transformStream<TYPE>
    {
        transformStream<TYPE> ts;
        ts + t;
        ts + ots;
        return ts;
    }

#endif // TRANSFORMSTREAM_H
