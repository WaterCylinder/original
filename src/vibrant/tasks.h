#ifndef ORIGINAL_TASKS_H
#define ORIGINAL_TASKS_H

#include "async.h"
#include "queue.h"
#include "ownerPtr.h"
#include "refCntPtr.h"


// todo
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

        template<typename Callback, typename... Args>
        explicit task(Callback&& c, Args&&... args);

        void run() override;

        async::future<TYPE> getFuture();
    };

    class taskDelegator {
        enum class shutdownMode {
            NORMAL,
            IMMEDIATE,
        };

        ownerPtr<thread, deleter<thread[]>> threads_;
        queue<strongPtr<taskBase>, vector> tasks_;
        mutable pCondition condition_;
        mutable pMutex mutex_;
        shutdownMode mode_;
        u_integer started_;
    public:
        explicit taskDelegator(u_integer thread_cnt);

        taskDelegator(u_integer thread_cnt, const std::initializer_list<strongPtr<taskBase>>& tasks);

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
    : threads_(makeOwnerPtrArray<thread>(thread_cnt)), mode_(shutdownMode::NORMAL), started_(0)
{
    // todo
}

inline original::taskDelegator::taskDelegator(const u_integer thread_cnt, const std::initializer_list<strongPtr<taskBase>>& tasks)
    : taskDelegator(thread_cnt)
{
    this->tasks_ = tasks;
}

inline original::u_integer
original::taskDelegator::tasksCount() const noexcept
{
    return this->tasks_.size();
}

#endif //ORIGINAL_TASKS_H
