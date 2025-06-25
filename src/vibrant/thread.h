#ifndef THREAD_H
#define THREAD_H

#include "error.h"
#include "functional"
#include "pthread.h"
#include "ownerPtr.h"


/**
 * @file thread.h
 * @brief Thread management utilities
 * @details Provides a layered threading abstraction with:
 * - Low-level POSIX thread wrapper (pThread)
 * - High-level RAII thread management (thread)
 * - Exception-safe thread operations
 * - Flexible join/detach policies
 *
 * Key Features:
 * - Three-layer thread abstraction:
 *   1. threadBase - Common interface and state management
 *   2. pThread - POSIX thread wrapper
 *   3. thread - High-level RAII wrapper
 * - Automatic resource management
 * - Move-only semantics
 * - Exception safety (basic guarantee)
 * - Configurable join/detach behavior
 *
 * Thread Safety:
 * - Individual thread objects are not thread-safe
 * - External synchronization required for shared access
 *
 * Exception Handling:
 * - Throws sysError for thread operation failures
 * - Destructors may throw if thread is joinable and not joined/detached
 *
 * @ingroup Threading
 * @see original::threadBase
 * @see original::pThread
 * @see original::thread
 */

namespace original {
    /**
     * @class threadBase
     * @brief Base class for thread implementations
     * @details Provides common thread functionality and interface
     *
     * Key Features:
     * - Manages thread joinable state
     * - Provides basic thread validity checks
     * - Non-copyable but movable
     *
     * @note This is an abstract base class and cannot be instantiated directly
     */
    class threadBase {
    protected:
        /**
         * @class threadData
         * @tparam Callback Type of the thread callback function
         * @brief Wrapper for thread execution data
         * @details Handles the storage and execution of thread callbacks,
         * including exception handling and resource management.
         */
        template<typename Callback>
        class threadData
        {
            Callback c;
        public:
            /**
             * @brief Construct thread data wrapper
             * @param c Callback to store
             * @post The callback is moved into the threadData object
             */
            explicit threadData(Callback c);

            /**
             * @brief Thread entry point wrapper
             * @param arg Pointer to threadData instance
             * @return Always nullptr
             * @throw sysError if callback throws an error
             * @note This static method serves as the bridge between C-style pthread
             *       callbacks and C++ callable objects.
             */
            static void* run(void* arg);
        };

        bool is_joinable; ///< Flag indicating if thread can be joined

        /**
         * @brief Check if thread is valid
         * @return true if thread is valid (has an associated execution context)
         * @note Pure virtual function to be implemented by derived classes
         */
        [[nodiscard]] virtual bool valid() const = 0;

        /**
         * @brief Get thread identifier
         * @return Unique identifier for the thread
         * @note Pure virtual function to be implemented by derived classes
         */
        [[nodiscard]] virtual ul_integer id() const = 0;

        /**
         * @brief Construct thread base
         * @param is_joinable Whether thread is joinable
         */
        explicit threadBase(bool is_joinable = false);
    public:
        /**
         * @brief Destructor
         * @note Terminates program if thread is joinable and not joined/detached
         */
        virtual ~threadBase();

        threadBase(const threadBase&) = delete; ///< Deleted copy constructor
        threadBase& operator=(const threadBase&) = delete; ///< Deleted copy assignment

        threadBase(threadBase&& other) noexcept = default; ///< Default move constructor
        threadBase& operator=(threadBase&& other) noexcept = default; ///< Default move assignment

        /**
         * @brief Check if thread is valid
         * @return true if thread is valid
         */
        explicit operator bool() const;

        /**
         * @brief Check if thread is not valid
         * @return true if thread is not valid
         */
        bool operator!() const;

        /**
         * @brief Check if thread is joinable
         * @return true if thread is joinable
         * @note A thread is joinable if it represents an active thread of execution
         */
        [[nodiscard]] bool joinable() const;

        /**
         * @brief Wait for thread to complete execution
         * @throw sysError if join operation fails
         * @note Blocks the calling thread until this thread completes
         */
        virtual void join() = 0;

        /**
         * @brief Detach thread from handle
         * @throw sysError if detach operation fails
         * @note Allows thread to execute independently, resources automatically
         *       released when thread completes
         */
        virtual void detach() = 0;
    };

    /**
     * @class pThread
     * @brief POSIX thread implementation
     * @details Wrapper around pthread with RAII semantics. Provides low-level
     *          thread management using POSIX threads API.
     *
     * @note This class is not thread-safe for concurrent operations on the same object
     */
    class pThread final : public threadBase {
        pthread_t handle; ///< Native thread handle

        /**
         * @brief Check if thread is valid
         * @return true if thread handle is valid
         */
        [[nodiscard]] bool valid() const override;
    public:
        /**
         * @brief Construct empty (invalid) thread
         * @post Creates a thread object not associated with any execution
         */
        explicit pThread();

        /**
         * @brief Construct and start POSIX thread
         * @tparam Callback Callback function type
         * @tparam ARGS Argument types for callback
         * @param c Callback function to execute in new thread
         * @param args Arguments to forward to callback
         * @throw sysError if thread creation fails
         * @post New thread starts executing the callback with provided arguments
         */
        template<typename Callback, typename... ARGS>
        explicit pThread(Callback c, ARGS&&... args);

        /**
         * @brief Move constructor
         * @param other Thread to move from
         * @post Source thread becomes invalid
         */
        pThread(pThread&& other) noexcept;

        /**
         * @brief Move assignment
         * @param other Thread to move from
         * @return Reference to this object
         * @post Source thread becomes invalid
         */
        pThread& operator=(pThread&& other) noexcept;

        /**
         * @brief Get thread identifier
         * @return Unique identifier for the thread
         */
        [[nodiscard]] ul_integer id() const override;

        /**
         * @brief Wait for thread to complete
         * @note Terminates program if join fails
         * @note Blocks until the thread completes execution
         */
        void join() override;

        /**
         * @brief Detach thread (allow it to run independently)
         * @note Terminates program if detach fails
         * @note After detach, the thread object no longer represents the thread
         */
        void detach() override;
    };

    /**
     * @class thread
     * @brief High-level thread wrapper
     * @details Manages thread lifetime with automatic join/detach. Provides
     *          RAII semantics for thread management with configurable join policy.
     *
     * Key Features:
     * - Wraps low-level pThread with automatic cleanup
     * - Configurable join policy (AUTO_JOIN or AUTO_DETACH)
     *
     * Join Policy:
     * - joinPolicy::AUTO_JOIN: join the thread in destructor
     * - joinPolicy::AUTO_DETACH: detach the thread in destructor
     *
     * Example usage:
     * @code
     * original::thread t([](){
     *     // thread work
     * }, original::thread::AUTO_DETACH);
     * @endcode
     *
     * @see original::pThread
     * @see original::thread::joinPolicy
     */
    class thread {
        pThread thread_; ///< Underlying thread implementation
        bool will_join;  ///< Join policy flag

        /**
         * @enum joinPolicy
         * @brief Defines thread cleanup policy on destruction
         * @details Controls whether a thread should be joined or detached automatically
         * when the thread object is destroyed.
         *
         * @var AUTO_JOIN
         * The thread is joined in the destructor (blocking cleanup).
         *
         * @var AUTO_DETACH
         * The thread is detached in the destructor (non-blocking cleanup).
         */
        enum class joinPolicy {
            AUTO_JOIN,   ///< Join the thread automatically on destruction
            AUTO_DETACH, ///< Detach the thread automatically on destruction
        };
    public:

        /// @brief Alias for joinPolicy::AUTO_JOIN
        static constexpr auto AUTO_JOIN = joinPolicy::AUTO_JOIN;

        /// @brief Alias for joinPolicy::AUTO_DETACH
        static constexpr auto AUTO_DETACH = joinPolicy::AUTO_DETACH;

        /**
         * @brief Construct empty thread
         * @post Creates an invalid thread object (not associated with any execution)
         */
        explicit thread();

        /**
         * @brief Construct and start thread with callback (AUTO_JOIN policy)
         * @tparam Callback Callable type
         * @tparam ARGS Argument types
         * @param c Callable to execute in thread
         * @param args Arguments forwarded to the callable
         * @post Thread starts and will be joined on destruction
         */
        template<typename Callback, typename... ARGS>
        explicit thread(Callback c, ARGS&&... args);

        /**
         * @brief Construct and start a thread with the given callback and join policy
         * @tparam Callback Type of the callable object
         * @tparam ARGS Types of arguments to pass to the callable
         * @param c Callable object to run in the new thread
         * @param policy Join policy (AUTO_JOIN or AUTO_DETACH)
         * @param args Arguments forwarded to the callable
         * @post Starts a new thread and applies the specified join policy
         *
         * @see joinPolicy
         */
        template<typename Callback, typename... ARGS>
        explicit thread(Callback c, joinPolicy policy, ARGS&&... args);

        /**
         * @brief Construct a thread from an existing pThread with a join policy
         * @param p_thread The POSIX thread wrapper to take ownership of
         * @param policy Join policy (AUTO_JOIN or AUTO_DETACH)
         * @post Takes ownership of the thread and applies the specified join policy
         */
        explicit thread(pThread p_thread, joinPolicy policy = AUTO_JOIN);

        thread(const thread&) = delete; ///< Deleted copy constructor
        thread& operator=(const thread&) = delete; ///< Deleted copy assignment

        /**
         * @brief Move constructor (defaults to AUTO_JOIN)
         * @param other Thread to move from
         * @post Ownership transferred; will join on destruction
         */
        thread(thread&& other) noexcept;

        /**
         * @brief Move constructor with specified join policy
         * @param other Thread to move from
         * @param policy Join policy (AUTO_JOIN or AUTO_DETACH)
         * @post Ownership transferred and join behavior follows policy
         */
        thread(thread&& other, joinPolicy policy) noexcept;

        /**
         * @brief Move assignment
         * @param other Thread to move from
         * @return Reference to this object
         * @post Ownership and join policy transferred
         */
        thread& operator=(thread&& other) noexcept;

        /**
         * @brief Get thread identifier
         * @return Unique identifier for the thread
         */
        [[nodiscard]] ul_integer id() const;

        /**
         * @brief Check if thread is joinable
         * @return true if thread is joinable
         */
        [[nodiscard]] bool joinable() const;

        /**
         * @brief Check if thread is valid
         * @return true if thread is valid
         */
        explicit operator bool() const;

        /**
         * @brief Check if thread is not valid
         * @return true if thread is not valid
         */
        bool operator!() const;

        /**
         * @brief Wait for thread to complete
         * @note Terminates program if join fails
         * @note Blocks until the thread completes execution
         */
        void join();

        /**
         * @brief Detach thread (allow it to run independently)
         * @note Terminates program if detach fails
         * @note After detach, the thread object no longer represents the thread
         */
        void detach();

        /**
         * @brief Destructor
         * @note Automatically joins or detaches based on will_join policy
         * @note Terminates program if join/detach operation fails
         */
        ~thread();
    };
}


template <typename Callback>
original::threadBase::threadData<Callback>::threadData(Callback c)
    : c(std::move(c)) {}

template <typename Callback>
void* original::threadBase::threadData<Callback>::run(void* arg)
{
    auto self = ownerPtr<threadData>(static_cast<threadData*>(arg));
    try {
        self->c();
    }catch (const error&) {
        throw sysError();
    }
    return nullptr;
}

inline original::threadBase::operator bool() const
{
    return this->valid();
}

inline bool original::threadBase::operator!() const
{
    return !this->valid();
}

inline bool original::threadBase::joinable() const {
    return this->is_joinable;
}

inline original::threadBase::threadBase(bool is_joinable)
    : is_joinable(is_joinable) {}

inline original::threadBase::~threadBase() {
    if (this->is_joinable) {
        std::terminate();
    }
}

inline original::pThread::pThread() : handle() {}

template<typename Callback, typename... ARGS>
original::pThread::pThread(Callback c, ARGS&&... args) : threadBase(true), handle()
{
    auto bound_lambda =
    [func = std::forward<Callback>(c), ...lambda_args = std::forward<ARGS>(args)]() mutable {
        std::invoke(std::move(func), std::move(lambda_args)...);
    };

    using bound_callback = decltype(bound_lambda);
    using bound_thread_data = threadData<bound_callback>;

    auto task = new bound_thread_data(std::move(bound_lambda));

    if (const int code = pthread_create(&this->handle, nullptr, &bound_thread_data::run, task); code != 0)
    {
        std::terminate();
    }
}

inline bool original::pThread::valid() const
{
    return this->handle != pthread_t{};
}

inline original::pThread::pThread(pThread&& other) noexcept
    : threadBase(std::move(other)), handle() {
    this->operator=(std::move(other));
}

inline original::pThread& original::pThread::operator=(pThread&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    if (this->is_joinable && this->valid()) {
        pthread_detach(this->handle);
    }

    this->handle = other.handle;
    other.handle = {};
    this->is_joinable = other.is_joinable;
    other.is_joinable = false;
    return *this;
}

inline original::ul_integer original::pThread::id() const {
    ul_integer id = 0;
    std::memcpy(&id, &this->handle, sizeof(pthread_t));
    return id;
}

inline void original::pThread::join() {
    if (this->is_joinable){
        if (const int code = pthread_join(this->handle, nullptr);
            code != 0){
            std::terminate();
        }
        this->is_joinable = false;
        this->handle = {};
    }
}

inline void original::pThread::detach() {
    if (this->is_joinable){
        if (const int code = pthread_detach(this->handle);
            code != 0){
            std::terminate();
        }
        this->is_joinable = false;
        this->handle = {};
    }
}

inline original::thread::thread()
    : will_join(true) {}

template <typename Callback, typename ... ARGS>
original::thread::thread(Callback c, ARGS&&... args)
    : thread_(std::forward<Callback>(c), std::forward<ARGS>(args)...), will_join(true) {}

template <typename Callback, typename ... ARGS>
original::thread::thread(Callback c, const joinPolicy policy, ARGS&&... args)
    : thread_(std::forward<Callback>(c), std::forward<ARGS>(args)...), will_join(policy == AUTO_JOIN) {}

inline original::thread::thread(pThread p_thread, const joinPolicy policy)
    : thread_(std::move(p_thread)), will_join(policy == AUTO_JOIN) {}

inline original::thread::thread(thread&& other) noexcept
    : thread_(std::move(other.thread_)), will_join(true) {}

inline original::thread::thread(thread&& other, const joinPolicy policy) noexcept
    : thread_(std::move(other.thread_)), will_join(policy == AUTO_JOIN) {}

inline original::thread& original::thread::operator=(thread&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    this->thread_ = std::move(other.thread_);
    this->will_join = other.will_join;
    other.will_join = false;
    return *this;
}

inline void original::thread::join()
{
    this->thread_.join();
}

inline void original::thread::detach()
{
    this->thread_.detach();
}

inline original::thread::~thread()
{
    if (this->thread_.joinable()) {
        this->will_join ? this->thread_.join() : this->thread_.detach();
    }
}

inline original::ul_integer original::thread::id() const {
    return this->thread_.id();
}

inline bool original::thread::joinable() const {
    return this->thread_.joinable();
}

inline original::thread::operator bool() const {
    return this->thread_.operator bool();
}

inline bool original::thread::operator!() const {
    return this->thread_.operator!();
}

#endif //THREAD_H
