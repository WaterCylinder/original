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

    // ==================== Task Base Interface ====================

    /**
     * @class taskBase
     * @brief Abstract base class for all task types
     * @details Defines the common interface for executable tasks
     */
    class taskBase {
    public:
        /**
         * @brief Executes the task
         */
        virtual void run() = 0;

        /**
         * @brief Virtual destructor for proper polymorphism
         */
        virtual ~taskBase() = default;
    };

    // ==================== Concrete Task Class ====================

    /**
     * @class task
     * @brief Concrete task implementation with result type
     * @tparam TYPE The return type of the task computation
     * @details Wraps a computation and provides future/promise integration
     */
    template<typename TYPE>
    class task final : public taskBase {
        async::promise<TYPE, std::function<TYPE()>> p;  ///< Promise for the computation result

    public:
        // Disable copying
        task(const task&) = delete;
        task& operator=(const task&) = delete;

        // Allow moving
        task(task&&) = default;
        task& operator=(task&&) = default;

        /**
         * @brief Default constructor
         */
        task() = default;

        /**
         * @brief Constructs a task with a callable and arguments
         * @tparam Callback Type of the callable
         * @tparam Args Types of the arguments
         * @param c Callable to execute
         * @param args Arguments to forward to the callable
         */
        template<typename Callback, typename... Args>
        explicit task(Callback&& c, Args&&... args);

        /**
         * @brief Executes the task computation
         */
        void run() override;

        /**
         * @brief Gets the future associated with this task
         * @return A future that will receive the computation result
         */
        async::future<TYPE> getFuture();
    };

    // ==================== Task Delegator (Thread Pool) ====================

    /**
     * @class taskDelegator
     * @brief Thread pool for managing and executing tasks with priority
     * @details Provides a managed thread pool with task prioritization,
     *          deferred execution, and resource monitoring
     */
    class taskDelegator {
    public:
        /**
         * @enum priority
         * @brief Task priority levels for execution scheduling
         */
        enum class priority : u_integer {
            IMMEDIATE = 0,  ///< Execute immediately if threads available
            HIGH = 1,       ///< High priority task
            NORMAL = 2,     ///< Normal priority task (default)
            LOW = 3,        ///< Low priority task
            DEFERRED = 4,   ///< Deferred execution (manual activation)
        };

        // Priority constants for convenience
        static constexpr auto IMMEDIATE = priority::IMMEDIATE;
        static constexpr auto HIGH = priority::HIGH;
        static constexpr auto NORMAL = priority::NORMAL;
        static constexpr auto LOW = priority::LOW;
        static constexpr auto DEFERRED = priority::DEFERRED;

    private:
        // Internal type definitions
        using priorityTask = couple<strongPtr<taskBase>, priority>;  ///< Task with priority

        /**
         * @struct taskComparator
         * @brief Comparator for priority-based task ordering
         * @tparam COUPLE Type of the priority task couple
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

        using priorityTaskQueue = prique<priorityTask, taskComparator, vector>;  ///< Priority task queue

        // Member variables
        array<thread> threads_;              ///< Worker threads
        priorityTaskQueue tasks_waiting_;    ///< Waiting tasks with priority
        queue<strongPtr<taskBase>> task_immediate_;  ///< Immediate execution tasks
        queue<strongPtr<taskBase>> tasks_deferred_;  ///< Deferred execution tasks
        mutable pCondition condition_;       ///< Condition variable for synchronization
        mutable pMutex mutex_;               ///< Mutex for thread safety
        bool stopped_;                       ///< Stop flag
        u_integer active_threads_;           ///< Count of active threads
        u_integer idle_threads_;             ///< Count of idle threads

    public:
        /**
         * @brief Constructs a task delegator with specified thread count
         * @param thread_cnt Number of worker threads (default: 8)
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
         * @brief Submits a pre-created task with normal priority
         * @tparam TYPE Task result type
         * @param t Shared pointer to the task
         * @return Future for the task result
         */
        template<typename TYPE>
        async::future<TYPE> submit(strongPtr<task<TYPE>>& t);

        /**
         * @brief Submits a pre-created task with specified priority
         * @tparam TYPE Task result type
         * @param priority Task priority level
         * @param t Shared pointer to the task
         * @return Future for the task result
         */
        template<typename TYPE>
        async::future<TYPE> submit(priority priority, strongPtr<task<TYPE>>& t);

        /**
         * @brief Moves one deferred task to the waiting queue
         */
        void runDeferred();

        /**
         * @brief Moves all deferred tasks to the waiting queue
         */
        void runAllDeferred();

        /**
         * @brief Stops the task delegator and waits for completion
         */
        void stop();

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

template <typename TYPE>
template <typename Callback, typename... Args>
original::task<TYPE>::task(Callback&& c, Args&&... args)
    : p([c = std::forward<Callback>(c), ...args = std::forward<Args>(args)]() mutable {
        return c(args...);
    }) {}

template <typename TYPE>
void original::task<TYPE>::run()
{
    this->p.run();
}

template <typename TYPE>
original::async::future<TYPE> original::task<TYPE>::getFuture()
{
    return this->p.getFuture();
}

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
                            this->tasks_deferred_.empty() &&
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
original::async::future<TYPE> original::taskDelegator::submit(strongPtr<task<TYPE>>& t)
{
    return this->submit(priority::NORMAL, t);
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

inline void original::taskDelegator::stop()
{
    {
        uniqueLock lock(this->mutex_);
        this->stopped_ = true;
    }
    this->condition_.notifyAll();

    for (auto& thread_ : this->threads_) {
        if (thread_.joinable()) {
            thread_.join();
        }
    }
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
    bool stopped;
    {
        uniqueLock lock(this->mutex_);
        stopped = this->stopped_;
    }
    if (!stopped) {
        this->stop();
    }
}

#endif //ORIGINAL_TASKS_H
