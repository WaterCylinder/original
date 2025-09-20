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
            strongPtr<TYPE> result_;                      ///< Storage of asynchronous computation result
            mutable pCondition cond_{};                   ///< Condition variable for synchronization
            mutable pMutex mutex_{};                      ///< Mutex for thread safety
            std::exception_ptr e_{};                      ///< Exception pointer for error handling

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
            void wait() const;

            /**
             * @brief Waits for the result with a timeout
             * @param timeout Maximum time to wait
             * @return True if result is ready within timeout, false otherwise
             */
            bool waitFor(time::duration timeout) const;

            /**
             * @brief Retrieves the result value (blocks until ready)
             * @return The computed result
             * @throws std::exception if an exception was set
             */
            TYPE get();

            /**
             * @brief Peeks at the result value without consuming it (blocks until ready)
             * @return Const reference to the computed result
             * @throws std::exception if an exception was set
             */
            const TYPE& peek() const;

            /**
             * @brief Gets a strong pointer to the result value
             * @return Strong pointer to the result
             */
            strongPtr<TYPE> getPtr() const;

            /**
             * @brief Throws stored exception if present
             */
            void rethrowIfException() const;

            /**
             * @brief Gets the stored exception
             * @return Exception pointer (nullptr if no exception)
             */
            [[nodiscard]] std::exception_ptr exception() const;
        };

    public:
        /**
         * @class futureBase
         * @brief Abstract base interface for asynchronous consumers
         * @details Provides a common, type-erased interface for accessing asynchronous
         * results, regardless of their actual type or ownership semantics.
         *
         * Key methods include:
         * - `valid()`   : check if the consumer is valid
         * - `ready()`   : check if the result is ready
         * - `wait()`    : block until ready
         * - `exception()`: retrieve any stored exception
         */
        class futureBase {
        public:
            /**
             * @brief Checks if the future is valid (has an associated async wrapper)
             * @return True if valid, false otherwise
             */
            [[nodiscard]] virtual bool valid() const noexcept = 0;

            /**
             * @brief Waits until the result becomes ready
             */
            virtual void wait() const = 0;

            /**
             * @brief Waits for the result with a timeout
             * @param timeout Maximum time to wait
             * @return True if result is ready within timeout, false otherwise
             */
            virtual bool waitFor(time::duration timeout) const = 0;

            /**
             * @brief Gets the stored exception if computation failed
             * @return Exception pointer (nullptr if no exception)
             */
            [[nodiscard]] virtual std::exception_ptr exception() const noexcept = 0;

            /**
             * @brief Checks if the result is ready
             * @return True if result is available
             */
            [[nodiscard]] virtual bool ready() const = 0;

            virtual ~futureBase() = default;
        };

        // ==================== Future Class ====================

        template<typename TYPE>
        class sharedFuture;

        /**
         * @class future
         * @brief Represents a one-shot future result of an asynchronous computation
         * @tparam TYPE The result type of the computation
         * @details `future` allows retrieving the result exactly once.
         * After consumption, the value is moved out of storage.
         */
        template<typename TYPE>
        class future final : public futureBase {
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
             * @brief Checks if the future is valid (has an associated async wrapper)
             * @return True if valid, false otherwise
             */
            [[nodiscard]] bool valid() const noexcept override;

            /**
             * @brief Converts this future to a shared future
             * @return A shared future that shares the same async wrapper
             */
            sharedFuture<TYPE> share();

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
            [[nodiscard]] bool ready() const override;

            /**
             * @brief Gets the exception if computation failed
             * @return Exception pointer (nullptr if no exception)
             */
            [[nodiscard]] std::exception_ptr exception() const noexcept override;

            /**
             * @brief Waits until the result becomes ready
             */
            void wait() const override;

            /**
             * @brief Waits for the result with a timeout
             * @param timeout Maximum time to wait
             * @return True if result is ready within timeout, false otherwise
             */
            bool waitFor(time::duration timeout) const override;
        };

        /**
         * @class sharedFuture
         * @brief Represents a sharable asynchronous result
         * @tparam TYPE The result type of the computation
         * @details Unlike `future`, `sharedFuture` can be copied and safely stored in
         * multiple places. All copies refer to the same asynchronous result, making it
         * suitable for scenarios where results are needed by multiple consumers.
         */
        template<typename TYPE>
        class sharedFuture final : public futureBase, public hashable<sharedFuture<TYPE>> {
            strongPtr<asyncWrapper<TYPE>> awr_{};

            friend class async;
            explicit sharedFuture(strongPtr<asyncWrapper<TYPE>> awr);

        public:
            sharedFuture() = default;

            // Allow copying and moving
            sharedFuture(const sharedFuture&) = default;
            sharedFuture& operator=(const sharedFuture&) = default;
            sharedFuture(sharedFuture&&) = default;
            sharedFuture& operator=(sharedFuture&&) = default;

            /**
             * @brief Checks if the shared future is valid (has an associated async wrapper)
             * @return True if valid, false otherwise
             */
            [[nodiscard]] bool valid() const noexcept override;

            /**
             * @brief Gets the result value (blocks until ready)
             * @return The computed result
             * @throws std::exception if the computation threw an exception
             */
            TYPE result() const;

            /**
             * @brief Gets a strong pointer to the result value
             * @details Returns a strong reference pointer to the asynchronous result,
             * extending the lifetime of the referenced object and avoiding dangling references.
             * The pointer approach avoids unnecessary copying
             * @warning the returned pointer directly references the result stored within the async module.
             * Modifying the stored result through constCastTo or other means may cause unexpected behavior
             * and is strongly discouraged.
             * @return Strong pointer to the const result value
             * @throws sysError if the shared future is invalid or not ready
             * @throws std::exception if the computation threw an exception
             */
            strongPtr<const TYPE> strongPointer() const;

            /**
             * @brief Checks if the result is ready
             * @return True if result is available
             */
            [[nodiscard]] bool ready() const override;

            /**
             * @brief Gets the exception if computation failed
             * @return Exception pointer (nullptr if no exception)
             */
            [[nodiscard]] std::exception_ptr exception() const noexcept override;

            /**
             * @brief Equality comparison operator
             * @param other The other shared future to compare with
             * @return True if both shared futures refer to the same async wrapper
             */
            bool operator==(const sharedFuture& other) const noexcept;

            /**
             * @brief Inequality comparison operator
             * @param other The other shared future to compare with
             * @return True if shared futures refer to different async wrappers
             */
            bool operator!=(const sharedFuture& other) const noexcept;

            /**
             * @brief Waits until the result becomes ready
             */
            void wait() const override;

            /**
             * @brief Waits for the result with a timeout
             * @param timeout Maximum time to wait
             * @return True if result is ready within timeout, false otherwise
             */
            bool waitFor(time::duration timeout) const override;

            /**
             * @brief Generates a hash value for the shared future
             * @return Hash value based on the async wrapper pointer
             */
            [[nodiscard]] u_integer toHash() const noexcept override;

            /**
             * @brief Checks if two shared futures are equal
             * @param other The other shared future to compare with
             * @return True if both shared futures refer to the same async wrapper
             */
            bool equals(const sharedFuture& other) const noexcept override;

            ~sharedFuture() override = default;
        };

        // ==================== Promise Class ====================

        /**
         * @class promise
         * @brief Represents an asynchronous producer
         * @tparam TYPE The result type of the computation
         * @tparam Callback The type of the computation callback
         * @details Provides the means to set the result of an asynchronous computation
         */
        template<typename TYPE, typename Callback>
        class promise {
            std::function<TYPE()> c_{};                ///< The computation to execute
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
         * @brief Executes a callable asynchronously and returns a future for the result
         * @tparam Callback The type of the callable
         * @tparam Args The types of the arguments
         * @param c Callable to execute
         * @param args Arguments to forward to the callable
         * @return A future that will hold the result of the computation
         */
        template <typename Callback, typename... Args>
        static auto get(Callback&& c, Args&&... args) -> future<std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>>;
    };

    /**
     * @brief Pipe operator for chaining asynchronous computations
     * @details Applies a callback function to the result of a future once it is ready.
     * Provides a functional style syntax for continuation of asynchronous tasks.
     *
     * @tparam T Result type of the future
     * @tparam Callback Callable type, must accept T or T&& (depending on constness and value category)
     * @param f Source future containing the asynchronous result
     * @param c Callback function to process the result
     * @return A new future holding the result of the callback
     */
    template<typename T, typename Callback>
    auto operator|(async::future<T> f, Callback&& c);

    /**
     * @brief Pipe operator specialization for future<void>
     * @details Applies a callback function once the future completes.
     * Useful for chaining tasks that do not produce a value.
     *
     * @tparam Callback Callable type, must accept no arguments
     * @param f Source future<void> representing completion of an asynchronous task
     * @param c Callback function to execute after completion
     * @return A new future holding the result of the callback
     */
    template <typename Callback>
    auto operator|(async::future<void> f, Callback&& c);

    /**
     * @brief Pipe operator for chaining computations on sharedFuture
     * @details Applies a callback function to the result of a sharedFuture once it is ready.
     * Unlike future, sharedFuture can be copied and used in multiple places.
     *
     * @tparam T Result type of the sharedFuture
     * @tparam Callback Callable type, must accept T const& or T (depending on design)
     * @param sf Source sharedFuture containing the asynchronous result
     * @param c Callback function to process the result
     * @return A new future holding the result of the callback
     */
    template<typename T, typename Callback>
    auto operator|(async::sharedFuture<T> sf, Callback&& c);

    /**
     * @brief Pipe operator specialization for sharedFuture<void>
     * @details Applies a callback function once the sharedFuture completes.
     * Suitable for chaining multiple consumers of the same asynchronous task.
     *
     * @tparam Callback Callable type, must accept no arguments
     * @param sf Source sharedFuture<void> representing completion of an asynchronous task
     * @param c Callback function to execute after completion
     * @return A new future holding the result of the callback
     */
    template <typename Callback>
    auto operator|(async::sharedFuture<void> sf, Callback&& c);


    // ==================== Void Specializations ====================

    /**
     * @brief Specialization of asyncWrapper for void results
     */
    template <>
    class async::asyncWrapper<void> {
        atomic<bool> ready_{makeAtomic(false)};  ///< Atomic flag indicating completion
        alternative<void> result_;                    ///< Mark of asynchronous computation status
        mutable pCondition cond_{};                   ///< Condition variable for synchronization
        mutable pMutex mutex_{};                      ///< Mutex for thread safety
        std::exception_ptr e_{};                      ///< Exception pointer for error handling

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
        void wait() const;

        bool waitFor(time::duration timeout) const;

        /**
         * @brief Waits for completion and checks for exceptions
         * @throws std::exception if the computation threw an exception
         */
        void get();

        /**
         * @brief Checks for completion and exceptions without consuming the result
         * @throws std::exception if the computation threw an exception
         */
        void peek() const;

        /**
         * @brief Throws stored exception if present
         * @throws std::exception if the computation threw an exception
         */
        void rethrowIfException() const;

        /**
         * @brief Gets the stored exception
         * @return Exception pointer (nullptr if no exception)
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept;
    };

    /**
     * @brief Specialization of future for void results
     */
    template <>
    class async::future<void> final : public futureBase {
        strongPtr<asyncWrapper<void>> awr_{};  ///< Shared pointer to the async wrapper

        friend class async;
        explicit future(strongPtr<asyncWrapper<void>> awr);

    public:
        future() = default;

        // Disable copying
        future(const future&) = delete;
        future& operator=(const future&) = delete;

        // Allow moving
        future(future&&) = default;
        future& operator=(future&&) = default;

        /**
         * @brief Checks if the future is valid (has an associated async wrapper)
         * @return True if valid, false otherwise
         */
        [[nodiscard]] bool valid() const noexcept override;

        /**
         * @brief Converts this future to a shared future
         * @return A shared future that shares the same async wrapper
         */
        sharedFuture<void> share();

        /**
         * @brief Waits for completion and checks for exceptions
         * @throws std::exception if the computation threw an exception
         */
        void result();

        /**
         * @brief Checks if the computation is completed
         * @return True if completed
         */
        [[nodiscard]] bool ready() const override;

        /**
         * @brief Gets the exception if computation failed
         * @return Exception pointer (nullptr if no exception)
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept override;

        /**
         * @brief Waits until the computation completes
         */
        void wait() const override;

        bool waitFor(time::duration timeout) const override;
    };

    template <>
    class async::sharedFuture<void> final : public futureBase, public hashable<sharedFuture<void>> {
        strongPtr<asyncWrapper<void>> awr_{};

        friend class async;
        explicit sharedFuture(strongPtr<asyncWrapper<void>> awr);

    public:
        sharedFuture() = default;

        // Allow copying and moving
        sharedFuture(const sharedFuture&) = default;
        sharedFuture& operator=(const sharedFuture&) = default;
        sharedFuture(sharedFuture&&) = default;
        sharedFuture& operator=(sharedFuture&&) = default;

        /**
         * @brief Checks if the shared future is valid (has an associated async wrapper)
         * @return True if valid, false otherwise
         */
        [[nodiscard]] bool valid() const noexcept override;

        /**
         * @brief Waits for completion and checks for exceptions
         * @throws std::exception if the computation threw an exception
         */
        void result() const;

        /**
         * @brief Checks if the computation is completed
         * @return True if completed
         */
        [[nodiscard]] bool ready() const override;

        /**
         * @brief Gets the exception if computation failed
         * @return Exception pointer (nullptr if no exception)
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept override;

        /**
         * @brief Equality comparison operator
         * @param other The other shared future to compare with
         * @return True if both shared futures refer to the same async wrapper
         */
        bool operator==(const sharedFuture& other) const noexcept;

        /**
         * @brief Inequality comparison operator
         * @param other The other shared future to compare with
         * @return True if shared futures refer to different async wrappers
         */
        bool operator!=(const sharedFuture& other) const noexcept;

        /**
         * @brief Waits until the computation completes
         */
        void wait() const override;

        bool waitFor(time::duration timeout) const override;

        /**
         * @brief Generates a hash value for the shared future
         * @return Hash value based on the async wrapper pointer
         */
        [[nodiscard]] u_integer toHash() const noexcept override;

        /**
         * @brief Checks if two shared futures are equal
         * @param other The other shared future to compare with
         * @return True if both shared futures refer to the same async wrapper
         */
        [[nodiscard]] bool equals(const sharedFuture& other) const noexcept override;

        ~sharedFuture() override = default;
    };

    /**
     * @brief Specialization of promise for void results
     * @tparam Callback The type of the computation callback
     */
    template <typename Callback>
    class async::promise<void, Callback> {
        std::function<void()> c_{};                  ///< The computation to execute
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
        this->result_ = makeStrongPtr<TYPE>(std::move(v));
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
void original::async::asyncWrapper<TYPE>::wait() const
{
    uniqueLock lock{this->mutex_};
    this->cond_.wait(this->mutex_, [this]
    {
        return this->ready();
    });
}

template <typename TYPE>
bool original::async::asyncWrapper<TYPE>::waitFor(time::duration timeout) const
{
    uniqueLock lock{this->mutex_};
    return this->cond_.waitFor(this->mutex_, timeout, [this]
    {
        return this->ready();
    });
}

template <typename TYPE>
TYPE original::async::asyncWrapper<TYPE>::get()
{
    uniqueLock lock{this->mutex_};
    this->rethrowIfException();
    if (this->result_) {
        TYPE result = std::move(*this->result_);
        this->result_.reset();
        return result;
    }
    this->cond_.wait(this->mutex_, [this]{
        return this->ready();
    });

    this->rethrowIfException();

    TYPE result = std::move(*this->result_);
    this->result_.reset();
    return result;
}

template <typename TYPE>
const TYPE& original::async::asyncWrapper<TYPE>::peek() const
{
    uniqueLock lock{this->mutex_};
    this->rethrowIfException();
    if (this->result_) {
        return *this->result_;
    }
    this->cond_.wait(this->mutex_, [this]{
        return this->ready();
    });

    this->rethrowIfException();
    return *this->result_;
}

template <typename TYPE>
original::strongPtr<TYPE> original::async::asyncWrapper<TYPE>::getPtr() const
{
    this->rethrowIfException();
    return this->result_;
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
original::async::future<TYPE>::future(strongPtr<asyncWrapper<TYPE>> awr)
    : awr_(std::move(awr)) {}

template <typename TYPE>
bool original::async::future<TYPE>::valid() const noexcept
{
    return this->awr_ != nullptr;
}

template <typename TYPE>
original::async::sharedFuture<TYPE> original::async::future<TYPE>::share()
{
    return sharedFuture<TYPE>(std::move(this->awr_));
}


template <typename TYPE>
TYPE original::async::future<TYPE>::result()
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    return this->awr_->get();
}

template <typename TYPE>
bool original::async::future<TYPE>::ready() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    return this->awr_->ready();
}

template <typename TYPE>
std::exception_ptr original::async::future<TYPE>::exception() const noexcept
{
    if (!this->valid()) {
        return {};
    }
    return this->awr_->exception();
}

template <typename TYPE>
void original::async::future<TYPE>::wait() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    this->awr_->wait();
}

template <typename TYPE>
bool original::async::future<TYPE>::waitFor(time::duration timeout) const
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    return this->awr_->waitFor(timeout);
}

template <typename TYPE>
original::async::sharedFuture<TYPE>::sharedFuture(strongPtr<asyncWrapper<TYPE>> awr)
    : awr_(std::move(awr)) {}

template <typename TYPE>
bool original::async::sharedFuture<TYPE>::valid() const noexcept
{
    return this->awr_ != nullptr;
}

template <typename TYPE>
TYPE original::async::sharedFuture<TYPE>::result() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    return this->awr_->peek();
}

template <typename TYPE>
original::strongPtr<const TYPE> original::async::sharedFuture<TYPE>::strongPointer() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    if (!this->ready()) {
        throw sysError("Cannot get a strongPtr from a sharedFuture that is not ready");
    }
    return this->awr_->getPtr().template constCastTo<const TYPE>();
}

template <typename TYPE>
bool original::async::sharedFuture<TYPE>::ready() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    return this->awr_->ready();
}

template <typename TYPE>
std::exception_ptr original::async::sharedFuture<TYPE>::exception() const noexcept
{
    if (!this->valid()) {
        return {};
    }
    return this->awr_->exception();
}

template <typename TYPE>
bool original::async::sharedFuture<TYPE>::operator==(const sharedFuture& other) const noexcept
{
    return this->awr_ == other.awr_;
}

template <typename TYPE>
bool original::async::sharedFuture<TYPE>::operator!=(const sharedFuture& other) const noexcept
{
    return this->awr_ != other.awr_;
}

template <typename TYPE>
void original::async::sharedFuture<TYPE>::wait() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    this->awr_->wait();
}

template <typename TYPE>
bool original::async::sharedFuture<TYPE>::waitFor(time::duration timeout) const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    return this->awr_->waitFor(timeout);
}

template <typename TYPE>
original::u_integer original::async::sharedFuture<TYPE>::toHash() const noexcept
{
    return this->awr_.toHash();
}

template <typename TYPE>
bool original::async::sharedFuture<TYPE>::equals(const sharedFuture& other) const noexcept
{
    return *this == other;
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
        this->awr_->setValue(this->c_());
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
    -> future<std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...>>
{
    auto p = makePromise(std::forward<Callback>(c), std::forward<Args>(args)...);
    auto fut = p.getFuture();

    thread t{
        [p = std::move(p)]() mutable {
            p.run();
        },
        thread::AUTO_DETACH
    };

    return fut;
}

template <typename T, typename Callback>
auto original::operator|(async::future<T> f, Callback&& c)
{
    using ResultType = std::invoke_result_t<Callback, T>;
    strongPtr<async::future<T>> shared_f = makeStrongPtr<async::future<T>>(std::move(f));
    return async::get([shared_f, c = std::forward<Callback>(c)] mutable -> ResultType {
        if constexpr (!std::is_void_v<ResultType>) {
            return c(shared_f->result());
        } else {
            return c();
        }
    });
}

template <typename Callback>
auto original::operator|(async::future<void> f, Callback&& c)
{
    using ResultType = std::invoke_result_t<Callback>;
    strongPtr<async::future<void>> shared_f = makeStrongPtr<async::future<void>>(std::move(f));
    return async::get([shared_f, c = std::forward<Callback>(c)]() mutable -> ResultType {
        shared_f->result();
        if constexpr (!std::is_void_v<ResultType>) {
            return c();
        } else {
            c();
            return;
        }
    });
}

template <typename T, typename Callback>
auto original::operator|(async::sharedFuture<T> sf, Callback&& c)
{
    using ResultType = std::invoke_result_t<Callback, T>;
    return async::get([sf, c = std::forward<Callback>(c)] mutable -> ResultType {
        if constexpr (!std::is_void_v<ResultType>) {
            return c(sf.result());
        } else {
            sf.result();
            return c();
        }
    }).share();
}

template <typename Callback>
auto original::operator|(async::sharedFuture<void> sf, Callback&& c)
{
    using ResultType = std::invoke_result_t<Callback>;
    return async::get([sf, c = std::forward<Callback>(c)]() mutable -> ResultType {
        sf.result();
        if constexpr (!std::is_void_v<ResultType>) {
            return c();
        } else {
            c();
            return;
        }
    }).share();
}

inline void original::async::asyncWrapper<void>::setValue()
{
    {
        uniqueLock lock{this->mutex_};
        this->result_.set();
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

inline void original::async::asyncWrapper<void>::wait() const
{
    uniqueLock lock{this->mutex_};
    this->cond_.wait(this->mutex_, [this]
    {
        return this->ready();
    });
}

inline bool original::async::asyncWrapper<void>::waitFor(time::duration timeout) const
{
    uniqueLock lock{this->mutex_};
    return this->cond_.waitFor(this->mutex_, std::move(timeout), [this]
    {
       return this->ready();
    });
}

inline void original::async::asyncWrapper<void>::get()
{
    uniqueLock lock{this->mutex_};
    this->rethrowIfException();
    if (this->result_) {
        this->result_.reset();
        return;
    }
    this->cond_.wait(this->mutex_, [this] {
        return this->ready();
    });

    this->rethrowIfException();
    this->result_.reset();
}

inline void original::async::asyncWrapper<void>::peek() const
{
    uniqueLock lock{this->mutex_};
    this->rethrowIfException();
    if (this->result_) {
        return;
    }
    this->cond_.wait(this->mutex_, [this] {
        return this->ready();
    });

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
    uniqueLock lock{this->mutex_};
    return this->e_;
}

inline original::async::future<void>::future(strongPtr<asyncWrapper<void>> awr)
    : awr_(std::move(awr)) {}

inline bool original::async::future<void>::valid() const noexcept
{
    return this->awr_ != nullptr;
}

inline original::async::sharedFuture<void> original::async::future<void>::share()
{
    return sharedFuture(std::move(this->awr_));
}

inline void original::async::future<void>::result()
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    this->awr_->get();
}

inline bool original::async::future<void>::ready() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    return this->awr_->ready();
}

inline std::exception_ptr original::async::future<void>::exception() const noexcept
{
    if (!this->valid()) {
        return {};
    }
    return this->awr_->exception();
}

inline void original::async::future<void>::wait() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    this->awr_->wait();
}

inline bool original::async::future<void>::waitFor(const time::duration timeout) const
{
    if (!this->valid()) {
        throw sysError("Access an invalid future");
    }
    return this->awr_->waitFor(timeout);
}

inline original::async::sharedFuture<void>::sharedFuture(strongPtr<asyncWrapper<void>> awr)
    : awr_(std::move(awr)) {}

inline bool original::async::sharedFuture<void>::valid() const noexcept
{
    return this->awr_ != nullptr;
}

inline void original::async::sharedFuture<void>::result() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    this->awr_->peek();
}

inline bool original::async::sharedFuture<void>::ready() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    return this->awr_->ready();
}

inline std::exception_ptr original::async::sharedFuture<void>::exception() const noexcept
{
    if (!this->valid()) {
        return {};
    }
    return this->awr_->exception();
}

inline bool original::async::sharedFuture<void>::operator==(const sharedFuture& other) const noexcept
{
    return this->awr_ == other.awr_;
}

inline bool original::async::sharedFuture<void>::operator!=(const sharedFuture& other) const noexcept
{
    return this->awr_ != other.awr_;
}


inline void original::async::sharedFuture<void>::wait() const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    this->awr_->wait();
}

inline bool original::async::sharedFuture<void>::waitFor(const time::duration timeout) const
{
    if (!this->valid()) {
        throw sysError("Access an invalid sharedFuture");
    }
    return this->awr_->waitFor(timeout);
}

inline original::u_integer original::async::sharedFuture<void>::toHash() const noexcept
{
    return this->awr_.toHash();
}

inline bool original::async::sharedFuture<void>::equals(const sharedFuture& other) const noexcept
{
    return *this == other;
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