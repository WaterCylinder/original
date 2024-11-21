#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <limits>
#include "iterator.h"
#include <functional>

namespace original
{
    class algorithms
    {
        public:
        template<typename TYPE>
        static size_t distance(const iterator<TYPE>& begin, const iterator<TYPE>& end);

        template<typename TYPE>
        static iterator<TYPE> find(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        template<typename TYPE, typename Callback>
        static iterator<TYPE> find(const iterator<TYPE>& begin,
                                   const iterator<TYPE>& end, Callback condition);

        template<typename TYPE>
        static size_t count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        template<typename TYPE, typename Callback>
        static size_t count(const iterator<TYPE>& begin,
                            const iterator<TYPE>& end, Callback condition);

        template<typename TYPE>
        static bool equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                          const iterator<TYPE>& begin2, const iterator<TYPE>& end2);

        template<typename TYPE>
        static iterator<TYPE> fill(const iterator<TYPE>& begin,
                             const iterator<TYPE>& end, const TYPE& value = TYPE{});

        template<typename TYPE>
        static iterator<TYPE> fill(const iterator<TYPE>& begin, size_t n, const TYPE& value = TYPE{});

        template<typename TYPE>
        static void swap(iterator<TYPE>& it1, iterator<TYPE>& it2) noexcept;

        template<typename TYPE>
        static iterator<TYPE> copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                                   const iterator<TYPE>& begin_tar);

        template<typename TYPE, typename Callback>
        static iterator<TYPE> copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                                   const iterator<TYPE>& begin_tar, Callback condition);

        template<typename TYPE>
        static iterator<TYPE> reverse(const iterator<TYPE>& begin, const iterator<TYPE>& end);
    };
}

    template <typename TYPE>
    auto original::algorithms::distance(const iterator<TYPE>& begin, const iterator<TYPE>& end) -> size_t
    {
        if (begin.isNull() || end.isNull()) {
            return std::numeric_limits<size_t>::max();
        }

        int dis = 0;
        iterator it = iterator(begin);
        while (!it.equal(end)) {
            if (it.isNull())
            {
                return std::numeric_limits<size_t>::max();
            }
            dis += 1;
            it.next();
        }
        return dis;
    }

    template <typename TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> iterator<TYPE>
    {
        iterator it = iterator(begin);
        while (!it.isNull() && !it.equal(end)) {
            if (it.get() == target) {
                return it;
            }
            it.next();
        }
        return end;
    }

    template<typename TYPE, typename Callback>
    auto original::algorithms::find(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                    const Callback condition) -> iterator<TYPE>{
        callBackChecker<Callback, bool, const TYPE&>::check();
        iterator it = iterator(begin);
        while (!it.isNull() && !it.equal(end)) {
            if (condition(it.get())) {
                return it;
            }
            it.next();
        }
        return end;
    }

    template <typename TYPE>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> size_t
    {
        size_t cnt = 0;
        iterator it = iterator(begin);
        while (!it.isNull() && !end.atPrev(it)) {
            if (it.get() == target) {
                cnt += 1;
            }
            it.next();
        }
        return cnt;
    }

    template <typename TYPE, typename Callback>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                     const Callback condition) -> size_t
    {
        callBackChecker<Callback, bool, const TYPE&>::check();
        size_t cnt = 0;
        iterator it = iterator(begin);
        while (!it.isNull() && !end.atPrev(it)) {
            if (condition(it.get())) {
                cnt += 1;
            }
            it.next();
        }
        return cnt;
    }

    template <typename TYPE>
    auto original::algorithms::equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                                     const iterator<TYPE>& begin2, const iterator<TYPE>& end2) -> bool
    {
        iterator it1 = iterator(begin1);
        iterator it2 = iterator(begin2);

        while (!it1.isNull() && !it2.isNull() && !it1.equal(end1) && !it2.equal(end2)) {
            it1.next();
            it2.next();
        }
        return it1.equal(end1) && it2.equal(end2) && it1.get() == it2.get();
    }

    template<typename TYPE>
    auto original::algorithms::fill(const iterator<TYPE>& begin,
                                    const iterator<TYPE>& end, const TYPE& value) -> iterator<TYPE> {
        iterator it = iterator(begin);
        while (!it.equal(end)){
            it.set(value);
            it.next();
        }
        it.set(value);
        return it;
    }

    template<typename TYPE>
    auto original::algorithms::fill(const iterator<TYPE>& begin,
                                    size_t n, const TYPE& value) -> iterator<TYPE> {
        iterator it = iterator(begin);
        for (int i = 0; i < n; ++i) {
            it.set(value);
            it.next();
        }
        return it;
    }

    template <typename TYPE>
    auto original::algorithms::swap(iterator<TYPE>& it1, iterator<TYPE>& it2) noexcept -> void
    {
        TYPE tmp = it2.get();
        it2.set(it1.get());
        it1.set(tmp);
    }

    template <typename TYPE>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                        const iterator<TYPE>& begin_tar) -> iterator<TYPE>{
        iterator it_src = iterator(begin_src);
        iterator it_tar = iterator(begin_tar);
        while (!it_src.equal(end_src)){
            it_tar.set(it_src.get());
            it_src.next();
            it_tar.next();
        }
        it_tar.set(it_src.get());
        it_tar.next();
        return it_tar;
    }

    template<typename TYPE, typename Callback>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                               const iterator<TYPE>& begin_tar, Callback condition) -> iterator<TYPE>{
        callBackChecker<Callback, bool, const TYPE&>::check();
        iterator it_src = iterator(begin_src);
        iterator it_tar = iterator(begin_tar);
        while (!it_src.equal(end_src)){
            if (condition(it_src.get())){
                it_tar.set(it_src.get());
            }
            it_src.next();
            it_tar.next();
        }
        if (condition(it_src.get())){
            it_tar.set(it_src.get());
        }
        it_tar.next();
        return it_tar;
    }

    template<typename TYPE>
    auto original::algorithms::reverse(const iterator<TYPE>& begin,
                                       const iterator<TYPE>& end) -> iterator<TYPE>{
        iterator left = iterator(begin);
        iterator right = iterator(end);
        while (!left.equal(right) && !left.atNext(right)){
            swap(left, right);
            left.next();
            right.prev();
        }
        return left;
    }

#endif // ALGORITHMS_H
