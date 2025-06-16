#ifndef THREAD_H
#define THREAD_H

#include "error.h"
#include "functional"
#include "pthread.h"
#include "ownerPtr.h"


namespace original {
    class threadBase {
    protected:
        template<typename Callback>
        class threadData
        {
            Callback c;
        public:
            explicit threadData(Callback c);

            static void* run(void* arg);
        };

        bool is_joinable;

        [[nodiscard]] virtual bool valid() const = 0;

        explicit threadBase(bool is_joinable = false);
    public:
        virtual ~threadBase() noexcept(false);

        threadBase(const threadBase&) = delete;
        threadBase& operator=(const threadBase&) = delete;

        threadBase(threadBase&& other) noexcept = default;

        threadBase& operator=(threadBase&& other) noexcept = default;

        explicit operator bool() const;

        bool operator!() const;

        [[nodiscard]] bool joinable() const;
    };

    class pThread final : public threadBase {
        pthread_t handle;

        [[nodiscard]] bool valid() const override;
    public:
        explicit pThread();

        template<typename Callback, typename... ARGS>
        explicit pThread(Callback c, ARGS&&... args);

        pThread(pThread&& other) noexcept;

        pThread& operator=(pThread&& other) noexcept;

        void join();

        void detach();
    };

    class thread {

        pThread thread_;
        bool will_join;
    public:
        explicit thread();

        template<typename Callback, typename... ARGS>
        explicit thread(Callback c, ARGS&&... args);

        template<typename Callback, typename... ARGS>
        explicit thread(Callback c, bool will_join, ARGS&&... args);

        explicit thread(pThread p_thread, bool will_join = true);

        thread(const thread&) = delete;
        thread& operator=(const thread&) = delete;

        thread(thread&& other) noexcept;

        thread(thread&& other, bool will_join) noexcept;

        thread& operator=(thread&& other) noexcept;

        void join();

        void detach();

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

#endif //THREAD_H
