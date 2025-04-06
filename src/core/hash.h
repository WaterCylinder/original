#ifndef HASH_H
#define HASH_H


#include "config.h"
#include "types.h"


namespace original {
    template <typename TYPE>
    class hash;

    template <typename DERIVED>
    class hashable;

    template <typename DERIVED>
    concept isHashable = ExtendsOf<hashable<DERIVED>, DERIVED>;


    template <typename TYPE>
    class hash {
        template <typename T>
        static u_integer hashFunc(const T& t) noexcept;

        template <isHashable T>
        static u_integer hashFunc(const T& t) noexcept;
    public:
        u_integer operator()(const TYPE& t) noexcept;
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
template<typename T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    return static_cast<u_integer>(t);
}

template<typename TYPE>
template<original::isHashable T>
original::u_integer original::hash<TYPE>::hashFunc(const T &t) noexcept {
    return t.toHash();
}

template<typename TYPE>
original::u_integer original::hash<TYPE>::operator()(const TYPE &t) noexcept {
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
