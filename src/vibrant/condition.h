#ifndef CONDITION_H
#define CONDITION_H
#include "mutex.h"
#include "zeit.h"

/**
 * @file condition.h
 * @brief Condition variable implementation for thread synchronization
 * @details Provides condition variable functionality for coordinating
 * between threads, including:
 * - Basic wait/notify operations
 * - Timed waits with duration support
 * - Predicate-based waiting
 * - Integration with mutex.h locking mechanisms
 *
 * Key features:
 * - POSIX-based implementation (pCondition)
 * - Thread-safe condition variable operations
 * - Timeout support using zeit.h duration types
 * - Predicate templates for safe condition checking
 */

namespace original
{
    /**
     * @class conditionBase
     * @brief Abstract base class for condition variable implementations
     * @details Provides the interface for thread synchronization operations:
     * - Waiting with mutex protection
     * - Timed waiting with duration support
     * - Notification of waiting threads
     * - Predicate-based waiting templates
     *
     * @note This is an abstract base class and cannot be instantiated directly.
     *       Derived classes must implement all pure virtual methods.
     */
    class conditionBase
    {
    public:
        /// Default constructor
        explicit conditionBase() = default;

        /**
         * @brief Waits for notification while holding the mutex
         * @param mutex Locked mutex to wait on
         * @throws sysError if wait operation fails
         * @note The mutex must be locked by the calling thread
         */
        virtual void wait(mutexBase& mutex) = 0;

        /**
         * @brief Waits for notification with timeout
         * @param mutex Locked mutex to wait on
         * @param d Maximum duration to wait
         * @return true if notified, false if timeout occurred
         * @throws sysError if wait operation fails
         * @note The mutex must be locked by the calling thread
         */
        virtual bool waitFor(mutexBase& mutex, time::duration d) = 0;

        /**
         * @brief Waits until predicate becomes true
         * @tparam Pred Predicate type (must be callable and return bool)
         * @param mutex Locked mutex to wait on
         * @param predicate Condition predicate to check
         * @throws sysError if wait operation fails
         * @note Implements the "wait with predicate" pattern to avoid spurious wakeups
         */
        template<typename Pred>
        void wait(mutexBase& mutex, Pred predicate) noexcept(noexcept(predicate()));

        /**
         * @brief Waits with timeout until predicate becomes true
         * @tparam Pred Predicate type (must be callable and return bool)
         * @param mutex Locked mutex to wait on
         * @param d Maximum duration to wait
         * @param predicate Condition predicate to check
         * @return true if predicate became true, false if timeout occurred
         * @throws sysError if wait operation fails
         */
        template<typename Pred>
        bool waitFor(mutexBase& mutex, const time::duration& d, Pred predicate) noexcept(noexcept(predicate()));

        /**
         * @brief Notifies one waiting thread
         * @throws sysError if notification fails
         */
        virtual void notify() = 0;

        /**
         * @brief Notifies all waiting threads
         * @throws sysError if notification fails
         */
        virtual void notifyAll() = 0;

        /// Virtual destructor
        virtual ~conditionBase() = default;

        /// Deleted copy constructor
        conditionBase(const conditionBase&) = delete;

        /// Deleted copy assignment operator
        conditionBase& operator=(const conditionBase&) = delete;
    };

    /**
     * @class pCondition
     * @brief POSIX condition variable implementation
     * @extends conditionBase
     * @details Wrapper around pthread_cond_t with RAII semantics.
     * Provides thread synchronization using POSIX condition variables.
     */
    class pCondition final : public conditionBase
    {
        pthread_cond_t cond_; ///< Internal POSIX condition variable handle

    public:
        // Inherit template methods from conditionBase
        using conditionBase::wait;
        using conditionBase::waitFor;

        /**
         * @brief Constructs and initializes the condition variable
         * @throws sysError if initialization fails
         */
        explicit pCondition();

        /**
         * @brief Waits for notification while holding the mutex
         * @param mutex Locked mutex to wait on
         * @throws sysError if wait operation fails
         * @throws valueError if mutex is not a pMutex
         */
        void wait(mutexBase& mutex) override;

        /**
         * @brief Waits for notification with timeout
         * @param mutex Locked mutex to wait on
         * @param d Maximum duration to wait
         * @return true if notified, false if timeout occurred
         * @throws sysError if wait operation fails
         * @throws valueError if mutex is not a pMutex
         */
        bool waitFor(mutexBase& mutex, time::duration d) override;

        /**
         * @brief Notifies one waiting thread
         * @throws sysError if notification fails
         */
        void notify() override;

        /**
         * @brief Notifies all waiting threads
         * @throws sysError if notification fails
         */
        void notifyAll() override;

        /**
         * @brief Destroys the condition variable
         * @note Calls pthread_cond_destroy()
         */
        ~pCondition() override;
    };
}

template<typename Pred>
void original::conditionBase::wait(mutexBase& mutex, Pred predicate) noexcept(noexcept(predicate())) {
    while (!predicate()){
        this->wait(mutex);
    }
}

template<typename Pred>
bool original::conditionBase::waitFor(mutexBase& mutex, const time::duration& d, Pred predicate) noexcept(noexcept(predicate())) {
    const time::point start = time::point::now();
    while (!predicate()) {
        auto elapsed = time::point::now() - start;
        if (elapsed >= d)
            return false;
        if (!this->waitFor(mutex, d - elapsed))
            return false;
        if (predicate())
            return true;
    }
    return true;
}

inline original::pCondition::pCondition() : cond_{}
{
    if (pthread_cond_init(&this->cond_, nullptr) != 0)
    {
        throw sysError();
    }
}

inline void original::pCondition::wait(mutexBase& mutex)
{
    const auto p_mutex = dynamic_cast<pMutex*>(&mutex);
    if (!p_mutex) {
        throw valueError();
    }

    const auto handle = static_cast<pMutex::native_handle*>(p_mutex->nativeHandle());
    pthread_cond_wait(&this->cond_, handle);
}

inline bool original::pCondition::waitFor(mutexBase& mutex, const time::duration d)
{
    const auto p_mutex = dynamic_cast<pMutex*>(&mutex);
    if (!p_mutex) {
        throw valueError();
    }

    const auto deadline = time::point::now() + d;
    const auto ts = deadline.toTimespec();
    const auto handle = static_cast<pMutex::native_handle*>(p_mutex->nativeHandle());
    const int code = pthread_cond_timedwait(&this->cond_, handle, &ts);
    if (code == 0) return true;
    if (code == ETIMEDOUT) return false;
    throw sysError();
}

inline void original::pCondition::notify()
{
    pthread_cond_signal(&this->cond_);
}

inline void original::pCondition::notifyAll()
{
    pthread_cond_broadcast(&this->cond_);
}

inline original::pCondition::~pCondition()
{
    pthread_cond_destroy(&this->cond_);
}

#endif //CONDITION_H
