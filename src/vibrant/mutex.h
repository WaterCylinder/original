#ifndef MUTEX_H
#define MUTEX_H

#include <cstring>
#include "pthread.h"
#include "error.h"
#include "tuple.h"


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

    class lockGuard {
    protected:
        virtual void lock() = 0;

        virtual bool tryLock() = 0;

        virtual void unlock() = 0;

        [[nodiscard]] virtual bool isLocked() const noexcept = 0;
    public:
        enum class lockPolicy {
            MANUAL_LOCK,
            AUTO_LOCK,
            TRY_LOCK,
            ADOPT_LOCK,
        };

        static constexpr auto MANUAL_LOCK = lockPolicy::MANUAL_LOCK;
        static constexpr auto AUTO_LOCK = lockPolicy::AUTO_LOCK;
        static constexpr auto TRY_LOCK = lockPolicy::TRY_LOCK;
        static constexpr auto ADOPT_LOCK = lockPolicy::ADOPT_LOCK;

        explicit lockGuard() = default;

        lockGuard(const lockGuard&) = delete;

        lockGuard& operator=(const lockGuard&) = delete;

        virtual ~lockGuard() = default;
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

    class uniqueLock final : public lockGuard {
        pMutex& p_mutex_;
        bool is_locked;

    public:
        explicit uniqueLock(pMutex& p_mutex, lockPolicy policy = AUTO_LOCK);

        uniqueLock(uniqueLock&&) = delete;

        uniqueLock& operator=(uniqueLock&&) = delete;

        [[nodiscard]] bool isLocked() const noexcept override;

        void lock() override;

        bool tryLock() override;

        void unlock() override;

        ~uniqueLock() override;
    };

    template<typename... MUTEX>
    class multiLock final : public lockGuard {
        tuple<MUTEX* ...> m_list;
        bool is_locked_all;

        template<u_integer... IDXES>
        void lockAll(indexSequence<IDXES...> sequence);

        template<u_integer... IDXES>
        bool tryLockAll(indexSequence<IDXES...> sequence);

        template<u_integer... IDXES>
        void unlockAll(indexSequence<IDXES...> sequence);
    public:
        explicit multiLock(MUTEX&... mutex);

        multiLock(lockPolicy policy, MUTEX&... mutex);

        [[nodiscard]] bool isLocked() const noexcept override;

        void lock() override;

        bool tryLock() override;

        void unlock() override;

        multiLock(multiLock&&) = delete;

        multiLock& operator=(multiLock&&) = delete;

        ~multiLock() override;
    };
}

inline original::pMutex::pMutex() : mutex_{} {
    if (const int code = pthread_mutex_init(&this->mutex_, nullptr);
        code != 0){
        throw sysError();
    }
}

inline original::ul_integer original::pMutex::id() const {
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
            break;
        case ADOPT_LOCK:
            this->is_locked = true;
    }
}

inline bool original::uniqueLock::isLocked() const noexcept {
    return this->is_locked;
}

inline void original::uniqueLock::lock() {
    if (this->is_locked)
        throw sysError();

    this->p_mutex_.lock();
    this->is_locked = true;
}

inline bool original::uniqueLock::tryLock() {
    if (this->is_locked)
        throw sysError();

    this->is_locked = this->p_mutex_.tryLock();
    return this->is_locked;
}

inline void original::uniqueLock::unlock() {
    if (this->is_locked){
        this->p_mutex_.unlock();
        this->is_locked = false;
    }
}

inline original::uniqueLock::~uniqueLock() {
    this->unlock();
}

template<typename... MUTEX>
template<original::u_integer... IDXES>
void original::multiLock<MUTEX...>::lockAll(indexSequence<IDXES...>) {
    (..., this->m_list.template get<IDXES>()->lock());
    this->is_locked_all = true;
}

template<typename... MUTEX>
template<original::u_integer... IDXES>
bool original::multiLock<MUTEX...>::tryLockAll(indexSequence<IDXES...>) {
    bool success = true;
    bool lock_status[sizeof...(IDXES)] = {};
    auto tryLockStatusUpdate = [&](auto i, auto* mutex) {
        if (mutex->tryLock()) {
            lock_status[i] = true;
        } else {
            success = false;
        }
    };

    u_integer idx = 0;
    ((tryLockStatusUpdate(idx++, this->m_list.template get<IDXES>())), ...);

    if (!success) {
        idx = 0;
        ((lock_status[idx]
            ? this->m_list.template get<IDXES>()->unlock() : void(), ++idx), ...);
    }
    this->is_locked_all = success;
    return success;
}

template<typename... MUTEX>
template<original::u_integer... IDXES>
void original::multiLock<MUTEX...>::unlockAll(indexSequence<IDXES...>) {
    (..., this->m_list.template get<IDXES>()->unlock());
}

template<typename... MUTEX>
original::multiLock<MUTEX...>::multiLock(MUTEX&... mutex)
    : multiLock(AUTO_LOCK, mutex...) {}

template<typename... MUTEX>
original::multiLock<MUTEX...>::multiLock(lockPolicy policy, MUTEX&... mutex)
    : m_list(&mutex...), is_locked_all(false) {
    switch (policy) {
        case MANUAL_LOCK:
            break;
        case AUTO_LOCK:
            this->lock();
            break;
        case TRY_LOCK:
            this->tryLock();
            break;
        case ADOPT_LOCK:
            this->is_locked_all = true;
    }
}

template<typename... MUTEX>
bool original::multiLock<MUTEX...>::isLocked() const noexcept {
    return this->is_locked_all;
}

template<typename... MUTEX>
void original::multiLock<MUTEX...>::lock() {
    if (this->is_locked_all)
        throw sysError();

    this->lockAll(makeSequence<sizeof...(MUTEX)>());
}

template<typename... MUTEX>
bool original::multiLock<MUTEX...>::tryLock() {
    if (this->is_locked_all)
        throw sysError();

    return this->tryLockAll(makeSequence<sizeof...(MUTEX)>());
}

template<typename... MUTEX>
void original::multiLock<MUTEX...>::unlock() {
    if (this->is_locked_all)
        this->unlockAll(makeReverseSequence<sizeof...(MUTEX)>());
}

template<typename... MUTEX>
original::multiLock<MUTEX...>::~multiLock(){
    this->unlock();
}

#endif //MUTEX_H
