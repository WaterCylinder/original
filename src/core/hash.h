#ifndef HASH_H
#define HASH_H

#include "config.h"
#include <cstring>
#include <string>


/**
 * @file hash.h
 * @brief Provides a generic hashing utility and a base interface for hashable types.
 * @details This header defines:
 * - A comprehensive hash function object (hash) with specializations for common types
 * - A base interface (hashable) for user-defined hashable types
 * - Concept checking for hashable types (isHashable)
 *
 * Hash Function Features:
 * - FNV-1a algorithm implementation for byte data
 * - Specializations for integral types, pointers, strings, and custom types
 * - Fallback implementations for trivially copyable types
 * - Support for nullptr handling
 *
 * @note All hash functions are noexcept and provide basic hash distribution
 */

#include "config.h"
#include <cstring>
#include <string>

namespace original {

    /**
     * @brief Forward declaration of hash class template.
     * @tparam TYPE The type for which the hash function object is defined.
     */
    template <typename TYPE>
    class hash;

    /**
     * @brief Forward declaration of hashable interface template.
     * @tparam DERIVED The derived class that inherits from hashable.
     */
    template <typename DERIVED>
    class hashable;

    /**
     * @concept isHashable
     * @tparam DERIVED The type to be checked
     * @brief Concept checking for hashable types
     * @details Requires:
     * - toHash() method returning u_integer
     * - equals() method taking const DERIVED& and returning bool
     */
    template <typename DERIVED>
    concept isHashable =
    requires(const DERIVED& t, const DERIVED& other) {
        { t.toHash() } -> std::same_as<u_integer>;
        { t.equals(other) } -> std::same_as<bool>;
    };

    /**
     * @class hash
     * @tparam TYPE The type for which the hash function object is defined
     * @brief Generic hash function object supporting multiple types
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
     */
    template <typename TYPE>
    class hash {

        template <typename T>
        static inline void hashCombine(u_integer& seed, const T& value) noexcept;

        /**
         * @brief Internal implementation of the hash function
         * @tparam T The type of object to hash
         * @param t The object to hash
         * @return Computed hash value
         * @details This internal function provides the fundamental hashing strategy:
         * - For trivially copyable types: Uses byte-wise FNV-1a hashing of the object's memory representation
         * - For non-trivially copyable types: Falls back to address-based hashing
         *
         * Implementation Notes:
         * 1. Checks if type is trivially copyable at compile time
         * 2. For trivially copyable types:
         *    - Copies object bytes to temporary buffer
         *    - Applies FNV-1a algorithm to the byte sequence
         * 3. For other types:
         *    - Uses the object's address as hash value
         *
         * @note This function is not meant to be called directly - use hashFunc() instead
         * @see hashFunc For the public interface to hashing functionality
         */
        template <typename T>
        static u_integer hashFuncImpl(const T& t) noexcept;

    public:
        template <typename>
        friend class hashable;

        /// FNV-1a initial offset value
        static constexpr u_integer FNV_OFFSET_BASIS = 0x811C9DC5;

        /// FNV-1a prime multiplier
        static constexpr u_integer FNV_32_PRIME = 0x01000193;

        /**
         * @brief FNV-1a hash implementation for raw byte data
         * @param data Pointer to byte array
         * @param size Number of bytes to hash
         * @return Computed hash value
         * @note This is the fundamental hashing operation used by other specializations
         */
        static u_integer fnv1a(const byte* data, u_integer size) noexcept;

        template <typename T, typename... Rest>
        static inline void hashCombine(u_integer& seed, const T& value, const Rest&... rest) noexcept;

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

        /**
         * @brief Hash function for types satisfying the isHashable concept
         * @tparam T A hashable type
         * @param t The object to hash
         * @return The hash value from the object's toHash method
         */
        template <isHashable T>
        static u_integer hashFunc(const T& t) noexcept;

        /**
         * @brief Hash function specialization for integral types
         * @tparam T Integral type
         * @param t The value to hash
         * @return The casted hash value
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
     * @tparam DERIVED The derived type
     * @brief Interface for user-defined hashable types
     * @details Provides default implementations for:
     * - toHash(): Uses byte-wise hashing of object representation
     * - equals(): Uses operator== comparison
     *
     * Usage:
     * @code
     * class MyType : public hashable<MyType> {
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

        /// Virtual destructor
        virtual ~hashable() = 0;
    };
}


namespace std {
    template <typename T>
    requires original::isHashable<T>
    struct hash<T> {
        std::size_t operator()(const T& t) const noexcept; // NOLINT
    };
}

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

template<typename TYPE>
template<original::isHashable T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
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
requires original::isHashable<T>
std::size_t std::hash<T>::operator()(const T &t) const noexcept {
    return static_cast<std::size_t>(t.toHash());
}

#endif //HASH_H
