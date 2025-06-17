#ifndef MUTEX_H
#define MUTEX_H
#include "pthread.h"
#include "error.h"


namespace original {
    class mutexBase {
    protected:
        virtual void lock() = 0;

        virtual void tryLock() = 0;

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

        void tryLock() override;

        void unlock() override;

        ~pMutex() override;
    };

    class mutex {
        pMutex p_mutex_;
        bool try_lock;
    public:
        explicit mutex(bool try_lock = false);

        mutex(const mutex&) = delete;

        mutex& operator=(const mutex&) = delete;

        mutex(mutex&&) = delete;

        mutex& operator=(mutex&&) = delete;

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


void original::pMutex::tryLock() {
    if (int code = pthread_mutex_trylock(&this->mutex_);
            code != 0) {
        throw sysError();
    }
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

original::mutex::~mutex() {
    this->p_mutex_.unlock();
}

#endif //MUTEX_H
