#ifndef CONDITION_H
#define CONDITION_H
#include "mutex.h"
#include "zeit.h"


namespace original
{
    class conditionBase
    {
    public:
        explicit conditionBase() = default;

        virtual void wait(mutexBase& mutex) = 0;

        virtual bool waitFor(mutexBase& mutex, time::duration d) = 0;

        template<typename Pred>
        void wait(mutexBase& mutex, Pred predicate) noexcept(noexcept(predicate()));

        template<typename Pred>
        bool waitFor(mutexBase& mutex, const time::duration& d, Pred predicate) noexcept(noexcept(predicate()));

        virtual void notify() = 0;

        virtual void notifyAll() = 0;

        virtual ~conditionBase() = default;

        conditionBase(const conditionBase&) = delete;

        conditionBase& operator=(const conditionBase&) = delete;
    };

    class pCondition final : public conditionBase
    {
        pthread_cond_t cond_;

    public:
        using conditionBase::wait;
        using conditionBase::waitFor;

        explicit pCondition();

        void wait(mutexBase& mutex) override;

        bool waitFor(mutexBase& mutex, time::duration d) override;

        void notify() override;

        void notifyAll() override;

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

    auto handle = static_cast<pMutex::native_handle*>(p_mutex->nativeHandle());
    pthread_cond_wait(&this->cond_, handle);
}

inline bool original::pCondition::waitFor(mutexBase& mutex, const time::duration d)
{
    const auto p_mutex = dynamic_cast<pMutex*>(&mutex);
    if (!p_mutex) {
        throw valueError();
    }

    const auto ts = static_cast<timespec>(time::point::now() + d);
    auto handle = static_cast<pMutex::native_handle*>(p_mutex->nativeHandle());
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
