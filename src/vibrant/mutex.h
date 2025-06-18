#ifndef MUTEX_H
#define MUTEX_H
#include "pthread.h"
#include "error.h"


namespace original {
    class mutexBase {
    protected:
        virtual void lock() = 0;

        virtual bool tryLock() = 0;

        virtual void unlock() = 0;
    public:
        explicit mutexBase() = default;

        mutexBase(const mutexBase&) = delete;

        mutexBase& operator=(const mutexBase&) = delete;

        virtual ~mutexBase() = default;
    };

    class pMutex final : public mutexBase {
        pthread_mutex_t mutex_;
    public:
        explicit pMutex();

        pMutex(pMutex&&) = delete;

        pMutex& operator=(pMutex&&) = delete;

        void lock() override;

        bool tryLock() override;

        void unlock() override;

        ~pMutex() override;
    };

    class scopeLock {
        pMutex& p_mutex_;
        bool is_locked;
    public:
        explicit scopeLock(pMutex& p_mutex, bool try_lock = false);

        scopeLock(const scopeLock&) = delete;

        scopeLock& operator=(const scopeLock&) = delete;

        scopeLock(scopeLock&&) = delete;

        scopeLock& operator=(scopeLock&&) = delete;

        ~mutex();
    };
}

original::pMutex::pMutex() : mutex_{} {
    if (int code = pthread_mutex_init(&this->mutex_, nullptr);
        code != 0){
        throw sysError();
    }
}

void original::pMutex::lock() {
    if (int code = pthread_mutex_lock(&this->mutex_);
        code != 0) {
        throw sysError();
    }
}


bool original::pMutex::tryLock() {
    if (int code = pthread_mutex_trylock(&this->mutex_);
            code != 0) {
        if (code == EBUSY)
            return false;

        throw sysError();
    }
    return true;
}

void original::pMutex::unlock() {
    if (int code = pthread_mutex_unlock(&this->mutex_);
        code != 0){
        throw sysError();
    }
}

original::pMutex::~pMutex() {
    if (int code = pthread_mutex_destroy(&this->mutex_);
        code != 0){
        std::terminate();
    }
}

original::mutex::mutex(bool try_lock) : p_mutex_{}, try_lock(try_lock) {
    if (this->try_lock){
        this->p_mutex_.tryLock();
    } else{
        this->p_mutex_.lock();
    }
}

original::scopeLock::~scopeLock() {
    if (this->is_locked)
        this->p_mutex_.unlock();
}

#endif //MUTEX_H
