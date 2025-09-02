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
    using atomic = atomicImpl<
        TYPE,
        !( std::is_trivially_copyable_v<TYPE> &&
           std::is_trivially_destructible_v<TYPE> &&
           __atomic_always_lock_free(sizeof(TYPE), nullptr) )
    >;

    template<typename TYPE>
    class atomicImpl<TYPE, false> {
        alignas(TYPE) byte data_[sizeof(TYPE)];

        atomicImpl();

        explicit atomicImpl(TYPE value, memOrder order = SEQ_CST);
    public:
        static constexpr auto RELAXED = memOrder::RELAXED;
        static constexpr auto ACQUIRE = memOrder::ACQUIRE;
        static constexpr auto RELEASE = memOrder::RELEASE;
        static constexpr auto ACQ_REL = memOrder::ACQ_REL;
        static constexpr auto SEQ_CST = memOrder::SEQ_CST;

        static constexpr bool isLockFree() noexcept;

        void store(TYPE value, memOrder order = SEQ_CST);

        TYPE load(memOrder order = SEQ_CST) const noexcept;

        TYPE exchange(TYPE value, memOrder order = SEQ_CST) noexcept;

        bool exchangeCmp(TYPE& expected, TYPE desired, memOrder order = SEQ_CST) noexcept;

        ~atomicImpl() = default;

        template<typename T>
        friend auto makeAtomic();

        template<typename T>
        friend auto makeAtomic(T value);
    };

    template<typename TYPE>
    class atomicImpl<TYPE, true> {
        mutable pMutex mutex_;
        alternative<TYPE> data_;

        atomicImpl() = default;

        explicit atomicImpl(TYPE value, memOrder = RELEASE);
    public:
        static constexpr auto RELAXED = memOrder::RELAXED;
        static constexpr auto ACQUIRE = memOrder::ACQUIRE;
        static constexpr auto RELEASE = memOrder::RELEASE;
        static constexpr auto ACQ_REL = memOrder::ACQ_REL;
        static constexpr auto SEQ_CST = memOrder::SEQ_CST;

        static constexpr bool isLockFree() noexcept;

        void store(TYPE value, memOrder = SEQ_CST);

        TYPE load(memOrder = SEQ_CST) const noexcept;

        TYPE exchange(const TYPE& value, memOrder = SEQ_CST) noexcept;

        bool exchangeCmp(TYPE& expected, const TYPE& desired, memOrder order = SEQ_CST) noexcept;

        ~atomicImpl() = default;

        template<typename T>
        friend auto makeAtomic();

        template<typename T>
        friend auto makeAtomic(T value);
    };

    template<typename TYPE>
    auto makeAtomic();

    template<typename TYPE>
    auto makeAtomic(TYPE value);
}

template <typename TYPE>
original::atomicImpl<TYPE, false>::atomicImpl() {
    std::memset(this->data_, byte{}, sizeof(TYPE));
}

template <typename TYPE>
original::atomicImpl<TYPE, false>::atomicImpl(TYPE value, memOrder order) {
    __atomic_store(reinterpret_cast<TYPE*>(this->data_), &value, static_cast<integer>(order));
}

template <typename TYPE>
constexpr bool original::atomicImpl<TYPE, false>::isLockFree() noexcept {
    return true;
}

template <typename TYPE>
void original::atomicImpl<TYPE, false>::store(TYPE value, memOrder order) {
    __atomic_store(reinterpret_cast<TYPE*>(this->data_), &value, static_cast<integer>(order));
}

template <typename TYPE>
TYPE original::atomicImpl<TYPE, false>::load(memOrder order) const noexcept {
    TYPE result;
    __atomic_load(reinterpret_cast<const TYPE*>(this->data_), &result, static_cast<integer>(order));
    return result;
}

template <typename TYPE>
TYPE original::atomicImpl<TYPE, false>::exchange(TYPE value, memOrder order) noexcept {
    TYPE result;
    __atomic_exchange(reinterpret_cast<TYPE*>(this->data_), &value,
                      &result, static_cast<integer>(order));
    return result;
}

template <typename TYPE>
bool original::atomicImpl<TYPE, false>::exchangeCmp(TYPE& expected, TYPE desired, memOrder order) noexcept
{
    return __atomic_compare_exchange_n(reinterpret_cast<TYPE*>(this->data_),
                                       &expected, std::move(desired), false,
                     static_cast<integer>(order), static_cast<integer>(order));
}

template <typename TYPE>
original::atomicImpl<TYPE, true>::atomicImpl(TYPE value, memOrder) {
    uniqueLock lock{this->mutex_};
    this->data_.set(value);
}

template <typename TYPE>
constexpr bool original::atomicImpl<TYPE, true>::isLockFree() noexcept {
    return false;
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
bool original::atomicImpl<TYPE, true>::exchangeCmp(TYPE& expected, const TYPE& desired, memOrder) noexcept {
    uniqueLock lock{this->mutex_};
    if (*this->data_ == expected) {
        this->data_.set(desired);
        return true;
    }
    expected = *this->data_;
    return false;
}

template<typename TYPE>
auto original::makeAtomic()
{
    return atomic<TYPE>{};
}

template<typename TYPE>
auto original::makeAtomic(TYPE value)
{
    return atomic<TYPE>{std::move(value)};
}
#endif
#endif
