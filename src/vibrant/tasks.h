#ifndef ORIGINAL_TASKS_H
#define ORIGINAL_TASKS_H

#include "async.h"
#include "atomic.h"
#include "queue.h"
#include "refCntPtr.h"
#include "array.h"
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
        array<thread> threads_;
        queue<strongPtr<taskBase>, vector> tasks_high;
        queue<strongPtr<taskBase>, vector> tasks_normal;
        queue<strongPtr<taskBase>, vector> tasks_deferred;
        mutable pCondition condition_;
        mutable pMutex mutex_;
        atomic<bool> stopped_;
        u_integer active_threads_;
        u_integer idle_threads_;
    public:
        enum class priority : u_integer {
            HIGH = 0,
            NORMAL = 1,
            DEFERRED = 2,
        };
        explicit taskDelegator(u_integer thread_cnt = 8);

        template<typename Callback, typename... Args>
        auto submit(Callback&& c, Args&&... args);

        template<typename Callback, typename... Args>
        auto submit(priority priority, Callback&& c, Args&&... args);

        template<typename TYPE>
        async::future<TYPE> submit(const strongPtr<task<TYPE>>& t);

        void runDeferred();

        void runAllDeferred();

        void stop();

        u_integer tasksInQueue() const noexcept;

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

inline original::taskDelegator::taskDelegator(const u_integer thread_cnt)
    : threads_(thread_cnt), stopped_(makeAtomic(false)), active_threads_(0), idle_threads_(thread_cnt) {
    for (auto& thread_ : this->threads_) {
        thread_ = thread {
            [this]{
                while (true) {
                    strongPtr<taskBase> task;
                    {
                        uniqueLock lock(this->mutex_);
                        this->condition_.wait(this->mutex_, [this] {
                            return *this->stopped_ || this->tasksInQueue() != 0;
                        });

                        if (*this->stopped_ && this->tasksInQueue() == 0) {
                            this->idle_threads_ -= 1;
                            return;
                        }

                        if (!this->tasks_high.empty()) {
                            task = std::move(this->tasks_high.pop());
                        } else if (!this->tasks_normal.empty()) {
                            task = std::move(this->tasks_normal.pop());
                        } else if (!this->tasks_deferred.empty()) {
                            task = std::move(this->tasks_deferred.pop());
                        }

                        if (task) {
                            this->active_threads_ += 1;
                            this->idle_threads_ -= 1;
                        } else {
                            continue;
                        }
                    }

                    task->run();
                    {
                        uniqueLock lock(this->mutex_);
                        this->active_threads_ -= 1;
                        this->idle_threads_ += 1;
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
    auto f = new_task->getFuture();
    {
        uniqueLock lock(this->mutex_);
        if (this->stopped_) {
            throw sysError("taskDelegator already stopped");
        }
        switch (priority) {
            case priority::HIGH:
                this->tasks_high.push(new_task.template dynamicCastTo<taskBase>());
                this->condition_.notify();
                break;
            case priority::NORMAL:
                this->tasks_normal.push(new_task.template dynamicCastTo<taskBase>());
                this->condition_.notify();
                break;
            case priority::DEFERRED:
                this->tasks_deferred.push(new_task.template dynamicCastTo<taskBase>());
                break;
        }
    }
    return f;
}

template <typename TYPE>
original::async::future<TYPE>
original::taskDelegator::submit(const strongPtr<task<TYPE>>& t)
{
    auto f = t->getFuture();
    {
        uniqueLock lock(this->mutex_);
        if (this->stopped_) {
            throw sysError("taskDelegator already stopped");
        }
        this->tasks_normal.push(t.template dynamicCastTo<taskBase>());
    }
    this->condition_.notify();
    return f;
}

inline void original::taskDelegator::runDeferred()
{
    uniqueLock lock(this->mutex_);
    if (!this->tasks_deferred.empty()) {
        this->tasks_normal.push(this->tasks_deferred.pop());
        this->condition_.notify();
    }
}

inline void original::taskDelegator::runAllDeferred()
{
    while (!this->tasks_deferred.empty()) {
        this->runDeferred();
    }
}

inline void original::taskDelegator::stop()
{
    {
        uniqueLock lock(this->mutex_);
        this->stopped_.store(true);
    }
    this->condition_.notifyAll();

    for (auto& thread_ : this->threads_) {
        if (thread_.joinable()) {
            thread_.join();
        }
    }
}

inline original::u_integer
original::taskDelegator::tasksInQueue() const noexcept
{
    return this->tasks_high.size() +
           this->tasks_normal.size() +
           this->tasks_deferred.size();
}

inline original::u_integer original::taskDelegator::activeThreads() const noexcept
{
    return this->active_threads_;
}

inline original::u_integer original::taskDelegator::idleThreads() const noexcept
{
    return this->idle_threads_;
}

inline original::taskDelegator::~taskDelegator()
{
    if (!*this->stopped_) {
        this->stop();
    }
}

#endif //ORIGINAL_TASKS_H
