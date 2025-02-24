#ifndef TUPLE_H
#define TUPLE_H

#include "printable.h"
#include "comparable.h"


namespace original{
    template<typename... TYPES>
    class tuple final : public printable, public comparable<tuple<TYPES...>>{
        template<uint32_t I, typename... TS>
        class tupleImpl;

        template<uint32_t I>
        class tupleImpl<I> : public printable, public comparable<tupleImpl<I>>{
            explicit tupleImpl() = default;

            tupleImpl(const tupleImpl<I>& other) = default;

        public:
            friend class tuple;
            int64_t compareTo(const tupleImpl<I>& other) const override;

            std::string toString(bool enter) const override;
        };

        template<uint32_t I, typename T>
        class tupleImpl<I, T> : public printable, public comparable<tupleImpl<I, T>>{
            T cur_elem;
            tupleImpl<I + 1> next;

            explicit tupleImpl(T cur);

            tupleImpl(const tupleImpl& other);
        public:
            friend class tuple;
            int64_t compareTo(const tupleImpl<I, T>& other) const override;

            std::string toString(bool enter) const override;
        };

        template<uint32_t I, typename T, typename... TS>
        class tupleImpl<I, T, TS...> : public printable, public comparable<tupleImpl<I, T, TS...>>{
            T cur_elem;
            tupleImpl<I + 1, TS...> next;

            explicit tupleImpl(T cur, TS... next_elems);

            tupleImpl(const tupleImpl& other);
        public:
            friend class tuple;
            int64_t compareTo(const tupleImpl<I, T, TS...>& other) const override;

            std::string toString(bool enter) const override;
        };

        tupleImpl<0, TYPES...> elems;

    public:

        explicit tuple();

        explicit tuple(const TYPES&... e);

        tuple(const tuple& other);

        tuple& operator=(const tuple& other);

        tuple(tuple&& other) noexcept;

        tuple& operator=(tuple&& other) noexcept;

        int64_t compareTo(const tuple& other) const override;

        std::string toString(bool enter) const override;

        std::string className() const override;
    };
}

template<typename... TYPES>
template<uint32_t I>
int64_t original::tuple<TYPES...>::tupleImpl<I>::compareTo(const tupleImpl<I>&) const {
    return 0;
}

template<typename... TYPES>
template<uint32_t I>
std::string original::tuple<TYPES...>::tupleImpl<I>::toString(bool enter) const {
    return "";
}

template<typename... TYPES>
template<uint32_t I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(T cur)
    : cur_elem(cur), next() {}

template<typename... TYPES>
template<uint32_t I, typename T>
original::tuple<TYPES...>::tupleImpl<I, T>::tupleImpl(const tupleImpl<I, T>& other)
    : cur_elem(other.cur_elem), next() {}

template<typename... TYPES>
template<uint32_t I, typename T>
int64_t original::tuple<TYPES...>::tupleImpl<I, T>::compareTo(const tupleImpl<I, T>& other) const {
    if (cur_elem != other.cur_elem)
        return cur_elem < other.cur_elem ? -1 : 1;
    return next.compareTo(other.next);
}

template<typename... TYPES>
template<uint32_t I, typename T>
std::string original::tuple<TYPES...>::tupleImpl<I, T>::toString(bool enter) const {
    std::stringstream ss;
    ss << ", " << cur_elem;
    ss << next;
    return ss.str();
}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(T cur, TS... next_elems)
    : cur_elem(cur), next(next_elems...) {}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::tupleImpl(const tupleImpl<I, T, TS...>& other)
    : cur_elem(other.cur_elem), next(other.next) {}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
int64_t
original::tuple<TYPES...>::tupleImpl<I, T, TS...>::compareTo(const tupleImpl<I, T, TS...>& other) const {
    if (cur_elem != other.cur_elem)
        return cur_elem < other.cur_elem ? -1 : 1;
    return next.compareTo(other.next);
}

template<typename... TYPES>
template<uint32_t I, typename T, typename... TS>
std::string original::tuple<TYPES...>::tupleImpl<I, T, TS...>::toString(bool enter) const {
    std::stringstream ss;
    if constexpr (I != 0)
        ss << ", ";
    ss << cur_elem;
    ss << next;
    return ss.str();
}

template<typename... TYPES>
original::tuple<TYPES...>::tuple() : tuple(TYPES{}...) {}

template<typename... TYPES>
original::tuple<TYPES...>::tuple(const TYPES&... e) : elems(e...) {}

template<typename... TYPES>
original::tuple<TYPES...>::tuple(const tuple& other) : elems(other.elems) {}

template<typename... TYPES>
original::tuple<TYPES...>& original::tuple<TYPES...>::operator=(const tuple& other) {
    if (this == &other) return *this;
    this->elems = other.elems;
    return *this;
}

template<typename... TYPES>
original::tuple<TYPES...>::tuple(tuple&& other) noexcept
    : elems(std::move(other.elems)) {}

template<typename... TYPES>
original::tuple<TYPES...>& original::tuple<TYPES...>::operator=(tuple&& other) noexcept {
    if (this == &other) return *this;
    this->elems = std::move(other.elems);
    return *this;
}

template<typename... TYPES>
int64_t original::tuple<TYPES...>::compareTo(const tuple& other) const {
    return elems.compareTo(other.elems);
}

template<typename... TYPES>
std::string original::tuple<TYPES...>::toString(bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(" << elems << ")";
    return ss.str();
}

template<typename... TYPES>
std::string original::tuple<TYPES...>::className() const {
    return "tuple";
}

#endif //TUPLE_H
