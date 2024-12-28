#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <filter.h>
#include <limits>
#include "iterator.h"
#include "error.h"

namespace original
{
    class algorithms
    {
        public:
        template<typename TYPE>
        static uint32_t distance(const iterator<TYPE>& begin, const iterator<TYPE>& end);

        template<typename TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, const iterator<TYPE> &end, const TYPE &target);

        template<typename TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, uint32_t n, const TYPE &target);

        template<typename TYPE, typename Callback>
        static iterator<TYPE>* find(const iterator<TYPE> &begin,
                                    const iterator<TYPE> &end, Callback condition);

        template<typename TYPE, typename Callback>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, uint32_t n, Callback condition);

        template<typename TYPE>
        static uint32_t count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        template<typename TYPE, typename Callback>
        static uint32_t count(const iterator<TYPE>& begin, const iterator<TYPE>& end, Callback condition);

        template<typename TYPE>
        static bool equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                          const iterator<TYPE>& begin2, const iterator<TYPE>& end2);

        template<typename TYPE, typename Callback>
        static void forEach(const iterator<TYPE>& begin,
                            const iterator<TYPE>& end, Callback operation);

        template<typename TYPE, typename Callback>
        static iterator<TYPE>* forEach(const iterator<TYPE> &begin, uint32_t n, Callback operation);

        template<typename TYPE>
        static void fill(const iterator<TYPE>& begin,
                         const iterator<TYPE>& end, const TYPE& value = TYPE{});

        template<typename TYPE>
        static iterator<TYPE>* fill(const iterator<TYPE> &begin, uint32_t n, const TYPE &value = TYPE{});

        template<typename TYPE>
        static void swap(const iterator<TYPE>& it1, const iterator<TYPE>& it2) noexcept;

        template<typename TYPE>
        static iterator<TYPE>* copy(const iterator<TYPE> &begin_src, const iterator<TYPE> &end_src,
                                    const iterator<TYPE> &begin_tar);

        template<typename TYPE, typename Callback>
        static iterator<TYPE>* copy(const iterator<TYPE> &begin_src, const iterator<TYPE> &end_src,
                                    const iterator<TYPE> &begin_tar, Callback condition = filter<TYPE>{});

        template<typename TYPE>
        static iterator<TYPE>* reverse(const iterator<TYPE> &begin, const iterator<TYPE> &end);
    };
}

    template <typename TYPE>
    auto original::algorithms::distance(const iterator<TYPE>& begin, const iterator<TYPE>& end) -> uint32_t
    {
        if (!begin.isValid() || !end.isValid()) {
            return std::numeric_limits<uint32_t>::max();
        }

        uint32_t dis = 0;
        auto it = begin.clone();
        while (!it->equal(end)) {
            if (!it->isValid())
            {
                return std::numeric_limits<uint32_t>::max();
            }
            dis += 1;
            it->next();
        }
        delete it;
        return dis;
    }

    template <typename TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> iterator<TYPE>* {
        auto it = begin.clone();
        while (it->isValid() && !it->equal(end)) {
            if (it->get() == target) {
                return it;
            }
            it->next();
        }
        delete it;
        return end.clone();
    }

    template <typename TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const uint32_t n, const TYPE& target) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (uint32_t i = 0; i < n; i += 1, it->next())
        {
            if (it->get() == target) return it;
        }
        return it;
    }

    template<typename TYPE, typename Callback>
    auto original::algorithms::find(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                    const Callback condition) -> iterator<TYPE>* {
        callBackChecker<Callback, bool, const TYPE&>::check();
        auto it = begin.clone();
        while (it->isValid() && !it->equal(end)) {
            if (condition(it->get())) {
                return it;
            }
            it->next();
        }
        delete it;
        return end.clone();
    }

    template <typename TYPE, typename Callback>
    auto original::algorithms::find(const iterator<TYPE>& begin, const uint32_t n, Callback condition) -> iterator<TYPE>* {
        callBackChecker<Callback, bool, const TYPE&>::check();
        auto it = begin.clone();
        for (uint32_t i = 0; i < n; i += 1, it->next())
        {
            if (condition(it->get())) return it;
        }
        return it;
    }

    template <typename TYPE>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
        const TYPE& target) -> uint32_t
    {
        uint32_t cnt = 0;
        auto it = begin.clone();
        while (it->isValid() && !end.atPrev(it)) {
            if (it->get() == target) {
                cnt += 1;
            }
            it->next();
        }
        delete it;
        return cnt;
    }

    template <typename TYPE, typename Callback>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                     const Callback condition) -> uint32_t
    {
        callBackChecker<Callback, bool, const TYPE&>::check();
        uint32_t cnt = 0;
        auto it = begin.clone();
        while (it->isValid() && !end.atPrev(it)) {
            if (condition(it->get())) {
                cnt += 1;
            }
            it->next();
        }
        delete it;
        return cnt;
    }

    template <typename TYPE>
    auto original::algorithms::equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                                     const iterator<TYPE>& begin2, const iterator<TYPE>& end2) -> bool
    {
        auto it1 = begin1.clone();
        auto it2 = begin2.clone();

        while (it1->isValid() && it2->isValid() && !it1->equal(end1) && !it2->equal(end2)) {
            it1->next();
            it2->next();
        }
        const bool res = it1->equal(end1) && it2->equal(end2) && it1->get() == it2->get();
        delete it1;
        delete it2;
        return res;
    }

    template <typename TYPE, typename Callback>
    auto original::algorithms::forEach(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                       Callback operation) -> void
    {
        callBackChecker<Callback, void, TYPE&>::check();
        auto it = begin.clone();
        for (; !it->equal(end); it->next()) {
            operation(it->get());
        }
        operation(it->get());
        delete it;
    }

    template <typename TYPE, typename Callback>
    auto original::algorithms::forEach(const iterator<TYPE>& begin, const uint32_t n,
                                       Callback operation) -> iterator<TYPE>* {
        callBackChecker<Callback, void, TYPE&>::check();
        auto it = begin.clone();
        for (uint32_t i = 0; i < n; i += 1, it->next())
        {
            operation(it->get());
        }
        return it;
    }

    template<typename TYPE>
    auto original::algorithms::fill(const iterator<TYPE>& begin,
                                    const iterator<TYPE>& end, const TYPE& value) -> void
    {
        auto it = begin.clone();
        while (!it->equal(end)){
            it->set(value);
            it->next();
        }
        it->set(value);
        delete it;
    }

    template<typename TYPE>
    auto original::algorithms::fill(const iterator<TYPE>& begin,
                                    const uint32_t n, const TYPE& value) -> iterator<TYPE>* {
        auto it = begin.clone();
        for (uint32_t i = 0; i < n; ++i) {
            it->set(value);
            it->next();
        }
        return it;
    }

    template <typename TYPE>
    auto original::algorithms::swap(const iterator<TYPE>& it1, const iterator<TYPE>& it2) noexcept -> void
    {
        auto it_1 = it1.clone();
        auto it_2 = it2.clone();
        TYPE tmp = it_2->get();
        it_2->set(it_1->get());
        it_1->set(tmp);
        delete it_1;
        delete it_2;
    }

    template <typename TYPE>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                        const iterator<TYPE>& begin_tar) -> iterator<TYPE>* {
        auto it_src = begin_src.clone();
        auto it_tar = begin_tar.clone();
        while (!it_src->equal(end_src)){
            it_tar->set(it_src->get());
            it_src->next();
            it_tar->next();
        }
        it_tar->set(it_src->get());
        it_tar->next();
        delete it_src;
        return it_tar;
    }

    template<typename TYPE, typename Callback>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                               const iterator<TYPE>& begin_tar, Callback condition) -> iterator<TYPE>* {
        callBackChecker<Callback, bool, const TYPE&>::check();
        auto it_src = begin_src.clone();
        auto it_tar = begin_tar.clone();
        while (!it_src->equal(end_src)){
            if (condition(it_src->get())){
                it_tar->set(it_src->get());
            }
            it_src->next();
            it_tar->next();
        }
        if (condition(it_src->get())){
            it_tar->set(it_src->get());
        }
        it_tar->next();
        delete it_src;
        return it_tar;
    }

    template<typename TYPE>
    auto original::algorithms::reverse(const iterator<TYPE>& begin,
                                       const iterator<TYPE>& end) -> iterator<TYPE>* {
        auto left = begin.clone();
        auto right = end.clone();
        while (!left->equal(right) && !left->atNext(right)){
            swap(*left, *right);
            left->next();
            right->prev();
        }
        delete right;
        return left;
    }

#endif // ALGORITHMS_H
