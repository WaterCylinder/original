#ifndef ORIGINAL_ASYNC_H
#define ORIGINAL_ASYNC_H

#include "atomic.h"
#include "condition.h"
#include "optional.h"
#include "thread.h"
#include "refCntPtr.h"
#include "types.h"
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
            explicit asyncWrapper() = default;

            template<typename Callback, typename... Args>
            explicit asyncWrapper(Callback&& c, Args&&... args);

            [[nodiscard]] bool ready() const;

            [[nodiscard]] bool available() const;

            void rethrowIfException() const;

            void wait();

            TYPE get();
        };

    public:
        class futureBase {
        public:
            virtual ~futureBase() = default;
            [[nodiscard]] virtual bool ready() const = 0;
            virtual void wait() const = 0;
            virtual void rethrowIfException() const = 0;
        };

        template<typename F_TYPE>
        class future final : futureBase {
            mutable strongPtr<asyncWrapper<F_TYPE>> awr_;

            friend class async;

            template<typename Callback, typename... Args>
            explicit future(Callback&& c, Args&&... args);

            template<typename ParentReturn, typename Callback>
            explicit future(strongPtr<asyncWrapper<ParentReturn>>& parent, Callback&& next);

            bool ready() const override;

            void wait() const override;

            void rethrowIfException() const override;
        public:
            using ReturnType = F_TYPE;

            future(const future&) = delete;

            future& operator=(const future&) = delete;

            future(future&&) = default;

            future& operator=(future&&) = default;

            F_TYPE result() const;

            template <typename Callback>
            auto then(Callback&& next);
        };

        template<typename P_TYPE, typename Callback>
        class promise {
            std::function<Callback> c;

        public:
            explicit promise();

            template<typename F>
            explicit promise(F f);

            template<typename... Args>
            future<P_TYPE> getFuture(Args&&... args) const;

            template<typename... Args>
            P_TYPE operator()(Args&&... args) const;
        };

        template<typename Callback>
        static auto makePromise(Callback&& c);

        template <typename Callback, typename... Args>
        static auto get(Callback&& c, Args&&... args) -> std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>;
    };

    template<>
    class async::asyncWrapper<void> {
        atomic<bool> ready_;
        alternative<void> alter_;
        pCondition cond_;
        pMutex mutex_;
        std::exception_ptr e_;

    public:
        explicit asyncWrapper();

        template<typename Callback, typename... Args>
        explicit asyncWrapper(Callback&& c, Args&&... args);

        [[nodiscard]] bool ready() const;

        [[nodiscard]] bool available() const;

        void rethrowIfException() const;

        void wait();

        void get();
    };

    template<>
    class async::future<void> final : futureBase {
        mutable strongPtr<asyncWrapper<void>> awr_;

        friend class async;

        template<typename Callback, typename... Args>
        explicit future(Callback c, Args... args);

        template<typename ParentType, typename Callback>
        explicit future(strongPtr<asyncWrapper<ParentType>>& parent, Callback&& next);

        bool ready() const override;

        void wait() const override;

        void rethrowIfException() const override;
    public:
        future(const future&) = delete;

        future& operator=(const future&) = delete;

        future(future&&) = default;

        future& operator=(future&&) = default;

        void result() const;

        template <typename Callback>
        auto then(Callback&& next);
    };
}


template <typename TYPE>
template <typename Callback, typename ... Args>
original::async::asyncWrapper<TYPE>::asyncWrapper(Callback&& c, Args&&... args) : ready_(makeAtomic(false))
{
    thread t{
        [this, c = std::forward<Callback>(c), ...args = std::forward<Args>(args)]() mutable
        {
            uniqueLock lock{this->mutex_};
            try
            {
                this->alter_.set(c(std::forward<Args>(args)...));
            }
            catch (...)
            {
                this->e_ = std::current_exception();
            }
            this->ready_.store(true);
            this->cond_.notify();
        },
        thread::AUTO_DETACH
    };
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::ready() const
{
    return this->ready_.load();
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::available() const
{
    return this->ready() && this->alter_.hasValue();
}

template <typename TYPE>
void original::async::asyncWrapper<TYPE>::rethrowIfException() const
{
    if (this->e_) {
        std::rethrow_exception(this->e_);
    }
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
original::async::future<F_TYPE>::future(Callback&& c, Args&&... args)
    : awr_(makeStrongPtr<asyncWrapper<F_TYPE>>(std::forward<Callback>(c), std::forward<Args>(args)...)) {}

template <typename F_TYPE>
template<typename ParentReturn, typename Callback>
original::async::future<F_TYPE>::future(strongPtr<asyncWrapper<ParentReturn>>& parent, Callback&& next) {
    this->awr_ = makeStrongPtr<asyncWrapper<F_TYPE>>(
        [parent, next = std::forward<Callback>(next)]() mutable -> F_TYPE {
            if (!parent->ready()) {
                parent->wait();
            }
            parent->rethrowIfException();

            if constexpr (!std::is_void_v<ParentReturn>) {
                if (parent->available()) {
                    auto v = parent->get();
                    return next(std::move(v));
                }
                throw valueError("Original value not available");
            } else {
                parent->get();
                if constexpr (std::is_void_v<F_TYPE>) {
                    next();
                    return;
                } else {
                    return next();
                }
            }
        }
    );
}

template <typename F_TYPE>
bool original::async::future<F_TYPE>::ready() const
{
    return this->awr_->ready();
}

template <typename F_TYPE>
void original::async::future<F_TYPE>::wait() const
{
    this->awr_->wait();
}

template <typename F_TYPE>
void original::async::future<F_TYPE>::rethrowIfException() const
{
    this->awr_->rethrowIfException();
}

template <typename F_TYPE>
F_TYPE original::async::future<F_TYPE>::result() const
{
    if (!this->awr_->ready()) {
        this->awr_->wait();
    }

    this->awr_->rethrowIfException();

    if (this->awr_->available()) {
        return this->awr_->get();
    }

    throw valueError("Get an unavailable value");
}

template <typename F_TYPE>
template <typename Callback>
auto original::async::future<F_TYPE>::then(Callback&& next)
{
    using RETURN = std::invoke_result_t<Callback, F_TYPE>;
    return future<RETURN>(this->awr_, std::forward<Callback>(next));
}

template <typename P_TYPE, typename Callback>
original::async::promise<P_TYPE, Callback>::promise()
    : c([] {}) {}

template <typename P_TYPE, typename Callback>
template <typename F>
original::async::promise<P_TYPE, Callback>::promise(F f)
    : c(std::move(f)) {}

template <typename P_TYPE, typename Callback>
template <typename... Args>
original::async::future<P_TYPE>
original::async::promise<P_TYPE, Callback>::getFuture(Args&&... args) const
{
    return future<P_TYPE>{c, std::forward<Args>(args)...};
}

template <typename P_TYPE, typename Callback>
template <typename ... Args>
P_TYPE original::async::promise<P_TYPE, Callback>::operator()(Args&&... args) const
{
    return this->getFuture(std::forward<Args>(args)...).result();
}

template <typename Callback>
auto original::async::makePromise(Callback&& c)
{
    using Traits = functionTraits<std::decay_t<Callback>>;
    using Return = Traits::ReturnType;
    using Sig = Traits::Signature;
    return promise<Return, Sig>(std::forward<Callback>(c));
}

template <typename Callback, typename... Args>
auto original::async::get(Callback&& c, Args&&... args)
    -> std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>
{
    using ResultType = std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>;

    auto p = promise<ResultType, ResultType(Args...)>{std::forward<Callback>(c)};
    return p.getFuture(std::forward<Args>(args)...).result();
}

inline original::async::asyncWrapper<void>::asyncWrapper()
    : ready_(makeAtomic(false)) {}

template <typename Callback, typename ... Args>
original::async::asyncWrapper<void>::asyncWrapper(Callback&& c, Args&&... args) : ready_(makeAtomic(false))
{
    thread t{
        [this, c = std::forward<Callback>(c), ...args = std::forward<Args>(args)]() mutable
        {
            uniqueLock lock{this->mutex_};
            try
            {
                c(std::forward<Args>(args)...);
                this->alter_.set();
            }
            catch (...)
            {
                this->e_ = std::current_exception();
            }
            this->ready_.store(true);
            this->cond_.notify();
        },
        thread::AUTO_DETACH
    };
}

inline bool original::async::asyncWrapper<void>::ready() const
{
    return this->ready_.load();
}

inline bool original::async::asyncWrapper<void>::available() const
{
    return this->ready() && this->alter_.hasValue();
}

inline void original::async::asyncWrapper<void>::rethrowIfException() const
{
    if (this->e_) {
        std::rethrow_exception(this->e_);
    }
}

inline void original::async::asyncWrapper<void>::wait()
{
    this->cond_.wait(this->mutex_);
}

inline void original::async::asyncWrapper<void>::get()
{
    this->alter_.reset();
}

template <typename Callback, typename ... Args>
original::async::future<void>::future(Callback c, Args... args)
    : awr_(makeStrongPtr<asyncWrapper<void>>(std::move(c), std::forward<Args>(args)...)) {}

template<typename ParentType, typename Callback>
original::async::future<void>::future(strongPtr<asyncWrapper<ParentType>>& parent, Callback&& next)
{
    this->awr_ = makeStrongPtr<asyncWrapper<void>>(
        [parent, next = std::forward<Callback>(next)]() mutable {
            if constexpr (!std::is_void_v<ParentType>) {
                if (!parent->ready()) {
                    parent->wait();
                }
                parent->rethrowIfException();

                if (parent->available()) {
                    auto v = parent->get();
                    next(std::move(v));
                } else {
                    throw valueError("Parent future returned no value");
                }
            } else {
                if (!parent->ready()) {
                    parent->wait();
                }
                parent->rethrowIfException();
                parent->get();

                next();
            }
        }
    );
}

inline bool original::async::future<void>::ready() const
{
    return this->awr_->ready();
}

inline void original::async::future<void>::wait() const
{
    this->awr_->wait();
}

inline void original::async::future<void>::rethrowIfException() const
{
    this->awr_->rethrowIfException();
}

inline void original::async::future<void>::result() const
{
    if (!this->awr_->ready()) {
        this->awr_->wait();
    }

    this->awr_->rethrowIfException();

    if (this->awr_->available()) {
        this->awr_->get();
        return;
    }

    throw valueError("Not available");
}

template <typename Callback>
auto original::async::future<void>::then(Callback&& next)
{
    using RETURN = std::invoke_result_t<Callback>;
    return future<RETURN>(this->awr_, std::forward<Callback>(next));
}

#endif // ORIGINAL_ASYNC_H