#ifndef ORIGINAL_ASYNC_H
#define ORIGINAL_ASYNC_H

#include "condition.h"
#include "optional.h"
#include "thread.h"
#include <exception>
#include <functional>
#include <utility>

namespace original {
    class async {
        template<typename TYPE>
        class asyncWrapper {
            bool ready_ = false;
            alternative<TYPE> alter_;
            pCondition cond_;
            pMutex mutex_;
            std::exception_ptr e_;

        public:
            explicit asyncWrapper() = default;

            template<typename Callback, typename... Args>
            explicit asyncWrapper(Callback c, Args... args);

            [[nodiscard]] bool ready() const;

            [[nodiscard]] bool available() const;

            void wait();

            TYPE get();
        };

    public:
        template<typename F_TYPE>
        class future {
            mutable asyncWrapper<F_TYPE> awr_;

            friend class async;

            template<typename Callback, typename... Args>
            explicit future(Callback c, Args... args);

        public:
            F_TYPE result() const;
        };

        template<typename P_TYPE, typename Callback>
        class promise {
            std::function<Callback> c;

        public:
            explicit promise();

            explicit promise(Callback c);

            template<typename... Args>
            future<P_TYPE> getFuture(Args... args) const;
        };

        template <typename Callback, typename... Args>
        static auto get(Callback&& c, Args&&... args) -> std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>;
    };
}


template <typename TYPE>
template <typename Callback, typename ... Args>
original::async::asyncWrapper<TYPE>::asyncWrapper(Callback c, Args... args)
{
    thread t{
        [this, c = std::move(c), ...args = std::move(args)]() mutable {
            uniqueLock lock{this->mutex_};
            try {
                this->alter_.set(c(std::move(args)...));
            }
            catch (...) {
                this->e_ = std::current_exception();
            }
            this->ready_ = true;
            this->cond_.notify();
        }, thread::AUTO_DETACH
    };

    if (this->e_) {
        std::rethrow_exception(this->e_);
    }
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::ready() const
{
    return this->ready_;
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::available() const
{
    return this->ready() && this->alter_.hasValue();
}

template <typename TYPE>
void original::async::asyncWrapper<TYPE>::wait()
{
    this->cond_.wait(this->mutex_);
}

template <typename TYPE>
TYPE original::async::asyncWrapper<TYPE>::get()
{
    TYPE result = *this->alter_;
    this->alter_.reset();
    return result;
}

template <typename F_TYPE>
template <typename Callback, typename ... Args>
original::async::future<F_TYPE>::future(Callback c, Args... args)
    : awr_(std::move(c), std::forward<Args>(args)...) {}

template <typename F_TYPE>
F_TYPE original::async::future<F_TYPE>::result() const
{
    if (!this->awr_.ready()) {
        this->awr_.wait();
    }

    if (this->awr_.available()) {
        return this->awr_.get();
    }

    throw valueError("Get an unavailable value");
}

template <typename P_TYPE, typename Callback>
original::async::promise<P_TYPE, Callback>::promise()
    : c([] {}) {}

template <typename P_TYPE, typename Callback>
original::async::promise<P_TYPE, Callback>::promise(Callback c)
    : c(std::move(c)) {}

template <typename P_TYPE, typename Callback>
template <typename... Args>
original::async::future<P_TYPE>
original::async::promise<P_TYPE, Callback>::getFuture(Args... args) const
{
    return future<P_TYPE>{c, std::forward<Args>(args)...};
}

template <typename Callback, typename... Args>
auto original::async::get(Callback&& c, Args&&... args)
    -> std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>
{
    using TYPE = std::invoke_result_t<std::decay_t<Callback>, Args...>;
    using FUNC_PTR = decltype(+std::declval<std::decay_t<Callback>>());
    using FUNC_SIG = std::remove_pointer_t<FUNC_PTR>;

    auto p = promise<TYPE, FUNC_SIG>(std::forward<Callback>(c));
    return p.getFuture(std::forward<Args>(args)...).result();
}

#endif // ORIGINAL_ASYNC_H
