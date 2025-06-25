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

        [[nodiscard]] virtual ul_integer id() const = 0;
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

        [[nodiscard]] ul_integer id() const override;

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

        [[nodiscard]] bool isLocked() const;

        ~scopeLock();
    };
}

inline original::pMutex::pMutex() : mutex_{} {
    if (const int code = pthread_mutex_init(&this->mutex_, nullptr);
        code != 0){
        throw sysError();
    }
}

original::ul_integer original::pMutex::id() const {
    ul_integer id = 0;
    std::memcpy(&id, &this->mutex_, sizeof(pthread_mutex_t));
    return id;
}

inline void original::pMutex::lock() {
    if (const int code = pthread_mutex_lock(&this->mutex_);
        code != 0) {
        throw sysError();
    }
}

inline bool original::pMutex::tryLock() {
    if (const int code = pthread_mutex_trylock(&this->mutex_);
            code != 0) {
        if (code == EBUSY)
            return false;

        throw sysError();
    }
    return true;
}

inline void original::pMutex::unlock() {
    if (const int code = pthread_mutex_unlock(&this->mutex_);
        code != 0){
        throw sysError();
    }
}

inline original::pMutex::~pMutex() {
    if (const int code = pthread_mutex_destroy(&this->mutex_);
        code != 0){
        std::terminate();
    }
}

inline original::scopeLock::scopeLock(pMutex& p_mutex, bool try_lock)
    : p_mutex_(p_mutex), is_locked(false) {
    if (try_lock){
        this->is_locked = this->p_mutex_.tryLock();
    } else{
        this->p_mutex_.lock();
        this->is_locked = true;
    }
}

inline bool original::scopeLock::isLocked() const {
    return this->is_locked;
}

inline original::scopeLock::~scopeLock() {
    if (this->is_locked)
        this->p_mutex_.unlock();
}

#endif //MUTEX_H
