#ifndef ORIGINAL_TASKS_H
#define ORIGINAL_TASKS_H

#include "async.h"
#include "queue.h"
#include "refCntPtr.h"
#include "array.h"


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
        bool stopped_;
        u_integer started_;
    public:
        explicit taskDelegator(u_integer thread_cnt);

        u_integer tasksCount() const noexcept;
    };
}

template <typename TYPE>
template <typename Callback, typename ... Args>
original::task<TYPE>::task(Callback&& c, Args&&... args)
    : p(async::makePromise(std::forward<Callback>(c), std::forward<Args>(args)...)) {}

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
    : threads_(thread_cnt), stopped_(false), started_(0) {
    for (auto& thread_ : this->threads_) {
        thread new_thread {
            [this]{
                while (true) {
                    strongPtr<taskBase> task;
                    {
                        uniqueLock lock(this->mutex_);
                        this->condition_.wait(this->mutex_, [this]
                        {
                            return this->started_ != 0 || this->stopped_;
                        });
                        if (this->stopped_ && this->started_ == 0 && this->tasks_.empty()) {
                            return;
                        }
                        task = std::move(this->tasks_.pop());
                    }
                    this->started_ += 1;
                    task->run();
                }
            }
        };
        thread_ = std::move(new_thread);
    }
}

inline original::u_integer
original::taskDelegator::tasksCount() const noexcept
{
    return this->tasks_.size();
}

#endif //ORIGINAL_TASKS_H
