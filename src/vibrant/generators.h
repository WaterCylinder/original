#ifndef ORIGINAL_GENERATORS_H
#define ORIGINAL_GENERATORS_H
#include "coroutines.h"
#include "couple.h"
#include "sets.h"


namespace original {
    template<typename TYPE>
    coroutine::generator<couple<u_integer, TYPE>> enumerate(coroutine::generator<TYPE> gen);

    template<typename TYPE, typename SET = hashSet<TYPE>>
    requires ExtendsOf<set<TYPE, allocator<couple<const TYPE, const bool>>>, SET>
    SET collect(coroutine::generator<TYPE> gen);

    template<typename TYPE, template <typename> typename SERIAL = vector>
    requires ExtendsOf<baseList<TYPE, allocator<TYPE>>, SERIAL<TYPE>>
    SERIAL<TYPE> list(coroutine::generator<TYPE> gen);

    template<typename TYPE, typename Callback>
    auto transforms(coroutine::generator<TYPE> gen, Callback&& c) -> coroutine::generator<std::invoke_result_t<Callback, TYPE>>;

    template<typename TYPE, typename Callback>
    coroutine::generator<TYPE> filters(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename TYPE, typename Callback>
    coroutine::generator<TYPE> extract(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename T, typename U>
    coroutine::generator<couple<T, U>> zip(coroutine::generator<T> gen1, coroutine::generator<U> gen2);

    template<typename TYPE>
    u_integer count(coroutine::generator<TYPE> gen);

    template<typename TYPE, typename Callback>
    u_integer count(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename TYPE, typename Callback>
    bool all(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename TYPE, typename Callback>
    bool none(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename TYPE, typename Callback>
    bool any(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename U, std::convertible_to<U> T>
    coroutine::generator<U> join(coroutine::generator<U> gen1, coroutine::generator<T> gen2);

    template<typename U, std::convertible_to<U> T>
    coroutine::generator<U> flatten(coroutine::generator<couple<U, T>> gen);

    template<typename TYPE>
    coroutine::generator<TYPE> take(coroutine::generator<TYPE> gen, u_integer n);

    template<typename TYPE>
    coroutine::generator<TYPE> skip(coroutine::generator<TYPE> gen, u_integer n);

    template<typename TYPE, typename Callback>
    u_integer position(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename TYPE, typename Callback>
    TYPE find(coroutine::generator<TYPE> gen, Callback&& c);

    template<typename Callback>
    class genPipe {
        Callback c_;

        genPipe() = default;

        explicit genPipe(Callback&& c);

        template<typename Generator>
        auto operator()(Generator&& gen);
    public:
        template<typename TYPE, typename F>
        friend auto operator|(coroutine::generator<TYPE> gen, genPipe<F> p);

        template<typename U, typename T>
        friend auto join(coroutine::generator<U> gen2);

        template<typename U, typename T>
        friend auto flatten();

        template<typename F>
        friend auto transforms(F&& f);

        template<typename F>
        friend auto filters(F&& f);

        template<typename F>
        friend auto extract(F&& f);

        friend auto enumerate();

        friend auto take(u_integer n);

        friend auto skip(u_integer n);

        template<typename F>
        friend auto zipWith(coroutine::generator<F> gen2);

        friend auto count();

        template<typename F>
        friend auto count(F&& f);

        template<typename F>
        friend auto all(F&& f);

        template<typename F>
        friend auto none(F&& f);

        template<typename F>
        friend auto any(F&& f);

        template<typename F>
        friend auto position(F&& f);

        template<typename F>
        friend auto find(F&& f);
    };

    template<typename TYPE, typename F>
    auto operator|(coroutine::generator<TYPE> gen, genPipe<F> p);

    template<typename F>
    auto transforms(F&& f);

    template<typename F>
    auto filters(F&& f);

    template<typename F>
    auto extract(F&& f);

    auto enumerate();

    auto take(u_integer n);

    auto skip(u_integer n);

    template<typename U, typename T>
    auto join(coroutine::generator<T> gen2);

    template<typename U, typename T>
    auto flatten();

    template<typename U>
    auto zipWith(coroutine::generator<U> gen2);

    auto count();

    template<typename F>
    auto count(F&& f);

    template<typename F>
    auto all(F&& f);

    template<typename F>
    auto none(F&& f);

    template<typename F>
    auto any(F&& f);

    template<typename F>
    auto position(F&& f);

    template<typename F>
    auto find(F&& f);
}

template <typename TYPE>
original::coroutine::generator<original::couple<original::u_integer, TYPE>>
original::enumerate(coroutine::generator<TYPE> gen)
{
    u_integer i = 0;
    for (auto elem : gen)
    {
        co_yield {i, elem};
        i += 1;
    }
}

template<typename TYPE, typename SET>
requires original::ExtendsOf<original::set<TYPE, original::allocator<original::couple<const TYPE, const bool>>>, SET>
SET original::collect(coroutine::generator<TYPE> gen)
{
    SET set;
    for (auto elem : gen)
    {
        set.add(elem);
    }
    return set;
}

template <typename TYPE, template <typename> class SERIAL>
requires original::ExtendsOf<original::baseList<TYPE, original::allocator<TYPE>>, SERIAL<TYPE>>
SERIAL<TYPE> original::list(coroutine::generator<TYPE> gen)
{
    SERIAL<TYPE> list;
    for (auto elem : gen)
    {
        list.pushEnd(elem);
    }
    return list;
}

template <typename TYPE, typename Callback>
auto original::transforms(coroutine::generator<TYPE> gen, Callback&& c) -> coroutine::generator<std::invoke_result_t<Callback, TYPE>>
{
    for (auto elem : gen)
    {
        co_yield c(std::forward<TYPE>(elem));
    }
}

template <typename TYPE, typename Callback>
original::coroutine::generator<TYPE>
original::filters(coroutine::generator<TYPE> gen, Callback&& c)
{
    for (auto elem : gen)
    {
        if (c(std::forward<TYPE>(elem)))
        {
            co_yield elem;
        }
    }
}

template <typename TYPE, typename Callback>
original::coroutine::generator<TYPE>
original::extract(coroutine::generator<TYPE> gen, Callback&& c)
{
    for (auto elem : gen)
    {
        if (c(std::forward<TYPE>(elem)))
        {
            continue;
        }
        co_yield elem;
    }
}

template <typename T, typename U>
original::coroutine::generator<original::couple<T, U>>
original::zip(coroutine::generator<T> gen1, coroutine::generator<U> gen2)
{
    auto elem1 = gen1.next();
    auto elem2 = gen2.next();
    while (elem1 && elem2)
    {
        co_yield {std::move(*elem1), std::move(*elem2)};
        elem1 = std::move(gen1.next());
        elem2 = std::move(gen2.next());
    }
}

template <typename TYPE>
original::u_integer original::count(coroutine::generator<TYPE> gen)
{
    u_integer res = 0;
    while (auto val = gen.next())
    {
        res += 1;
    }
    return res;
}

template <typename TYPE, typename Callback>
original::u_integer original::count(coroutine::generator<TYPE> gen, Callback&& c)
{
    u_integer res = 0;
    while (auto val = gen.next())
    {
        if (c(std::forward<TYPE>(*val)))
            res += 1;
    }
    return res;
}

template <typename TYPE, typename Callback>
bool original::all(coroutine::generator<TYPE> gen, Callback&& c)
{
    while (auto val = gen.next())
    {
        if (!c(std::forward<TYPE>(*val)))
            return false;
    }
    return true;
}

template <typename TYPE, typename Callback>
bool original::none(coroutine::generator<TYPE> gen, Callback&& c)
{
    while (auto val = gen.next())
    {
        if (c(std::forward<TYPE>(*val)))
            return false;
    }
    return true;
}

template <typename TYPE, typename Callback>
bool original::any(coroutine::generator<TYPE> gen, Callback&& c)
{
    while (auto val = gen.next())
    {
        if (c(std::forward<TYPE>(*val)))
            return true;
    }
    return false;
}

template<typename U, std::convertible_to<U> T>
original::coroutine::generator<U>
original::join(coroutine::generator<U> gen1, coroutine::generator<T> gen2)
{
    for (auto elem : gen1)
    {
        co_yield elem;
    }
    for (auto elem : gen2)
    {
        co_yield static_cast<U>(elem);
    }
}

template <typename U, std::convertible_to<U> T>
original::coroutine::generator<U>
original::flatten(coroutine::generator<couple<U, T>> gen)
{
    for (auto [elem1, elem2] : gen)
    {
        co_yield elem1;
        co_yield static_cast<U>(elem2);
    }
}

template <typename TYPE>
original::coroutine::generator<TYPE>
original::take(coroutine::generator<TYPE> gen, const u_integer n)
{
    u_integer i = 0;
    for (auto elem : gen)
    {
        if (i == n)
        {
            co_return;
        }
        i += 1;
        co_yield elem;
    }
}

template <typename TYPE>
original::coroutine::generator<TYPE>
original::skip(coroutine::generator<TYPE> gen, const u_integer n)
{
    u_integer i = 0;
    for (auto elem : gen)
    {
        if (i < n)
        {
            i += 1;
            continue;
        }
        co_yield elem;
    }
}

template <typename TYPE, typename Callback>
original::u_integer original::position(coroutine::generator<TYPE> gen, Callback&& c)
{
    u_integer i = 0;
    for (auto elem : gen)
    {
        if (c(std::forward<TYPE>(elem))){
            return i;
        }
        i += 1;
    }
    return i;
}

template <typename TYPE, typename Callback>
TYPE original::find(coroutine::generator<TYPE> gen, Callback&& c)
{
    for (auto elem : gen)
    {
        if (c(std::forward<TYPE>(elem)))
        {
            return elem;
        }
    }
    return TYPE{};
}

template <typename Callback>
original::genPipe<Callback>::genPipe(Callback&& c) : c_(std::forward<Callback>(c)) {}

template <typename Callback>
template <typename Generator>
auto original::genPipe<Callback>::operator()(Generator&& gen)
{
    return this->c_(std::forward<Generator>(gen));
}

template <typename TYPE, typename F>
auto original::operator|(coroutine::generator<TYPE> gen, genPipe<F> p)
{
    return p(std::move(gen));
}

template <typename F>
auto original::transforms(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return transforms(std::move(gen), std::move(c));
    }};
}

template <typename F>
auto original::filters(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return filters(std::move(gen), std::move(c));
    }};
}

template <typename F>
auto original::extract(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return extract(std::move(gen), std::move(c));
    }};
}

inline auto original::enumerate()
{
    return genPipe{[]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return enumerate(std::move(gen));
    }};
}

inline auto original::take(u_integer n)
{
    return genPipe{[n]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return take(std::move(gen), n);
    }};
}

inline auto original::skip(u_integer n)
{
    return genPipe{[n]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return skip(std::move(gen), n);
    }};
}

template <typename U, typename T>
auto original::join(coroutine::generator<T> gen2)
{
    return genPipe{[gen2 = std::move(gen2)]<typename TYPE>(coroutine::generator<TYPE> gen1) mutable {
        return join<U, TYPE>(std::move(gen1), std::move(gen2));
    }};
}

template<typename U, typename T>
auto original::flatten()
{
    return genPipe{[]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return flatten<U, T>(std::move(gen));
    }};
}

template<typename U>
auto original::zipWith(coroutine::generator<U> gen2)
{
    return genPipe{[gen2 = std::move(gen2)]<typename T>(coroutine::generator<T> gen1) mutable {
        return zip(std::move(gen1), std::move(gen2));
    }};
}

inline auto original::count()
{
    return genPipe{[]<typename TYPE>(coroutine::generator<TYPE> gen) {
        return count(std::move(gen));
    }};
}

template<typename F>
auto original::count(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return count(std::move(gen), std::move(c));
    }};
}

template<typename F>
auto original::all(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return all(std::move(gen), std::move(c));
    }};
}

template<typename F>
auto original::none(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return none(std::move(gen), std::move(c));
    }};
}

template<typename F>
auto original::any(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return any(std::move(gen), std::move(c));
    }};
}

template<typename F>
auto original::position(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return position(std::move(gen), std::move(c));
    }};
}

template<typename F>
auto original::find(F&& f)
{
    return genPipe{[c = std::forward<F>(f)]<typename TYPE>(coroutine::generator<TYPE> gen) mutable {
        return find(std::move(gen), std::move(c));
    }};
}

#endif //ORIGINAL_GENERATORS_H
