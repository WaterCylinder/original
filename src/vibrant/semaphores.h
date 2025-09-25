#ifndef ORIGINAL_SEMAPHORES_H
#define ORIGINAL_SEMAPHORES_H
#include "config.h"
#include "error.h"
#include "mutex.h"
#include "condition.h"


namespace original {
    template<u_integer = 1>
    class semaphore {
        u_integer count_;
        pMutex mutex_;
        pCondition condition_;

    public:
        semaphore();

        explicit semaphore(u_integer init_count);

        void acquire();

        bool tryAcquire();

        bool acquireFor(time::duration timeout);

        void release(u_integer increase = 1);

        bool tryRelease(u_integer increase = 1);

        bool releaseFor(u_integer increase, time::duration timeout);

        bool releaseFor(const time::duration& timeout);
    };

    template<>
    class semaphore<0> {
        u_integer count_;
        pMutex mutex_;
        pCondition condition_;

    public:
        semaphore();

        explicit semaphore(u_integer init_count);

        void acquire();

        bool tryAcquire();

        bool acquireFor(const time::duration& timeout);

        void release(u_integer increase = 1);
    };

    template<u_integer MAX_CNT>
    class semaphoreGuard {
        semaphore<MAX_CNT>& semaphore_;

    public:
        explicit semaphoreGuard(semaphore<MAX_CNT>& sem);

        ~semaphoreGuard();
    };
}

template<original::u_integer MAX_CNT>
original::semaphore<MAX_CNT>::semaphore() : count_(MAX_CNT) {}

template<original::u_integer MAX_CNT>
original::semaphore<MAX_CNT>::semaphore(const u_integer init_count) : count_(init_count) {
    if (init_count > MAX_CNT) {
        throw valueError("Init count is " + printable::formatString(init_count) +
                         ", that is larger than the max count " + printable::formatString(MAX_CNT));
    }
}

template<original::u_integer MAX_CNT>
void original::semaphore<MAX_CNT>::acquire() {
    uniqueLock lock{this->mutex_};
    this->condition_.wait(this->mutex_, [this]{
        return this->count_ > 0;
    });
    this->count_ -= 1;
}

template<original::u_integer MAX_CNT>
bool original::semaphore<MAX_CNT>::tryAcquire() {
    uniqueLock lock{this->mutex_};
    if (this->count_ == 0) {
        return false;
    }
    this->count_ -= 1;
    return true;
}

template<original::u_integer MAX_CNT>
bool original::semaphore<MAX_CNT>::acquireFor(time::duration timeout) {
    uniqueLock lock{this->mutex_};
    bool success = this->condition_.waitFor(this->mutex_, timeout, [this]{
        return this->count_ > 0;
    });
    if (success){
        this->count_ -= 1;
    }
    return success;
}

template<original::u_integer MAX_CNT>
void original::semaphore<MAX_CNT>::release(u_integer increase) {
void original::semaphore<MAX_CNT>::release(const u_integer increase) {
    {
        uniqueLock lock{this->mutex_};
        if (increase > MAX_CNT) {
            throw valueError("Increase is larger than max count " + printable::formatString(MAX_CNT));
        }
        this->condition_.wait(this->mutex_, [this, increase] {
            return this->count_ + increase <= MAX_CNT;
        });
        this->count_ += increase;
    }
    this->condition_.notifySome(increase);
}

template<original::u_integer MAX_CNT>
bool original::semaphore<MAX_CNT>::tryRelease(original::u_integer increase) {
    uniqueLock lock{this->mutex_};
    if (this->count_ + increase > MAX_CNT) {
        return false;
    }
    this->count_ += increase;
bool original::semaphore<MAX_CNT>::tryRelease(const u_integer increase) {
    return true;
}

template<original::u_integer MAX_CNT>
bool original::semaphore<MAX_CNT>::releaseFor(const u_integer increase, time::duration timeout){
    bool success;
    {
        uniqueLock lock{this->mutex_};
        success = this->condition_.waitFor(this->mutex_, timeout, [this, increase]{
            return this->count_ + increase <= MAX_CNT;
        });
        if (success){
            this->count_ += increase;
        }
    }
    if (success){
        this->condition_.notifySome(increase);
    }
    return success;
}

template<original::u_integer MAX_CNT>
bool original::semaphore<MAX_CNT>::releaseFor(const time::duration& timeout) {
    return this->releaseFor(1, timeout);
}

inline original::semaphore<0>::semaphore() : count_(0) {}

inline original::semaphore<0>::semaphore(const u_integer init_count) : count_(init_count) {}

inline void original::semaphore<0>::acquire() {
    uniqueLock lock{this->mutex_};
    this->condition_.wait(this->mutex_, [this]{
        return this->count_ > 0;
    });
    this->count_ -= 1;
    this->condition_.notify();
}

inline bool original::semaphore<0>::tryAcquire() {
    uniqueLock lock{this->mutex_};
    if (this->count_ == 0) {
        return false;
    }
    this->count_ -= 1;
    return true;
}

inline bool original::semaphore<0>::acquireFor(const time::duration& timeout) {
    uniqueLock lock{this->mutex_};
    const bool success = this->condition_.waitFor(this->mutex_, timeout, [this]{
        return this->count_ > 0;
    });
    if (success){
        this->count_ -= 1;
    }
    return success;
}

inline void original::semaphore<0>::release(const u_integer increase) {
    {
        uniqueLock lock{this->mutex_};
        this->count_ += increase;
    }
    this->condition_.notifySome(increase);
}

template<original::u_integer MAX_CNT>
original::semaphoreGuard<MAX_CNT>::semaphoreGuard(semaphore<MAX_CNT>& sem) : semaphore_(sem) {
    this->semaphore_.acquire();
}

template<original::u_integer MAX_CNT>
original::semaphoreGuard<MAX_CNT>::~semaphoreGuard() {
    this->semaphore_.release();
}

#endif //ORIGINAL_SEMAPHORES_H
