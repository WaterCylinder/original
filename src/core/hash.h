#ifndef HASH_H
#define HASH_H


#include "config.h"
#include <cstring>
#include <string>


namespace original {
    template <typename TYPE>
    class hash;

    template <typename DERIVED>
    class hashable;

    template <typename DERIVED>
    concept isHashable =
        requires(const DERIVED& t, const DERIVED& other) {
            { t.toHash() } -> std::same_as<u_integer>;
            { t.equals(other) } -> std::same_as<bool>;
        };


    template <typename TYPE>
    class hash {
    public:
        static constexpr u_integer FNV_OFFSET_BASIS = 0x811C9DC5;

        static constexpr u_integer FNV_32_PRIME = 0x01000193;

        static u_integer fnv1a(const byte* data, u_integer size) noexcept;

        template <typename T>
        static u_integer hashFunc(const T& t) noexcept;

        template <isHashable T>
        static u_integer hashFunc(const T& t) noexcept;

        template<std::integral T>
        static u_integer hashFunc(const T& t) noexcept;

        template <typename T>
        static u_integer hashFunc(T* const& t) noexcept;

        static u_integer hashFunc(const char& t) noexcept;

        static u_integer hashFunc(const char* str) noexcept;

        static u_integer hashFunc(const std::string& str) noexcept;

        u_integer operator()(const TYPE& t) const noexcept;
    };

    template <typename DERIVED>
    class hashable {
    public:
        [[nodiscard]] virtual u_integer toHash() const noexcept;

        virtual bool equals(const DERIVED& other) const noexcept;

        virtual ~hashable() = 0;
    };
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::fnv1a(const byte* data, u_integer size) noexcept {
    u_integer hash = FNV_OFFSET_BASIS;
    for (u_integer i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= FNV_32_PRIME;
    }
    return hash;
}

template<typename TYPE>
template<typename T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    if constexpr (std::is_trivially_copyable_v<T>) {
        byte buffer[sizeof(T)];
        std::memcpy(buffer, &t, sizeof(T));
        return fnv1a(buffer, sizeof(T));
    } else {
        return static_cast<u_integer>(&t);
    }
}

template<typename TYPE>
template<original::isHashable T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    return t.toHash();
}

template<typename TYPE>
template<std::integral T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    return static_cast<u_integer>(t);
}

template<typename TYPE>
template<typename T>
original::u_integer original::hash<TYPE>::hashFunc(T* const& t) noexcept {
    return reinterpret_cast<u_integer>(t);
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::hashFunc(const char &t) noexcept {
    return static_cast<u_integer>(t);
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::hashFunc(const char* str) noexcept {
    if (str == nullptr) return 0;
    return fnv1a(reinterpret_cast<const byte*>(str), std::strlen(str));
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::hashFunc(const std::string& str) noexcept {
    return fnv1a(reinterpret_cast<const byte*>(str.data()), str.size());
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::operator()(const TYPE &t) const noexcept {
    return hashFunc(t);
}

template <typename DERIVED>
original::u_integer original::hashable<DERIVED>::toHash() const noexcept {
    return hash<DERIVED>()(static_cast<const DERIVED&>(*this));
}

template<typename DERIVED>
bool original::hashable<DERIVED>::equals(const DERIVED &other) const noexcept {
    return static_cast<const DERIVED&>(*this) == other;
}

template <typename DERIVED>
original::hashable<DERIVED>::~hashable() = default;

#endif //HASH_H
