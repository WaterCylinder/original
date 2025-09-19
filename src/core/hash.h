#ifndef HASH_H
#define HASH_H

#include "config.h"
#include <cstring>
#include <string>
#include "types.h"

/**
 * @file hash.h
 * @brief Provides a generic hashing utility and interface for hashable types.
 * @details This header defines:
 * - A comprehensive hash function object (hash) with specializations for common types
 * - A base interface (hashable) for user-defined hashable types
 * - Integration with std::hash for STL compatibility
 *
 * Features:
 * - Implements FNV-1a algorithm for byte data
 * - Specializations for integral types, pointers, strings, and custom types
 * - Fallback implementations for trivially copyable types
 * - Safe nullptr handling
 * - Hash combination utilities
 *
 * @note All hash functions are noexcept and provide basic hash distribution
 * @namespace original
 * @brief Main namespace containing all hashing utilities
 */

namespace original {

    /**
     * @brief Forward declaration of hashable interface template.
     * @tparam DERIVED The derived class that inherits from hashable.
     */
    template <typename DERIVED>
    class hashable;

    /**
     * @class hash
     * @brief Generic hash function object supporting multiple types.
     * @tparam TYPE The type for which the hash function object is defined
     * @details Provides:
     * - FNV-1a hash algorithm implementation
     * - Specialized hash functions for common types
     * - Fallback implementations for arbitrary types
     * - Consistent hashing interface via operator()
     *
     * Supported Types:
     * - Integral types (direct casting)
     * - Pointers (address-based hashing)
     * - Strings (FNV-1a over characters)
     * - Trivially copyable types (byte-wise hashing)
     * - Types implementing hashable interface
     *
     * Example Usage:
     * @code{.cpp}
     * original::hash<std::string> hasher;
     * u_integer h = hasher("hello world");
     * @endcode
     */
    template <typename TYPE>
    class hash {

        /**
         * @brief Internal implementation of the hash function
         * @tparam T The type of object to hash
         * @param t The object to hash
         * @return Computed hash value
         * @details This internal function provides the fundamental hashing strategy:
         * - For trivially copyable types: Uses byte-wise FNV-1a hashing
         * - For non-trivially copyable types: Falls back to address-based hashing
         *
         * @note This function is not meant to be called directly - use hashFunc() instead
         */
        template <typename T>
        static u_integer hashFuncImpl(const T& t) noexcept;

    public:
        template <typename>
        friend class hashable;

        /// @brief FNV-1a initial offset value (0x811C9DC5)
        static constexpr u_integer FNV_OFFSET_BASIS = 0x811C9DC5;

        /// @brief FNV-1a prime multiplier (0x01000193)
        static constexpr u_integer FNV_32_PRIME = 0x01000193;

        /**
         * @brief Combines a hash value with another value's hash
         * @tparam T Type of the value to combine
         * @param seed Current hash value (modified in-place)
         * @param value Value whose hash to combine
         */
        template <typename T>
        static void hashCombine(u_integer& seed, const T& value) noexcept;

        /**
         * @brief FNV-1a hash implementation for raw byte data
         * @param data Pointer to byte array
         * @param size Number of bytes to hash
         * @return Computed hash value
         * @note This is the fundamental hashing operation used by other specializations
         */
        static u_integer fnv1a(const byte* data, u_integer size) noexcept;

        /**
         * @brief Combines multiple hash values into one
         * @tparam T First value type
         * @tparam Rest Remaining value types
         * @param seed Current hash value (modified in-place)
         * @param value First value to combine
         * @param rest Remaining values to combine
         */
        template <typename T, typename... Rest>
        static void hashCombine(u_integer& seed, const T& value, const Rest&... rest) noexcept;

        /**
         * @brief Default hash function fallback
         * @tparam T The input type
         * @param t The value to be hashed
         * @return Hash value
         * @details For trivially copyable types: uses byte-wise FNV-1a
         *          For other types: falls back to address hashing
         */
        template <typename T>
        static u_integer hashFunc(const T& t) noexcept;

        template <typename T>
        requires ExtendsOf<hashable<T>, T>
        static u_integer hashFunc(const T& t) noexcept;

        /**
         * @brief Hash function specialization for integral types
         * @tparam T Integral type
         * @param t The value to hash
         * @return The cast hash value
         */
        template<std::integral T>
        static u_integer hashFunc(const T& t) noexcept;

        /**
         * @brief Hash function for pointers
         * @tparam T The pointee type
         * @param t Pointer to hash
         * @return Hash value based on address
         */
        template <typename T>
        static u_integer hashFunc(T* const& t) noexcept;

        /**
         * @brief Hash function for nullptr
         * @param null Null pointer
         * @return 0 as the hash value
         */
        static u_integer hashFunc(const std::nullptr_t& null) noexcept;

        /**
         * @brief Hash function for single character
         * @param t Character to hash
         * @return Integer value of the character
         */
        static u_integer hashFunc(const char& t) noexcept;

        /**
         * @brief Hash function for C-style strings
         * @param str Null-terminated string
         * @return Computed FNV-1a hash
         * @note Handles nullptr by returning 0
         */
        static u_integer hashFunc(const char* str) noexcept;

        /**
         * @brief Hash function for std::string
         * @param str String to hash
         * @return Computed FNV-1a hash
         */
        static u_integer hashFunc(const std::string& str) noexcept;

        /**
         * @brief Hash function object call operator
         * @param t The object to hash
         * @return Result of hashFunc
         * @note Provides consistent interface for use in STL containers
         */
        u_integer operator()(const TYPE& t) const noexcept;
    };

    /**
     * @class hashable
     * @brief Interface for user-defined hashable types
     * @tparam DERIVED The derived type (CRTP pattern)
     * @details Provides default implementations for:
     * - toHash(): Uses byte-wise hashing of object representation
     * - equals(): Uses operator== comparison
     *
     * Usage Example:
     * @code{.cpp}
     * class MyType : public hashable<MyType> {
     * public:
     *     bool operator==(const MyType&) const { return true; }
     *     // Optionally override toHash() and equals()
     * };
     * @endcode
     */
    template <typename DERIVED>
    class hashable {
    public:
        /**
         * @brief Computes the hash of the object
         * @return A hash value
         * @details Default implementation uses byte-wise hashing of object
         */
        [[nodiscard]] virtual u_integer toHash() const noexcept;

        /**
         * @brief Compares two objects for equality
         * @param other The object to compare with
         * @return True if equal
         * @details Default implementation uses operator==
         */
        virtual bool equals(const DERIVED& other) const noexcept;

        /**
         * @brief Virtual destructor
         * @note Required for proper polymorphic behavior
         */
        virtual ~hashable() = 0;
    };
}


/**
 * @brief std::hash specialization for derived types of hashable
 * @tparam T Type that extends hashable
 */
template <typename T>
requires original::ExtendsOf<original::hashable<T>, T>
struct std::hash<T> { // NOLINT
    /**
     * @brief Hash function operator for STL compatibility
     * @param t The object to hash
     * @return Hash value as std::size_t
     */
    std::size_t operator()(const T& t) const noexcept;
};

template<typename TYPE>
template<typename T>
void original::hash<TYPE>::hashCombine(u_integer &seed, const T& value) noexcept {
    seed ^= hash<T>::hashFunc(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename TYPE>
template<typename T>
original::u_integer original::hash<TYPE>::hashFuncImpl(const T &t) noexcept {
    if constexpr (std::is_trivially_copyable_v<T>) {
        byte buffer[sizeof(T)];
        std::memcpy(buffer, &t, sizeof(T));
        return fnv1a(buffer, sizeof(T));
    } else {
        return static_cast<u_integer>(reinterpret_cast<uintptr_t>(&t));
    }
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::fnv1a(const byte* data, const u_integer size) noexcept {
    u_integer hash = FNV_OFFSET_BASIS;
    for (u_integer i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= FNV_32_PRIME;
    }
    return hash;
}

template<typename TYPE>
template<typename T, typename... Rest>
void original::hash<TYPE>::hashCombine(u_integer &seed, const T& value, const Rest&... rest) noexcept {
    hashCombine(seed, value);
    (hashCombine(seed, rest), ...);
}

template<typename TYPE>
template<typename T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    return hashFuncImpl(t);
}

template <typename TYPE>
template <typename T> requires original::ExtendsOf<original::hashable<T>, T>
original::u_integer original::hash<TYPE>::hashFunc(const T& t) noexcept {
    return t.toHash();
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::hashFunc(const std::nullptr_t&) noexcept {
    return 0;
}

template<typename TYPE>
template<std::integral T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    return static_cast<u_integer>(t);
}

template<typename TYPE>
template<typename T>
original::u_integer original::hash<TYPE>::hashFunc(T* const& t) noexcept {
    return static_cast<u_integer>(reinterpret_cast<uintptr_t>(t));
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
    return hash<DERIVED>::hashFuncImpl(static_cast<const DERIVED&>(*this));
}

template<typename DERIVED>
bool original::hashable<DERIVED>::equals(const DERIVED &other) const noexcept {
    return static_cast<const DERIVED&>(*this) == other;
}

template <typename DERIVED>
original::hashable<DERIVED>::~hashable() = default;

template <typename T>
requires original::ExtendsOf<original::hashable<T>, T>
std::size_t std::hash<T>::operator()(const T &t) const noexcept {
    return static_cast<std::size_t>(t.toHash());
}

#endif //HASH_H
