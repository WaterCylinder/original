#ifndef TYPES_H
#define TYPES_H

#include "type_traits"
#include <concepts>

namespace original{
    template <typename TYPE>
    concept Comparable = requires(const TYPE& t1, const TYPE& t2) {
        { t1 == t2 } -> std::same_as<bool>;
        { t1 != t2 } -> std::same_as<bool>;
        { t1 < t2 } -> std::same_as<bool>;
        { t1 <= t2 } -> std::same_as<bool>;
        { t1 > t2 } -> std::same_as<bool>;
        { t1 >= t2 } -> std::same_as<bool>;
    };

    template <typename Callback, typename TYPE>
    concept Compare =
    Comparable<TYPE> &&
    requires(Callback compare, const TYPE& t1, const TYPE& t2) {
        { compare(t1, t2) } -> std::same_as<bool>;
    };

    template <typename Callback, typename TYPE>
    concept Condition = requires(Callback condition, const TYPE& t){
        { condition(t) } -> std::same_as<bool>;
    };

    template < typename Callback, typename TYPE>
    concept Operation = requires(Callback operation, TYPE& t){
        { operation(t) } -> std::same_as<void>;
    };

    template <typename Base, typename Derive>
    concept SuperOf = std::is_base_of_v<Base, Derive> || std::is_same_v<Base, Derive>;

    template <typename Base, typename Derive>
    concept ExtendsOf = std::derived_from<Derive, Base> || std::is_same_v<Base, Derive>;
}

#endif //TYPES_H
