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
     */
    class threadBase {
    protected:
        /**
         * @class threadData
         * @tparam Callback Type of the thread callback function
         * @brief Wrapper for thread execution data
         */
        template<typename Callback>
        class threadData
        {
            Callback c;
        public:
            /**
             * @brief Construct thread data wrapper
             * @tparam Callback Callback function type
             * @param c Callback to store
             */
            explicit threadData(Callback c);

            /**
             * @brief Thread entry point wrapper
             * @tparam Callback Callback function type
             * @param arg Pointer to threadData instance
             * @return Always nullptr
             * @throw sysError if callback throws an error
             */
            static void* run(void* arg);
        };

        bool is_joinable;

        /**
         * @brief Check if thread is valid
         * @return true if thread is valid
         */
        [[nodiscard]] virtual bool valid() const = 0;

        virtual ul_integer id() const = 0;

        /**
         * @brief Construct thread base
         * @param is_joinable Whether thread is joinable
         */
        explicit threadBase(bool is_joinable = false);
    public:
        /**
         * @brief Destructor
         * @throw sysError if thread is joinable but not joined/detached
         */
        virtual ~threadBase() noexcept(false);

        threadBase(const threadBase&) = delete;
        threadBase& operator=(const threadBase&) = delete;

        threadBase(threadBase&& other) noexcept = default;
        threadBase& operator=(threadBase&& other) noexcept = default;

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
         */
        [[nodiscard]] bool joinable() const;
    };

    /**
     * @class pThread
     * @brief POSIX thread implementation
     * @details Wrapper around pthread with RAII semantics
     */
    class pThread final : public threadBase {
        pthread_t handle;

        [[nodiscard]] bool valid() const override;
    public:
        /**
         * @brief Construct empty (invalid) thread
         */
        explicit pThread();

        /**
         * @brief Construct and start POSIX thread
         * @tparam Callback Callback function type
         * @tparam ARGS Argument types for callback
         * @param c Callback function
         * @param args Arguments to forward to callback
         * @throw sysError if thread creation fails
         */
        template<typename Callback, typename... ARGS>
        explicit pThread(Callback c, ARGS&&... args);

        pThread(pThread&& other) noexcept;
        pThread& operator=(pThread&& other) noexcept;

        ul_integer id() const override;

        /**
         * @brief Wait for thread to complete
         * @throw sysError if join fails
         */
        void join();

        /**
         * @brief Detach thread (allow it to run independently)
         * @throw sysError if detach fails
         */
        void detach();
    };

    /**
     * @class thread
     * @brief High-level thread wrapper
     * @details Manages thread lifetime with automatic join/detach
     */
    class thread {
        pThread thread_;
        bool will_join;
    public:
        /**
         * @brief Construct empty thread
         */
        explicit thread();

        /**
         * @brief Construct and start thread with callback
         * @tparam Callback Callback function type
         * @tparam ARGS Argument types for callback
         * @param c Callback function
         * @param args Arguments to forward to callback
         * @note Will automatically join on destruction
         */
        template<typename Callback, typename... ARGS>
        explicit thread(Callback c, ARGS&&... args);

        /**
         * @brief Construct and start thread with callback and join policy
         * @tparam Callback Callback function type
         * @tparam ARGS Argument types for callback
         * @param c Callback function
         * @param will_join Whether to join on destruction
         * @param args Arguments to forward to callback
         */
        template<typename Callback, typename... ARGS>
        explicit thread(Callback c, bool will_join, ARGS&&... args);

        /**
         * @brief Construct from existing pThread
         * @param p_thread Thread to wrap
         * @param will_join Whether to join on destruction
         */
        explicit thread(pThread p_thread, bool will_join = true);

        thread(const thread&) = delete;
        thread& operator=(const thread&) = delete;

        /**
         * @brief Move constructor
         * @param other Thread to move from
         * @note Defaults to join policy of source thread
         */
        thread(thread&& other) noexcept;

        /**
         * @brief Move constructor with explicit join policy
         * @param other Thread to move from
         * @param will_join Override join policy
         */
        thread(thread&& other, bool will_join) noexcept;

        thread& operator=(thread&& other) noexcept;

        ul_integer id() const;

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
         * @throw sysError if join fails
         */
        void join();

        /**
         * @brief Detach thread (allow it to run independently)
         * @throw sysError if detach fails
         */
        void detach();

        /**
         * @brief Destructor
         * @note Automatically joins or detaches based on will_join policy
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

inline original::threadBase::~threadBase() noexcept(false) {
    if (this->is_joinable) {
        throw sysError();
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
        throw sysError();
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

original::ul_integer original::pThread::id() const {
    ul_integer id = 0;
    std::memcpy(&id, &this->handle, sizeof(pthread_t));
    return id;
}

inline void original::pThread::join() {
    if (this->is_joinable){
        if (const int code = pthread_join(this->handle, nullptr);
            code != 0){
            throw sysError();
        }
        this->is_joinable = false;
        this->handle = {};
    }
}

inline void original::pThread::detach() {
    if (this->is_joinable){
        if (const int code = pthread_detach(this->handle);
            code != 0){
            throw sysError();
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
original::thread::thread(Callback c, const bool will_join, ARGS&&... args)
    : thread_(std::forward<Callback>(c), std::forward<ARGS>(args)...), will_join(will_join) {}

inline original::thread::thread(pThread p_thread, const bool will_join)
    : thread_(std::move(p_thread)), will_join(will_join) {}

inline original::thread::thread(thread&& other) noexcept
    : thread_(std::move(other.thread_)), will_join(true) {}

inline original::thread::thread(thread&& other, const bool will_join) noexcept
    : thread_(std::move(other.thread_)), will_join(will_join) {}

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

original::ul_integer original::thread::id() const {
    return this->thread_.id();
}

bool original::thread::joinable() const {
    return this->thread_.joinable();
}

original::thread::operator bool() const {
    return this->thread_.operator bool();
}

bool original::thread::operator!() const {
    return this->thread_.operator!();
}

#endif //THREAD_H
