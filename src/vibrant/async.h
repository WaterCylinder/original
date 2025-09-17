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

    /**
     * @class async
     * @brief Asynchronous programming utilities with future/promise pattern
     * @details Provides a thread-safe implementation of the future/promise pattern
     * for asynchronous computation. Supports both value-returning and void functions.
     */
    class async {
        // ==================== Async Wrapper (Internal) ====================

        /**
         * @class asyncWrapper
         * @brief Internal wrapper for asynchronous result storage
         * @tparam TYPE The result type of the asynchronous computation
         * @details Manages the state and synchronization of asynchronous results
         */
        template<typename TYPE>
        class asyncWrapper {
            atomic<bool> ready_{makeAtomic(false)};  ///< Atomic flag indicating result readiness
            alternative<TYPE> alter_;                ///< Optional storage for the result value
            pCondition cond_{};                      ///< Condition variable for synchronization
            mutable pMutex mutex_{};                 ///< Mutex for thread safety
            std::exception_ptr e_{};                 ///< Exception pointer for error handling

        public:
            asyncWrapper();

            /**
             * @brief Sets the result value and marks as ready
             * @param v The result value to store
             */
            void setValue(TYPE&& v);

            /**
             * @brief Sets an exception and marks as ready
             * @param e Exception pointer to store
             */
            void setException(std::exception_ptr e);

            /**
             * @brief Checks if the result is ready
             * @return True if result is available (value or exception)
             */
            [[nodiscard]] bool ready() const;

            /**
             * @brief Waits until the result becomes ready
             */
            void wait();

            /**
             * @brief Retrieves the result value (blocks until ready)
             * @return The computed result
             * @throws std::exception if an exception was set
             */
            TYPE get();

            /**
             * @brief Throws stored exception if present
             */
            void rethrowIfException() const;

            /**
             * @brief Gets the stored exception
             * @return Exception pointer (nullptr if no exception)
             */
            [[nodiscard]] std::exception_ptr exception() const;

            /**
             * @brief Checks if a valid result is available
             * @return True if result is ready and contains a value (not exception)
             */
            [[nodiscard]] bool available() const;
        };

    public:
        // ==================== Future Class ====================

        /**
         * @class future
         * @brief Represents a future result of an asynchronous computation
         * @tparam TYPE The result type of the computation
         * @details Provides access to the result of an asynchronous operation.
         * The result can be retrieved only once.
         */
        template<typename TYPE>
        class future {
            strongPtr<asyncWrapper<TYPE>> awr_{};  ///< Shared pointer to the async wrapper

            friend class async;
            explicit future(strongPtr<asyncWrapper<TYPE>> awr);

        public:
            future() = default;

            // Disable copying
            future(const future&) = delete;
            future& operator=(const future&) = delete;

            // Allow moving
            future(future&&) = default;
            future& operator=(future&&) = default;

            /**
             * @brief Gets the result (blocks until ready)
             * @return The computed result
             * @throws std::exception if the computation threw an exception
             */
            TYPE result();

            /**
             * @brief Checks if the result is ready
             * @return True if result is available
             */
            [[nodiscard]] bool ready() const;

            /**
             * @brief Gets the exception if computation failed
             * @return Exception pointer (nullptr if no exception)
             */
            [[nodiscard]] std::exception_ptr exception() const noexcept;

            /**
             * @brief Waits until the result becomes ready
             */
            void wait();
        };

        // ==================== Promise Class ====================

        /**
         * @class promise
         * @brief Represents a promise of a future result
         * @tparam TYPE The result type of the computation
         * @tparam Callback The type of the computation callback
         * @details Provides the means to set the result of an asynchronous computation
         */
        template<typename TYPE, typename Callback>
        class promise {
            std::function<TYPE()> c_;                  ///< The computation to execute
            strongPtr<asyncWrapper<TYPE>> awr_{};      ///< Shared pointer to the async wrapper

        public:
            promise() = default;

            /**
             * @brief Constructs a promise with a computation callback
             * @param c Callback that will produce the result
             */
            explicit promise(Callback&& c);

            /**
             * @brief Gets the future associated with this promise
             * @return A future that will receive the result
             */
            future<TYPE> getFuture();

            /**
             * @brief Executes the computation and sets the result
             */
            void run();
        };

        // ==================== Static Factory Methods ====================

        /**
         * @brief Creates a promise from a callable and arguments
         * @tparam Callback The type of the callable
         * @tparam Args The types of the arguments
         * @param c Callable to execute asynchronously
         * @param args Arguments to forward to the callable
         * @return A promise object for the asynchronous computation
         */
        template <typename Callback, typename... Args>
        static auto makePromise(Callback&& c, Args&&... args);

        /**
         * @brief Executes a callable asynchronously and gets the result
         * @tparam Callback The type of the callable
         * @tparam Args The types of the arguments
         * @param c Callable to execute
         * @param args Arguments to forward to the callable
         * @return The result of the computation (blocks until ready)
         * @throws std::exception if the computation threw an exception
         */
        template <typename Callback, typename... Args>
        static auto get(Callback&& c, Args&&... args);
    };

    // ==================== Void Specializations ====================

    /**
     * @brief Specialization of asyncWrapper for void results
     */
    template <>
    class async::asyncWrapper<void> {
        atomic<bool> ready_{makeAtomic(false)};  ///< Atomic flag indicating completion
        alternative<void> alter_;                ///< Optional void marker
        pCondition cond_{};                      ///< Condition variable for synchronization
        mutable pMutex mutex_{};                 ///< Mutex for thread safety
        std::exception_ptr e_{};                 ///< Exception pointer for error handling

    public:
        asyncWrapper() = default;

        /**
         * @brief Marks the computation as completed successfully
         */
        void setValue();

        /**
         * @brief Sets an exception and marks as completed
         * @param e Exception pointer to store
         */
        void setException(std::exception_ptr e);

        /**
         * @brief Checks if the computation is completed
         * @return True if completed (successfully or with exception)
         */
        [[nodiscard]] bool ready() const;

        /**
         * @brief Waits until the computation completes
         */
        void wait();

        /**
         * @brief Waits for completion and checks for exceptions
         * @throws std::exception if the computation threw an exception
         */
        void get();

        /**
         * @brief Throws stored exception if present
         */
        void rethrowIfException() const;

        /**
         * @brief Gets the stored exception
         * @return Exception pointer (nullptr if no exception)
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept;

        /**
         * @brief Checks if the computation completed successfully
         * @return True if completed without exception
         */
        [[nodiscard]] bool available() const;
    };

    /**
     * @brief Specialization of future for void results
     */
    template <>
    class async::future<void> {
        strongPtr<asyncWrapper<void>> awr_{};  ///< Shared pointer to the async wrapper

        friend class async;
        explicit future(const strongPtr<asyncWrapper<void>>& awr) : awr_(std::move(awr)) {}

    public:
        future() = default;

        // Disable copying
        future(const future&) = delete;
        future& operator=(const future&) = delete;

        // Allow moving
        future(future&&) = default;
        future& operator=(future&&) = default;

        /**
         * @brief Waits for completion and checks for exceptions
         * @throws std::exception if the computation threw an exception
         */
        void result();

        /**
         * @brief Checks if the computation is completed
         * @return True if completed
         */
        [[nodiscard]] bool ready() const;

        /**
         * @brief Gets the exception if computation failed
         * @return Exception pointer (nullptr if no exception)
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept;

        /**
         * @brief Waits until the computation completes
         */
        void wait();
    };

    /**
     * @brief Specialization of promise for void results
     * @tparam Callback The type of the computation callback
     */
    template <typename Callback>
    class async::promise<void, Callback> {
        std::function<void()> c_;                  ///< The computation to execute
        strongPtr<asyncWrapper<void>> awr_{};      ///< Shared pointer to the async wrapper

    public:
        promise() = default;

        /**
         * @brief Constructs a promise with a computation callback
         * @param c Callback that will execute the computation
         */
        explicit promise(Callback&& c);

        /**
         * @brief Gets the future associated with this promise
         * @return A future that will receive the completion signal
         */
        [[nodiscard]] future<void> getFuture() const;

        /**
         * @brief Executes the computation and marks completion
         */
        void run();
    };
} // namespace original

template <typename TYPE>
original::async::asyncWrapper<TYPE>::asyncWrapper() = default;

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
    uniqueLock lock{this->mutex_};
    this->cond_.wait(this->mutex_, [this]{
        return this->ready();
    });

    if (this->e_) std::rethrow_exception(this->e_);

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
    uniqueLock lock{this->mutex_};
    return this->e_;
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::available() const
{
    uniqueLock lock{this->mutex_};
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
    };

    return fut.result();
}

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
    uniqueLock lock{this->mutex_};
    this->cond_.wait(this->mutex_, [this] {
        return this->ready();
    });

    if (this->e_) std::rethrow_exception(this->e_);
}

inline void original::async::asyncWrapper<void>::rethrowIfException() const
{
    if (this->e_)
        std::rethrow_exception(this->e_);
}

inline std::exception_ptr
original::async::asyncWrapper<void>::exception() const noexcept
{
    uniqueLock lock{this->mutex_};
    return this->e_;
}

inline bool original::async::asyncWrapper<void>::available() const
{
    uniqueLock lock{this->mutex_};
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