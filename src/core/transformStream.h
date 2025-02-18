#ifndef TRANSFORMSTREAM_H
#define TRANSFORMSTREAM_H

/**
 * @file transformStream.h
 * @brief Transform pipeline composition and execution
 * @details Defines a stream of transformations that can be sequentially applied to data.
 * Supports building transformation pipelines through operator chaining.
 */

#include "transform.h"
#include "chain.h"
#include <memory>

namespace original {

    /**
     * @class transformStream
     * @tparam TYPE Type of data to be transformed
     * @brief Ordered sequence of transformation operations
     * @details Manages a chain of transformation objects that will be applied in sequence.
     * Features:
     * - Composable through + operator
     * - Shared ownership of transformations
     * - Lazy evaluation model
     */
    template<typename TYPE>
    class transformStream {
        chain<std::shared_ptr<transform<TYPE>>> stream;  ///< Ordered transformation chain

        /**
         * @brief Appends a cloned transformation to the end
         * @param t Transformation to append
         * @details Creates a shared_ptr of cloned transformation object
         */
        void pushEnd(const transform<TYPE>& t);

    protected:
        /**
         * @brief Protected constructor for factory pattern
         * @note Use operator+ for stream creation
         */
        explicit transformStream();

    public:
        /**
         * @brief Applies all transformations sequentially
         * @param t Data to be transformed (modified in-place)
         */
        void operator()(TYPE& t);

        /**
         * @brief Virtual destructor for the transformStream class.
         */
        ~transformStream() = default;

        /**
         * @brief Appends single transformation to stream
         * @param t Transformation to add
         * @return Reference to modified stream
         */
        transformStream& operator+(const transform<TYPE>& t);

        /**
         * @brief Merges another transformation stream
         * @param ts Stream to concatenate
         * @return Reference to modified stream
         */
        transformStream& operator+(const transformStream& ts);

        // Friend operators for stream construction
        /**
         * @brief Creates new stream from two transformations
         * @relates transformStream
         */
        template<typename T>
        friend transformStream<T> operator+(const transform<T>& t1, const transform<T>& t2);

        /**
         * @brief Creates new stream from transformation and existing stream
         * @relates transformStream
         */
        template<typename T>
        friend transformStream<T> operator+(const transform<T>& t, const transformStream<T>& ots);
    };

    /**
     * @brief Creates new transformation stream from two transforms
     * @tparam T Data type
     * @param t1 First transformation
     * @param t2 Second transformation
     * @return New stream applying t1 then t2
     */
    template<typename T>
    transformStream<T> operator+(const transform<T>& t1, const transform<T>& t2);

    /**
     * @brief Creates new stream by prefixing existing stream
     * @tparam T Data type
     * @param t Transformation to prepend
     * @param ots Original stream
     * @return New stream applying t then ots's transformations
     */
    template <typename T>
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
