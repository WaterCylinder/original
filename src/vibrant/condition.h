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
        explicit pCondition();

        void wait(mutexBase& mutex) override;

        bool waitFor(mutexBase& mutex, time::duration d) override;

        void notify() override;

        void notifyAll() override;

        ~pCondition() override;
    };
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

    pthread_cond_wait(&this->cond_, static_cast<pMutex::native_handle*>(p_mutex->nativeHandle()));
}

inline bool original::pCondition::waitFor(mutexBase& mutex, const time::duration d)
{
    const auto p_mutex = dynamic_cast<pMutex*>(&mutex);
    if (!p_mutex) {
        throw valueError();
    }

    const auto ts = static_cast<timespec>(time::point::now() + d);
    const int code = pthread_cond_timedwait(&this->cond_,
                                            static_cast<pMutex::native_handle*>(p_mutex->nativeHandle()),
                                            &ts);
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
