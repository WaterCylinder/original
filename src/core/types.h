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

    template <typename Callback, typename ReturnType, typename... Args>
    concept CallbackOf = requires(Callback callback, Args&&... args){
        { callback(std::forward<Args>(args)...) } -> std::same_as<ReturnType>;
    };

    template <typename Callback, typename TYPE>
    concept Compare =
    Comparable<TYPE> && CallbackOf<Callback, bool, const TYPE&, const TYPE&>;

    template <typename Callback, typename TYPE>
    concept Condition = CallbackOf<Callback, bool, const TYPE&>;

    template <typename Callback, typename TYPE>
    concept Operation = CallbackOf<Callback, void, TYPE&>;

    template <typename Base, typename Derive>
    concept SuperOf = std::is_base_of_v<Base, Derive> || std::is_same_v<Base, Derive>;

    template <typename Base, typename Derive>
    concept ExtendsOf = std::derived_from<Derive, Base> || std::is_same_v<Base, Derive>;
}

#endif //TYPES_H
