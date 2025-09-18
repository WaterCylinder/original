#ifndef ORIGINAL_TASKS_H
#define ORIGINAL_TASKS_H

#include "async.h"
#include "atomic.h"
#include "queue.h"
#include "refCntPtr.h"
#include "array.h"
#include "prique.h"
#include "vector.h"

namespace original {

    // ==================== Task Delegator (Thread Pool) ====================

    /**
     * @class taskDelegator
     * @brief Thread pool for managing and executing prioritized tasks
     * @details
     * Provides a managed thread pool that can execute tasks with different priority levels.
     * Supports immediate, high, normal, low, and deferred tasks. Deferred tasks can be
     * manually activated or discarded on shutdown.
     */
    class taskDelegator {
        // ==================== Task Base Interface ====================

        /**
         * @class taskBase
         * @brief Abstract base class for all tasks
         * @details
         * Provides the common interface for tasks to be executed in the thread pool.
         * Must implement the run() method.
         */
        class taskBase {
        public:
            /**
             * @brief Executes the task
             */
            virtual void run() = 0;

            /**
             * @brief Virtual destructor for proper polymorphic behavior
             */
            virtual ~taskBase() = default;
        };

        // ==================== Concrete Task Class ====================

        /**
         * @class task
         * @brief Concrete task implementation with future/promise support
         * @tparam TYPE Return type of the task
         * @details
         * Wraps a callable and its arguments, and provides integration with futures.
         */
        template<typename TYPE>
        class task final : public taskBase {
            async::promise<TYPE, std::function<TYPE()>> p;  ///< Promise for task result

        public:
            task(const task&) = delete;                 ///< Disable copy constructor
            task& operator=(const task&) = delete;      ///< Disable copy assignment
            task(task&&) = default;                     ///< Allow move constructor
            task& operator=(task&&) = default;          ///< Allow move assignment
            task() = default;

            /**
             * @brief Constructs a task from a callable and its arguments
             * @tparam Callback Callable type
             * @tparam Args Argument types
             * @param c Callable to execute
             * @param args Arguments for the callable
             */
            template<typename Callback, typename... Args>
            explicit task(Callback&& c, Args&&... args);

            /**
             * @brief Executes the task
             */
            void run() override;

            /**
             * @brief Gets the future associated with this task
             * @return Future object for the task result
             */
            async::future<TYPE> getFuture();
        };

    public:
        // ==================== Task Priorities ====================

        /**
         * @enum priority
         * @brief Task priority levels for execution scheduling
         */
        enum class priority : u_integer {
            IMMEDIATE = 0,  ///< Execute immediately if threads available
            HIGH = 1,       ///< High priority task
            NORMAL = 2,     ///< Normal priority task (default)
            LOW = 3,        ///< Low priority task
            DEFERRED = 4,   ///< Deferred execution
        };

        /**
         * @enum stopMode
         * @brief Stop behavior for deferred tasks
         */
        enum class stopMode {
            DISCARD_DEFERRED,  ///< Discard deferred tasks
            KEEP_DEFERRED,     ///< Keep deferred tasks
            RUN_DEFERRED,      ///< Execute all deferred tasks before stopping
        };

        // Convenience constants
        static constexpr auto IMMEDIATE = priority::IMMEDIATE;
        static constexpr auto HIGH = priority::HIGH;
        static constexpr auto NORMAL = priority::NORMAL;
        static constexpr auto LOW = priority::LOW;
        static constexpr auto DEFERRED = priority::DEFERRED;

        static constexpr auto DISCARD_DEFERRED = stopMode::DISCARD_DEFERRED;
        static constexpr auto KEEP_DEFERRED = stopMode::KEEP_DEFERRED;
        static constexpr auto RUN_DEFERRED = stopMode::RUN_DEFERRED;

    private:
        // Internal type definitions
        using priorityTask = couple<strongPtr<taskBase>, priority>;  ///< Task with priority

        /**
         * @struct taskComparator
         * @brief Comparator for ordering tasks by priority
         */
        template<typename COUPLE>
        struct taskComparator {
            /**
             * @brief Compares two tasks based on priority
             * @param lhs Left-hand side task
             * @param rhs Right-hand side task
             * @return True if lhs has higher priority than rhs
             */
            bool operator()(const COUPLE& lhs, const COUPLE& rhs) const;
        };

        using priorityTaskQueue = prique<priorityTask, taskComparator, vector>;  ///< Priority queue

        array<thread> threads_;              ///< Worker threads
        priorityTaskQueue tasks_waiting_;    ///< Waiting tasks
        queue<strongPtr<taskBase>> task_immediate_;  ///< Immediate tasks
        queue<strongPtr<taskBase>> tasks_deferred_;  ///< Deferred tasks
        mutable pCondition condition_;       ///< Synchronization
        mutable pMutex mutex_;               ///< Mutex for thread safety
        bool stopped_;                       ///< Stop flag
        u_integer active_threads_;           ///< Count of active threads
        u_integer idle_threads_;             ///< Count of idle threads

        /**
         * @brief Submits a pre-created task with specified priority
         * @tparam TYPE Task result type
         * @param priority Task priority level
         * @param t Shared pointer to the task
         * @return Future for the task result
         */
        template<typename TYPE>
        async::future<TYPE> submit(priority priority, strongPtr<task<TYPE>>& t);

    public:
        /**
         * @brief Constructs a task delegator with a given number of threads
         * @param thread_cnt Number of threads (default: 8)
         */
        explicit taskDelegator(u_integer thread_cnt = 8);

        /**
         * @brief Submits a task with normal priority
         * @tparam Callback Type of the callable
         * @tparam Args Types of the arguments
         * @param c Callable to execute
         * @param args Arguments to forward to the callable
         * @return Future for the task result
         */
        template<typename Callback, typename... Args>
        auto submit(Callback&& c, Args&&... args);

        /**
         * @brief Submits a task with specified priority
         * @tparam Callback Type of the callable
         * @tparam Args Types of the arguments
         * @param priority Task priority level
         * @param c Callable to execute
         * @param args Arguments to forward to the callable
         * @return Future for the task result
         */
        template<typename Callback, typename... Args>
        auto submit(priority priority, Callback&& c, Args&&... args);

        /**
         * @brief Activates one deferred task
         */
        void runDeferred();

        /**
         * @brief Activates all deferred tasks
         */
        void runAllDeferred();

        /**
         * @brief Returns number of deferred tasks
         */
        u_integer deferredCnt() const noexcept;

        /**
         * @brief Stops the task delegator
         * @param mode Stop mode (default: KEEP_DEFERRED)
         */
        void stop(stopMode mode = stopMode::KEEP_DEFERRED);

        /**
         * @brief Gets the number of active threads
         * @return Count of currently active threads
         */
        u_integer activeThreads() const noexcept;

        /**
         * @brief Gets the number of idle threads
         * @return Count of currently idle threads
         */
        u_integer idleThreads() const noexcept;

        /**
         * @brief Destructor - automatically stops if not already stopped
         */
        ~taskDelegator();
    };
} // namespace original

// ==================== Task Implementation ====================

template <typename TYPE>
template <typename Callback, typename... Args>
original::taskDelegator::task<TYPE>::task(Callback&& c, Args&&... args)
    : p([c = std::forward<Callback>(c), ...args = std::forward<Args>(args)]() mutable {
        return c(args...);
    }) {}

template <typename TYPE>
void original::taskDelegator::task<TYPE>::run()
{
    this->p.run();
}

template <typename TYPE>
original::async::future<TYPE> original::taskDelegator::task<TYPE>::getFuture()
{
    return this->p.getFuture();
}

// ==================== Task Delegator Implementation ====================

template <typename COUPLE>
bool original::taskDelegator::taskComparator<COUPLE>::operator()(const COUPLE& lhs, const COUPLE& rhs) const
{
    return static_cast<u_integer>(lhs.second()) < static_cast<u_integer>(rhs.second());
}

inline original::taskDelegator::taskDelegator(const u_integer thread_cnt)
    : threads_(thread_cnt),
      stopped_(false),
      active_threads_(0),
      idle_threads_(0) {
    for (auto& thread_ : this->threads_) {
        thread_ = thread {
            [this]{
                while (true) {
                    strongPtr<taskBase> task;
                    {
                        uniqueLock lock(this->mutex_);
                        this->idle_threads_ += 1;
                        this->condition_.wait(this->mutex_, [this] {
                            return this->stopped_ || !this->tasks_waiting_.empty() || !this->task_immediate_.empty();
                        });

                        if (this->stopped_ &&
                            this->tasks_waiting_.empty() &&
                            this->task_immediate_.empty()) {
                                this->idle_threads_ -= 1;
                                return;
                        }

                        if (!this->task_immediate_.empty()) {
                            task = std::move(this->task_immediate_.pop());
                        } else {
                            task = std::move(this->tasks_waiting_.pop().first());
                        }
                        this->idle_threads_ -= 1;
                    }

                    {
                        uniqueLock lock(this->mutex_);
                        this->active_threads_ += 1;
                    }
                    task->run();
                    {
                        uniqueLock lock(this->mutex_);
                        this->active_threads_ -= 1;
                    }
                }
            }
        };
    }
}

template <typename Callback, typename ... Args>
auto original::taskDelegator::submit(Callback&& c, Args&&... args)
{
    return this->submit(priority::NORMAL, std::forward<Callback>(c), std::forward<Args>(args)...);
}

template <typename Callback, typename ... Args>
auto original::taskDelegator::submit(const priority priority, Callback&& c, Args&&... args)
{
    using ReturnType = decltype(c(args...));
    auto new_task = makeStrongPtr<task<ReturnType>>(
        std::forward<Callback>(c),
        std::forward<Args>(args)...
    );
    return this->submit<ReturnType>(priority, new_task);
}

template <typename TYPE>
original::async::future<TYPE>
original::taskDelegator::submit(const priority priority, strongPtr<task<TYPE>>& t)
{
    auto f = t->getFuture();
    {
        uniqueLock lock(this->mutex_);
        if (this->stopped_) {
            throw sysError("taskDelegator already stopped");
        }
        switch (priority) {
        case priority::IMMEDIATE:
            if (this->idle_threads_ == 0) {
                throw sysError("No idle threads now");
            }
            this->task_immediate_.push(std::move(t.template dynamicCastTo<taskBase>()));
            break;
        case priority::HIGH:
        case priority::NORMAL:
        case priority::LOW:
            this->tasks_waiting_.push(priorityTask{t.template dynamicCastTo<taskBase>(), priority});
            break;
        case priority::DEFERRED:
            this->tasks_deferred_.push(t.template dynamicCastTo<taskBase>());
            return f;
        default:
            throw sysError("Unknown priority");
        }
    }
    this->condition_.notify();
    return f;
}

inline void original::taskDelegator::runDeferred()
{
    {
        uniqueLock lock(this->mutex_);
        if (!this->tasks_deferred_.empty()) {
            this->tasks_waiting_.push(priorityTask{this->tasks_deferred_.pop(), priority::DEFERRED});
        } else {
            return;
        }
    }
    this->condition_.notify();
}

inline void original::taskDelegator::runAllDeferred()
{
    {
        uniqueLock lock(this->mutex_);
        if (this->tasks_deferred_.empty()) {
            return;
        }
        while (!this->tasks_deferred_.empty()) {
            this->tasks_waiting_.push(priorityTask{this->tasks_deferred_.pop(), priority::DEFERRED});
        }
    }
    this->condition_.notifyAll();
}

inline original::u_integer original::taskDelegator::deferredCnt() const noexcept
{
    uniqueLock lock(this->mutex_);
    return this->tasks_deferred_.size();
}

inline void original::taskDelegator::stop(const stopMode mode)
{
    {
        uniqueLock lock(this->mutex_);
        switch (mode) {
        case RUN_DEFERRED:
            while (!this->tasks_deferred_.empty()) {
                this->tasks_waiting_.push(priorityTask{this->tasks_deferred_.pop(), DEFERRED});
            }
            break;
        case DISCARD_DEFERRED:
            this->tasks_deferred_.clear();
            break;
        case KEEP_DEFERRED:
            break;
        default:
            throw sysError("Unknown stop mode");
        }
        this->stopped_ = true;
    }
    this->condition_.notifyAll();
}

inline original::u_integer original::taskDelegator::activeThreads() const noexcept
{
    uniqueLock lock(this->mutex_);
    return this->active_threads_;
}

inline original::u_integer original::taskDelegator::idleThreads() const noexcept
{
    uniqueLock lock(this->mutex_);
    return this->idle_threads_;
}

inline original::taskDelegator::~taskDelegator()
{
    stop(stopMode::RUN_DEFERRED);
    for (auto& thread : threads_) {
        if (thread.joinable())
            thread.join();
    }
}

#endif //ORIGINAL_TASKS_H
