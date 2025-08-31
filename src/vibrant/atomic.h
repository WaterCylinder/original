#ifndef ORIGINAL_ATOMIC_H
#define ORIGINAL_ATOMIC_H

#include <type_traits>
#include <cstring>
#include "optional.h"
#include "config.h"
#include "mutex.h"

namespace original {
#ifdef ORIGINAL_COMPILER_GCC

    enum class memOrder {
        RELAXED = __ATOMIC_RELAXED,
        ACQUIRE = __ATOMIC_ACQUIRE,
        RELEASE = __ATOMIC_RELEASE,
        ACQ_REL = __ATOMIC_ACQ_REL,
        SEQ_CST = __ATOMIC_SEQ_CST,
    };

    template<typename TYPE, bool USE_LOCK>
    class atomicImpl;

    template<typename TYPE>
    class atomicImpl<TYPE, false> {
        alignas(TYPE) unsigned char data_[sizeof(TYPE)];

    public:
        static constexpr auto RELAXED = memOrder::RELAXED;
        static constexpr auto ACQUIRE = memOrder::ACQUIRE;
        static constexpr auto RELEASE = memOrder::RELEASE;
        static constexpr auto ACQ_REL = memOrder::ACQ_REL;
        static constexpr auto SEQ_CST = memOrder::SEQ_CST;

        atomicImpl();

        explicit atomicImpl(TYPE value, memOrder order = RELEASE);

        static constexpr bool isLockFree() noexcept;

        void store(TYPE value, memOrder order = RELEASE);

        TYPE load(memOrder order = ACQUIRE) const noexcept;

        TYPE exchange(const TYPE& value, memOrder order = ACQ_REL) noexcept;

        ~atomicImpl() = default;
    };

    template<typename TYPE>
    class atomicImpl<TYPE, true> {
        mutable pMutex mutex_;
        alternative<TYPE> data_;

    public:
        static constexpr auto RELAXED = memOrder::RELAXED;
        static constexpr auto ACQUIRE = memOrder::ACQUIRE;
        static constexpr auto RELEASE = memOrder::RELEASE;
        static constexpr auto ACQ_REL = memOrder::ACQ_REL;
        static constexpr auto SEQ_CST = memOrder::SEQ_CST;

        atomicImpl() = default;

        explicit atomicImpl(TYPE value, memOrder = RELEASE);

        static constexpr bool isLockFree() noexcept;

        void store(TYPE value, memOrder = RELEASE);

        TYPE load(memOrder = ACQUIRE) const noexcept;

        TYPE exchange(const TYPE& value, memOrder = ACQ_REL) noexcept;

        ~atomicImpl() = default;
    };

    template<typename TYPE>
    auto atomic();

    template<typename TYPE>
    auto atomic(TYPE value);
}

template <typename TYPE>
original::atomicImpl<TYPE, false>::atomicImpl() {
    std::memset(this->data_, 0, sizeof(TYPE));
}

template <typename TYPE>
original::atomicImpl<TYPE, false>::atomicImpl(TYPE value, memOrder order) {
    __atomic_store(reinterpret_cast<TYPE*>(this->data_), &value, static_cast<int>(order));
}

template <typename TYPE>
constexpr bool original::atomicImpl<TYPE, false>::isLockFree() noexcept {
    return true;
}

template <typename TYPE>
void original::atomicImpl<TYPE, false>::store(TYPE value, memOrder order) {
    __atomic_store(reinterpret_cast<TYPE*>(this->data_), &value, static_cast<int>(order));
}

template <typename TYPE>
TYPE original::atomicImpl<TYPE, false>::load(memOrder order) const noexcept {
    TYPE result;
    __atomic_load(reinterpret_cast<const TYPE*>(this->data_), &result, static_cast<int>(order));
    return result;
}

template <typename TYPE>
TYPE original::atomicImpl<TYPE, false>::exchange(const TYPE& value, memOrder order) noexcept {
    TYPE result;
    __atomic_exchange(reinterpret_cast<TYPE*>(this->data_), const_cast<TYPE*>(&value),
                      &result, static_cast<int>(order));
    return result;
}

template <typename TYPE>
original::atomicImpl<TYPE, true>::atomicImpl(TYPE value, memOrder) {
    uniqueLock lock{this->mutex_};
    this->data_.set(value);
}

template <typename TYPE>
constexpr bool original::atomicImpl<TYPE, true>::isLockFree() noexcept {
    return true;
}

template <typename TYPE>
void original::atomicImpl<TYPE, true>::store(TYPE value, memOrder) {
    uniqueLock lock{this->mutex_};
    this->data_.set(value);
}

template <typename TYPE>
TYPE original::atomicImpl<TYPE, true>::load(memOrder) const noexcept {
    uniqueLock lock{this->mutex_};
    return *this->data_;
}

template <typename TYPE>
TYPE original::atomicImpl<TYPE, true>::exchange(const TYPE& value, memOrder) noexcept {
    uniqueLock lock{this->mutex_};
    TYPE result = *this->data_;
    this->data_.set(value);
    return result;
}

template <typename TYPE>
auto original::atomic()
{
    return atomicImpl<
        TYPE,
        !( std::is_trivially_copyable_v<TYPE> &&
           std::is_trivially_destructible_v<TYPE> &&
           __atomic_always_lock_free(sizeof(TYPE), nullptr) )
    >{};
}

template <typename TYPE>
auto original::atomic(TYPE value)
{
    return atomicImpl<
        TYPE,
        !( std::is_trivially_copyable_v<TYPE> &&
           std::is_trivially_destructible_v<TYPE> &&
           __atomic_always_lock_free(sizeof(TYPE), nullptr) )
    >{std::move(value)};
}
#endif
#endif
