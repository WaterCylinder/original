#ifndef ORIGINAL_ASYNC_H
#define ORIGINAL_ASYNC_H

#include "atomic.h"
#include "condition.h"
#include "optional.h"
#include "refCntPtr.h"
#include "thread.h"
#include <exception>
#include <functional>
#include <utility>

namespace original {
    class async {
        template<typename TYPE>
        class asyncWrapper {
            atomic<bool> ready_;
            alternative<TYPE> alter_;
            pCondition cond_;
            pMutex mutex_;
            std::exception_ptr e_;

        public:
            asyncWrapper();

            void setValue(TYPE&& v);

            void setException(std::exception_ptr e);

            [[nodiscard]] bool ready() const;

            void wait();

            TYPE get();

            void rethrowIfException() const;

            [[nodiscard]] std::exception_ptr exception() const;

            [[nodiscard]] bool available() const;
        };

    public:
        template<typename TYPE>
        class future {
            strongPtr<asyncWrapper<TYPE>> awr_;

            friend class async;
            explicit future(strongPtr<asyncWrapper<TYPE>> awr);
        public:
            future() = default;

            future(const future&) = delete;

            future& operator=(const future&) = delete;

            future(future&&) = default;

            future& operator=(future&&) = default;

            TYPE result();

            [[nodiscard]] bool ready() const;

            [[nodiscard]] std::exception_ptr exception() const noexcept;

            void wait();
        };

        template<typename TYPE, typename Callback>
        class promise {
            std::function<TYPE()> c_;
            strongPtr<asyncWrapper<TYPE>> awr_;

        public:
            promise() = default;

            explicit promise(Callback&& c);

            future<TYPE> getFuture();

            void run();
        };

        template <typename Callback, typename... Args>
        static auto makePromise(Callback&& c, Args&&... args);

        template <typename Callback, typename... Args>
        static auto get(Callback&& c, Args&&... args);
    };

    template <>
    class async::asyncWrapper<void> {
        atomic<bool> ready_;
        alternative<void> alter_;
        pCondition cond_;
        pMutex mutex_;
        std::exception_ptr e_;

    public:
        asyncWrapper();

        void setValue();

        void setException(std::exception_ptr e);

        [[nodiscard]] bool ready() const;

        void wait();

        void get();

        void rethrowIfException() const;

        [[nodiscard]] std::exception_ptr exception() const noexcept;

        [[nodiscard]] bool available() const;
    };

    template <>
    class async::future<void> {
        strongPtr<asyncWrapper<void>> awr_;

        friend class async;
        explicit future(const strongPtr<asyncWrapper<void>>& awr) : awr_(std::move(awr)) {}

    public:
        future() = default;

        future(const future&) = delete;
        future& operator=(const future&) = delete;
        future(future&&) = default;
        future& operator=(future&&) = default;

        void result();

        [[nodiscard]] bool ready() const;

        [[nodiscard]] std::exception_ptr exception() const noexcept;

        void wait();
    };

    template <typename Callback>
    class async::promise<void, Callback> {
        std::function<void()> c_;
        strongPtr<asyncWrapper<void>> awr_;

    public:
        promise() = default;

        explicit promise(Callback&& c);

        [[nodiscard]] future<void> getFuture() const;

        void run();
    };
} // namespace original

template <typename TYPE>
original::async::asyncWrapper<TYPE>::asyncWrapper()
    : ready_(makeAtomic(false)) {}

template <typename TYPE>
void original::async::asyncWrapper<TYPE>::setValue(TYPE&& v)
{
    {
        uniqueLock lock{this->mutex_};
        this->alter_.set(std::move(v));
        this->ready_.store(true);
    }
    this->cond_.notify();
}

template <typename TYPE>
void original::async::asyncWrapper<TYPE>::setException(std::exception_ptr e)
{
    {
        uniqueLock lock{this->mutex_};
        this->e_ = std::move(e);
        this->ready_.store(true);
    }
    this->cond_.notify();
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::ready() const
{
    return this->ready_.load();
}

template <typename TYPE>
void original::async::asyncWrapper<TYPE>::wait()
{
    uniqueLock lock{this->mutex_};
    this->cond_.wait(this->mutex_, [this]
    {
        return this->ready();
    });
}

template <typename TYPE>
TYPE original::async::asyncWrapper<TYPE>::get()
{
    this->wait();
    this->rethrowIfException();
    TYPE result = *this->alter_;
    this->alter_.reset();
    return result;
}

template <typename TYPE>
void original::async::asyncWrapper<TYPE>::rethrowIfException() const
{
    if (this->e_)
        std::rethrow_exception(this->e_);
}

template <typename TYPE>
std::exception_ptr
original::async::asyncWrapper<TYPE>::exception() const
{
    return this->e_;
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::available() const
{
    return this->ready() && this->alter_.hasValue();
}

template <typename TYPE>
original::async::future<TYPE>::future(strongPtr<asyncWrapper<TYPE>> awr)
    : awr_(awr) {}

template <typename TYPE>
TYPE original::async::future<TYPE>::result()
{
    return this->awr_->get();
}

template <typename TYPE>
bool original::async::future<TYPE>::ready() const
{
    return this->awr_->ready();
}

template <typename TYPE>
std::exception_ptr original::async::future<TYPE>::exception() const noexcept
{
    return this->awr_->exception();
}

template <typename TYPE>
void original::async::future<TYPE>::wait()
{
    this->awr_->wait();
}

template <typename TYPE, typename Callback>
original::async::promise<TYPE, Callback>::promise(Callback&& c)
    : c_(std::forward<Callback>(c)), awr_(makeStrongPtr<asyncWrapper<TYPE>>()) {}

template <typename TYPE, typename Callback>
original::async::future<TYPE>
original::async::promise<TYPE, Callback>::getFuture()
{
    return future<TYPE>(this->awr_);
}

template <typename TYPE, typename Callback>
void original::async::promise<TYPE, Callback>::run()
{
    try {
        if constexpr (!std::is_void_v<TYPE>) {
            this->awr_->setValue(this->c_());
        } else {
            this->c_();
            this->awr_->setValue({});
        }
    } catch (...) {
        this->awr_->setException(std::current_exception());
    }
}

template <typename Callback, typename... Args>
auto original::async::makePromise(Callback&& c, Args&&... args) {
    using Return = std::invoke_result_t<Callback, Args...>;
    auto bound = [c = std::forward<Callback>(c),
                  ... args = std::forward<Args>(args)]() mutable -> Return {
        return c(args...);
    };

    return promise<Return, decltype(bound)>{std::move(bound)};
}

template <typename Callback, typename ... Args>
auto original::async::get(Callback&& c, Args&&... args)
{
    using ResultType = std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>;
    auto task = [c = std::forward<Callback>(c), ... args = std::forward<Args>(args)]() mutable -> ResultType {
        return c(std::forward<Args>(args)...);
    };

    auto p = makePromise(std::move(task));
    auto fut = p.getFuture();

    thread t{
        [p = std::move(p)]() mutable {
            p.run();
        },
        thread::AUTO_DETACH
    };

    return fut.result();
}

inline original::async::asyncWrapper<void>::asyncWrapper()
    : ready_(makeAtomic(false)) {}

inline void original::async::asyncWrapper<void>::setValue()
{
    {
        uniqueLock lock{this->mutex_};
        this->alter_.set();
        this->ready_.store(true);
    }
    this->cond_.notify();
}

inline void original::async::asyncWrapper<void>::setException(std::exception_ptr e)
{
    {
        uniqueLock lock{this->mutex_};
        this->e_ = std::move(e);
        this->ready_.store(true);
    }
    this->cond_.notify();
}

inline bool original::async::asyncWrapper<void>::ready() const
{
    return this->ready_.load();
}

inline void original::async::asyncWrapper<void>::wait()
{
    uniqueLock lock{this->mutex_};
    this->cond_.wait(this->mutex_, [this]
    {
        return this->ready();
    });
}

inline void original::async::asyncWrapper<void>::get()
{
    this->wait();
    this->rethrowIfException();
}

inline void original::async::asyncWrapper<void>::rethrowIfException() const
{
    if (this->e_)
        std::rethrow_exception(this->e_);
}

inline std::exception_ptr
original::async::asyncWrapper<void>::exception() const noexcept
{
    return this->e_;
}

inline bool original::async::asyncWrapper<void>::available() const
{
    return this->ready() && this->alter_.hasValue();
}

inline void original::async::future<void>::result()
{
    this->awr_->get();
}

inline bool original::async::future<void>::ready() const
{
    return this->awr_->ready();
}

inline std::exception_ptr original::async::future<void>::exception() const noexcept
{
    return this->awr_->exception();
}

inline void original::async::future<void>::wait()
{
    this->awr_->wait();
}

template <typename Callback>
original::async::promise<void, Callback>::promise(Callback&& c)
    : c_(std::forward<Callback>(c)), awr_(makeStrongPtr<asyncWrapper<void>>()) {}

template <typename Callback>
original::async::future<void> original::async::promise<void, Callback>::getFuture() const
{
    return future<void>(this->awr_);
}

template <typename Callback>
void original::async::promise<void, Callback>::run()
{
    try {
        this->c_();
        this->awr_->setValue();
    } catch (...) {
        this->awr_->setException(std::current_exception());
    }
}

#endif // ORIGINAL_ASYNC_H