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

    class uniqueLock {
        pMutex& p_mutex_;
        bool is_locked;

        enum class lockPolicy {
            MANUAL_LOCK,
            AUTO_LOCK,
            TRY_LOCK,
        };
    public:
        static constexpr lockPolicy MANUAL_LOCK = lockPolicy::MANUAL_LOCK;
        static constexpr lockPolicy AUTO_LOCK = lockPolicy::AUTO_LOCK;
        static constexpr lockPolicy TRY_LOCK = lockPolicy::TRY_LOCK;

        explicit uniqueLock(pMutex& p_mutex, lockPolicy policy = AUTO_LOCK);

        uniqueLock(const uniqueLock&) = delete;

        uniqueLock& operator=(const uniqueLock&) = delete;

        uniqueLock(uniqueLock&&) = delete;

        uniqueLock& operator=(uniqueLock&&) = delete;

        [[nodiscard]] bool isLocked() const noexcept;

        void lock();

        bool tryLock();

        void unlock();

        ~uniqueLock();
    };

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

inline original::uniqueLock::uniqueLock(pMutex& p_mutex, lockPolicy policy)
    : p_mutex_(p_mutex), is_locked(false) {
    switch (policy) {
        case MANUAL_LOCK:
            break;
        case AUTO_LOCK:
            this->lock();
            break;
        case TRY_LOCK:
            this->tryLock();
    }
}

inline bool original::uniqueLock::isLocked() const noexcept {
    return this->is_locked;
}

void original::uniqueLock::lock() {
    if (this->is_locked)
        throw sysError();

    this->p_mutex_.lock();
    this->is_locked = true;
}

bool original::uniqueLock::tryLock() {
    if (this->is_locked)
        throw sysError();

    this->is_locked = this->p_mutex_.tryLock();
    return this->is_locked;
}

void original::uniqueLock::unlock() {
    if (this->is_locked){
        this->p_mutex_.unlock();
        this->is_locked = false;
    }
}

inline original::uniqueLock::~uniqueLock() {
    this->unlock();
}

}

#endif //MUTEX_H
