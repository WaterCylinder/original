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

    public:
        explicit threadBase();

        virtual ~threadBase();

        threadBase(const threadBase&) = delete;
        threadBase& operator=(const threadBase&) = delete;

        threadBase(threadBase&& other) noexcept = default;

        threadBase& operator=(threadBase&& other) noexcept = default;

        explicit operator bool() const;

        bool operator!() const;
    };

    class pThread final : public threadBase {
        pthread_t handle;

        [[nodiscard]] bool valid() const override;
    public:
        template<typename Callback, typename... ARGS>
        explicit pThread(Callback c, ARGS&&... args);

        pThread(pThread&& other) noexcept;

        pThread& operator=(pThread&& other) noexcept;

        void join();

        void detach();
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

inline original::threadBase::threadBase()
    : is_joinable(true) {}

inline original::threadBase::~threadBase() {
    if (this->is_joinable) {
        throw sysError();
    }
}

template<typename Callback, typename... ARGS>
original::pThread::pThread(Callback c, ARGS&&... args) : handle()
{
    using bound_callback = decltype(std::bind(std::forward<Callback>(c), std::forward<ARGS>(args)...));
    using bound_thread_data = threadData<bound_callback>;

    auto task = new threadData<bound_callback>(std::bind(std::forward<Callback>(c), std::forward<ARGS>(args)...));

    if (const int code = pthread_create(&this->handle, nullptr, &bound_thread_data::run, task);
        code != 0)
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
    }
}

inline void original::pThread::detach() {
    if (this->is_joinable){
        if (const int code = pthread_detach(this->handle);
            code != 0){
            throw sysError();
        }
        this->is_joinable = false;
    }
}

#endif //THREAD_H
