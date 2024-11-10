#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <limits>
#include "iterator.h"

namespace original
{
    class algorithms
    {
        public:
        template<typename TYPE>
        static int distance(const iterator<TYPE>& begin, const iterator<TYPE>& end);

        template<typename TYPE>
        static iterator<TYPE> find(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        template<typename TYPE>
        static size_t count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        template<typename TYPE>
        static bool equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                          const iterator<TYPE>& begin2, const iterator<TYPE>& end2);

        template<typename TYPE>
        static void swap(iterator<TYPE>& it1, iterator<TYPE>& it2) noexcept;
    };
}

    template <typename TYPE>
    auto original::algorithms::distance(const iterator<TYPE>& begin, const iterator<TYPE>& end) -> int
    {
        if (begin.isNull() || end.isNull()) {
            return std::numeric_limits<int>::max();
        }

        int dis = 0;
        iterator it = iterator(begin);
        while (!it.equal(end)) {
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

    template <typename TYPE>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> size_t
    {
        size_t cnt = 0;
        iterator it = iterator(begin);
        while (!it.isNull() && !it.atNext(end)) {
            if (it.get() == target) {
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
            if (it1.get() != it2.get()) {
                return false;
            }
            it1.next();
            it2.next();
        }
        return it1.equal(end1) && it2.equal(end2);
    }

    template <typename TYPE>
    auto original::algorithms::swap(iterator<TYPE>& it1, iterator<TYPE>& it2) noexcept -> void
    {
        if (it1.isNull() || it2.isNull()) {
            return;
        }

        TYPE tmp = it2.get();
        it2.set(it1.get());
        it1.set(tmp);
    }

#endif // ALGORITHMS_H
