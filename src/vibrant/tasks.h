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
    class taskBase {
        public:
        virtual void run() = 0;
        virtual ~taskBase() = default;
    };

    template<typename TYPE>
    class task final : public taskBase {
        async::promise<TYPE, std::function<TYPE()>> p;

        public:
        task(const task&) = delete;
        task& operator=(const task&) = delete;
        task(task&&) = default;
        task& operator=(task&&) = default;

        task() = default;

        template<typename Callback, typename... Args>
        explicit task(Callback&& c, Args&&... args);

        void run() override;

        async::future<TYPE> getFuture();
    };

    class taskDelegator {
    public:
        enum class priority : u_integer {
            IMMEDIATE = 0,
            HIGH = 1,
            NORMAL = 2,
            LOW = 3,
            DEFERRED = 4,
        };
    private:
        using priorityTask = couple<strongPtr<taskBase>, priority>;
        template<typename COUPLE>
        struct taskComparator {
            bool operator()(const COUPLE& lhs, const COUPLE& rhs) const;
        };
        using priorityTaskQueue = prique<priorityTask, taskComparator, vector>;

        array<thread> threads_;
        priorityTaskQueue tasks_waiting_;
        queue<strongPtr<taskBase>> task_immediate_;
        queue<strongPtr<taskBase>> tasks_deferred_;
        mutable pCondition condition_;
        mutable pMutex mutex_;
        bool stopped_;
        u_integer active_threads_;
        u_integer idle_threads_;
    public:
        explicit taskDelegator(u_integer thread_cnt = 8);

        template<typename Callback, typename... Args>
        auto submit(Callback&& c, Args&&... args);

        template<typename Callback, typename... Args>
        auto submit(priority priority, Callback&& c, Args&&... args);

        template<typename TYPE>
        async::future<TYPE> submit(strongPtr<task<TYPE>>& t);

        template<typename TYPE>
        async::future<TYPE> submit(priority priority, strongPtr<task<TYPE>>& t);

        void runDeferred();

        void runAllDeferred();

        void stop();

        u_integer activeThreads() const noexcept;

        u_integer idleThreads() const noexcept;

        ~taskDelegator();
    };
}

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
