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
        queue<strongPtr<taskBase>, vector> tasks_;
        mutable pCondition condition_;
        mutable pMutex mutex_;
        atomic<bool> stopped_;
    public:
        explicit taskDelegator(u_integer thread_cnt);

        template<typename Callback, typename... Args>
        auto submit(Callback&& c, Args&&... args);

        u_integer tasksCount() const noexcept;

        void stop();

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

inline original::taskDelegator::taskDelegator(const u_integer thread_cnt = 8)
    : threads_(thread_cnt), stopped_(makeAtomic(false)) {
    for (auto& thread_ : this->threads_) {
        thread_ = thread {
            [this]{
                while (true) {
                    strongPtr<taskBase> task;
                    {
                        uniqueLock lock(this->mutex_);
                        this->condition_.wait(this->mutex_, [this] {
                            return *this->stopped_ || !this->tasks_.empty();
                        });
                        if (this->stopped_ && this->tasks_.empty()) {
                            return;
                        }
                        task = std::move(this->tasks_.pop());
                    }
                    task->run();
                }
            }
        };
    }
}

template <typename Callback, typename ... Args>
auto original::taskDelegator::submit(Callback&& c, Args&&... args)
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
        this->tasks_.push(new_task.template dynamicCastTo<taskBase>());
    }

    this->condition_.notify();
    return f;
}

inline original::u_integer
original::taskDelegator::tasksCount() const noexcept
{
    return this->tasks_.size();
}

inline void original::taskDelegator::stop()
{
    {
        uniqueLock lock(this->mutex_);
        this->stopped_.store(true);
    }
    this->condition_.notifyAll();
}

inline original::taskDelegator::~taskDelegator()
{
    this->stop();
    for (auto& thread_ : this->threads_) {
        if (thread_.joinable()) {
            thread_.join();
        }
    }
}

#endif //ORIGINAL_TASKS_H
