#ifndef ORIGINAL_ATOMIC_H
#define ORIGINAL_ATOMIC_H
#include <cstring>
#include "config.h"
#include "mutex.h"

namespace original {
#ifdef ORIGINAL_COMPILER_GCC
    template<typename TYPE>
    class atomic {
        static constexpr bool USE_LOCK_FREE = __atomic_always_lock_free(sizeof(TYPE), nullptr);
        #define ORIGINAL_USE_LOCK !USE_LOCK_FREE

        enum class memOrder {
            RELAXED = __ATOMIC_RELAXED,
            ACQUIRE = __ATOMIC_ACQUIRE,
            RELEASE = __ATOMIC_RELEASE,
            ACQ_REL = __ATOMIC_ACQ_REL,
            SEQ_CST = __ATOMIC_SEQ_CST,
        };

    #ifdef ORIGINAL_USE_LOCK
        mutable pMutex mutex_;
    #endif
        alignas(TYPE) byte data_[sizeof(TYPE)];

        public:
            static constexpr auto RELAXED = memOrder::RELAXED;
            static constexpr auto ACQUIRE = memOrder::ACQUIRE;
            static constexpr auto RELEASE = memOrder::RELEASE;
            static constexpr auto ACQ_REL = memOrder::ACQ_REL;
            static constexpr auto SEQ_CST = memOrder::SEQ_CST;

            atomic(const atomic&) noexcept = delete;

            atomic& operator=(const atomic&) noexcept = delete;

            atomic(atomic&&) noexcept = delete;

            atomic& operator=(atomic&&) noexcept = delete;

            explicit atomic();

            explicit atomic(TYPE value, memOrder order = RELEASE);

            static constexpr bool isLockFree() noexcept;

            void store(TYPE value, memOrder order = RELEASE);

            TYPE load(memOrder order = ACQUIRE) const noexcept;

            TYPE exchange(const TYPE& value, memOrder order = ACQUIRE) noexcept;

            ~atomic() = default;
    };
}

template <typename TYPE>
original::atomic<TYPE>::atomic() : data_{} {}

template <typename TYPE>
original::atomic<TYPE>::atomic(TYPE value, memOrder order) : atomic()
{
    if constexpr (ORIGINAL_USE_LOCK) {
        uniqueLock lock{this->mutex_};
        std::memcpy(this->data_, &value, sizeof(TYPE));
    } else {
        __atomic_store(this->data_, &value, static_cast<integer>(order));
    }
}

template <typename TYPE>
constexpr bool original::atomic<TYPE>::isLockFree() noexcept
{
    return USE_LOCK_FREE;
}

template <typename TYPE>
void original::atomic<TYPE>::store(TYPE value, memOrder order)
{
    if constexpr (ORIGINAL_USE_LOCK) {
        uniqueLock lock{this->mutex_};
        std::memcpy(this->data_, &value, sizeof(TYPE));
    } else {
        __atomic_store(this->data_, &value, static_cast<integer>(order));
    }
}

template <typename TYPE>
TYPE original::atomic<TYPE>::load(memOrder order) const noexcept
{
    TYPE result;
    if constexpr (ORIGINAL_USE_LOCK) {
        uniqueLock lock{this->mutex_};
        std::memcpy(&result, this->data_, sizeof(TYPE));
        return result;
    } else {
        __atomic_load(this->data_, &result, static_cast<integer>(order));
        return result;
    }
}

template <typename TYPE>
TYPE original::atomic<TYPE>::exchange(const TYPE& value, memOrder order) noexcept
{
    TYPE result;
    if constexpr (ORIGINAL_USE_LOCK) {
        uniqueLock lock{this->mutex_};
        std::memcpy(&result, this->data_, sizeof(TYPE));
        std::memcpy(this->data_, &value, sizeof(TYPE));
        return result;
    } else {
        __atomic_exchange(this->data_, &value, &result, static_cast<integer>(order));
        return result;
    }
}

#endif
#endif //ORIGINAL_ATOMIC_H
