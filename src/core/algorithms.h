#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "filter.h"
#include "iterator.h"
#include "error.h"
#include "types.h"

namespace original
{
    class algorithms
    {
        public:
        template<typename TYPE>
        static int64_t distance(const iterator<TYPE>& end, const iterator<TYPE>& begin);

        template<typename TYPE>
        static iterator<TYPE>* frontOf(const iterator<TYPE>& it, int64_t steps);

        template<typename TYPE>
        static iterator<TYPE>* backOf(const iterator<TYPE>& it, int64_t steps);

        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static bool allOf(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static bool anyOf(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static bool noneOf(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        template<typename TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, const iterator<TYPE> &end, const TYPE &target);

        template<typename TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, uint32_t n, const TYPE &target);

        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin,
                                    const iterator<TYPE> &end, const Callback& condition);

        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static iterator<TYPE>* find(const iterator<TYPE> &begin, uint32_t n, const Callback& condition);

        template<typename TYPE>
        static uint32_t count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const TYPE& target);

        template<typename TYPE, typename Callback>
        requires Condition<Callback, TYPE>
        static uint32_t count(const iterator<TYPE>& begin, const iterator<TYPE>& end, const Callback& condition);

        template<typename TYPE>
        static bool equal(const iterator<TYPE>& begin1, const iterator<TYPE>& end1,
                          const iterator<TYPE>& begin2, const iterator<TYPE>& end2);

        template<typename TYPE, typename Callback>
        requires Operation<Callback, TYPE>
        static void forEach(const iterator<TYPE>& begin,
                            const iterator<TYPE>& end, Callback operation);

        template<typename TYPE, typename Callback>
        requires Operation<Callback, TYPE>
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
        requires Condition<Callback, TYPE>
        static iterator<TYPE>* copy(const iterator<TYPE> &begin_src, const iterator<TYPE> &end_src,
                                    const iterator<TYPE> &begin_tar, Callback condition = filter<TYPE>{});

        template<typename TYPE>
        static iterator<TYPE>* reverse(const iterator<TYPE> &begin, const iterator<TYPE> &end);

        template<typename TYPE, typename Callback>
        requires Comparable<TYPE> && Compare<Callback, TYPE>
        static bool compare(const iterator<TYPE>& it1, const iterator<TYPE>& it2, const Callback& compares);

        template<typename TYPE, typename Callback>
        requires Comparable<TYPE> && Compare<Callback, TYPE>
        static void heapAdjustDown(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                   const iterator<TYPE>& current, const Callback& compares);

        template<typename TYPE, typename Callback>
        requires Comparable<TYPE> && Compare<Callback, TYPE>
        static void heapAdjustUp(const iterator<TYPE>& begin, const iterator<TYPE>& current,
                                 const Callback& compares);

        template<typename TYPE, typename Callback>
        requires Comparable<TYPE> && Compare<Callback, TYPE>
        static void heapInit(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                             const Callback& compares);

        protected:
        template<typename TYPE, typename Callback>
        requires Comparable<TYPE> && Compare<Callback, TYPE>
        static iterator<TYPE>* heapGetPrior(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                            const iterator<TYPE>& parent, const Callback& compares);


        /**
         * ---- Implementation of pointer overload version. ----
         * */


        template <typename TYPE>
        auto distance(const iterator<TYPE>* end, const iterator<TYPE>* begin) -> int64_t
        {
            return distance(*end, *begin);
        }

        template<typename TYPE>
        static iterator<TYPE>* frontOf(const iterator<TYPE>* it, int64_t steps){
            return frontOf(*it, steps);
        }

        template<typename TYPE>
        static iterator<TYPE>* backOf(const iterator<TYPE>* it, int64_t steps){
            return backOf(*it, steps);
        }

        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static bool allOf(const original::iterator<TYPE>* begin, const original::iterator<TYPE>* end,
                                         const Callback& condition) {
            return allOf(*begin, *end, condition);
        }

        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static bool anyOf(const original::iterator<TYPE>* begin, const original::iterator<TYPE>* end,
                                         const Callback& condition) {
            return anyOf(*begin, *end, condition);
        }

        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static bool noneOf(const original::iterator<TYPE>* begin, const original::iterator<TYPE>* end,
                                          const Callback& condition) {
            return noneOf(*begin, *end, condition);
        }

        template <typename TYPE>
        static auto find(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                        const TYPE& target) -> iterator<TYPE>* {
            return find(*begin, *end, target);
        }

        template <typename TYPE>
        static auto find(const iterator<TYPE>* begin, uint32_t n, const TYPE& target) -> iterator<TYPE>* {
            return find(*begin, n, target);
        }

        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto find(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                        const Callback& condition) -> iterator<TYPE>* {
            return find(*begin, *end, condition);
        }

        template <typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto find(const iterator<TYPE>* begin, uint32_t n, const Callback& condition) -> iterator<TYPE>* {
            return find(*begin, n, condition);
        }

        template <typename TYPE>
        static auto count(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                         const TYPE& target) -> uint32_t {
            return count(*begin, *end, target);
        }

        template <typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto count(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                         const Callback& condition) -> uint32_t {
            return count(*begin, *end, condition);
        }

        template <typename TYPE>
        static auto equal(const iterator<TYPE>* begin1, const iterator<TYPE>* end1,
                                         const iterator<TYPE>* begin2, const iterator<TYPE>* end2) -> bool {
            return equal(*begin1, *end1, *begin2, *end2);
        }

        template <typename TYPE, typename Callback>
        requires original::Operation<Callback, TYPE>
        static auto forEach(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                           Callback operation) -> void {
            forEach(*begin, *end, operation);
        }

        template <typename TYPE, typename Callback>
        requires original::Operation<Callback, TYPE>
        static auto forEach(const iterator<TYPE>* begin, uint32_t n, Callback operation) -> iterator<TYPE>* {
            return forEach(*begin, n, operation);
        }

        template<typename TYPE>
        static auto fill(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                        const TYPE& value) -> void {
            fill(*begin, *end, value);
        }

        template<typename TYPE>
        static auto fill(const iterator<TYPE>* begin, uint32_t n, const TYPE& value) -> iterator<TYPE>* {
            return fill(*begin, n, value);
        }

        template <typename TYPE>
        static auto swap(const iterator<TYPE>* it1, const iterator<TYPE>* it2) noexcept -> void {
            swap(*it1, *it2);
        }

        template <typename TYPE>
        static auto copy(const iterator<TYPE>* begin_src, const iterator<TYPE>* end_src,
                                        const iterator<TYPE>* begin_tar) -> iterator<TYPE>* {
            return copy(*begin_src, *end_src, *begin_tar);
        }

        template<typename TYPE, typename Callback>
        requires original::Condition<Callback, TYPE>
        static auto copy(const iterator<TYPE>* begin_src, const iterator<TYPE>* end_src,
                                        const iterator<TYPE>* begin_tar, Callback condition) -> iterator<TYPE>* {
            return copy(*begin_src, *end_src, *begin_tar, condition);
        }

        template<typename TYPE>
        static auto reverse(const iterator<TYPE>* begin, const iterator<TYPE>* end) -> iterator<TYPE>* {
            return reverse(*begin, *end);
        }

        template <typename TYPE, typename Callback>
        requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
        static auto compare(const iterator<TYPE>* it1, const iterator<TYPE>* it2,
                                           const Callback& compares) -> bool {
            return compare(*it1, *it2, compares);
        }

        template<typename TYPE, typename Callback>
        requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
        static auto heapAdjustDown(const iterator<TYPE>* begin, const iterator<TYPE>* range,
                                                  const iterator<TYPE>* current, const Callback& compares) -> void {
            heapAdjustDown(*begin, *range, *current, compares);
        }

        template<typename TYPE, typename Callback>
        requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
        static auto heapAdjustUp(const iterator<TYPE>* begin, const iterator<TYPE>* current,
                                                const Callback& compares) -> void {
            heapAdjustUp(*begin, *current, compares);
        }

        template<typename TYPE, typename Callback>
        requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
        static auto heapInit(const iterator<TYPE>* begin, const iterator<TYPE>* end,
                                            const Callback& compares) -> void {
            heapInit(*begin, *end, compares);
        }

        template<typename TYPE, typename Callback>
        requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
        static auto heapGetPrior(const iterator<TYPE>* begin, const iterator<TYPE>* range,
                                                const iterator<TYPE>* parent, const Callback& compares) -> iterator<TYPE>* {
            return heapGetPrior(*begin, *range, *parent, compares);
        }
    };
}

    template <typename TYPE>
    auto original::algorithms::distance(const iterator<TYPE>& end, const iterator<TYPE>& begin) -> int64_t
    {
        auto* it1 = end.clone();
        auto* it2 = begin.clone();
        auto dis = it1->operator-(*it2);
        delete it1;
        delete it2;
        return dis;
    }

    template<typename TYPE>
    original::iterator<TYPE> *original::algorithms::frontOf(const original::iterator<TYPE> &it, int64_t steps) {
        return it + steps;
    }

    template<typename TYPE>
    original::iterator<TYPE> *original::algorithms::backOf(const original::iterator<TYPE> &it, int64_t steps) {
        return it - steps;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    bool original::algorithms::allOf(const original::iterator<TYPE> &begin, const original::iterator<TYPE> &end,
                                     const Callback& condition) {
        auto* it = begin.clone();
        for (; !it->equal(end); it->next()){
            if (!condition(it->get())){
                delete it;
                return false;
            }
        }
        delete it;
        return true;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    bool original::algorithms::anyOf(const original::iterator<TYPE> &begin, const original::iterator<TYPE> &end,
                                     const Callback& condition) {
        auto* it = begin.clone();
        for (; !it->equal(end); it->next()){
            if (condition(it->get())){
                delete it;
                return true;
            }
        }
        delete it;
        return false;
    }

    template<typename TYPE, typename Callback>
    requires original::Condition<Callback, TYPE>
    bool original::algorithms::noneOf(const original::iterator<TYPE> &begin, const original::iterator<TYPE> &end,
                                      const Callback& condition) {
        auto* it = begin.clone();
        for (; !it->equal(end); it->next()){
            if (condition(it->get())){
                delete it;
                return false;
            }
        }
        delete it;
        return true;
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
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::find(const iterator<TYPE> &begin, const iterator<TYPE> &end,
                                    const Callback& condition) -> iterator<TYPE>* {
        callBackChecker::check<Callback, bool, const TYPE&>();
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
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::find(const iterator<TYPE>& begin, const uint32_t n, const Callback& condition) -> iterator<TYPE>* {
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
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::count(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                     const Callback& condition) -> uint32_t
    {
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
    requires original::Operation<Callback, TYPE>
    auto original::algorithms::forEach(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                       Callback operation) -> void
    {
        callBackChecker::check<Callback, void, TYPE&>();
        auto it = begin.clone();
        for (; !it->equal(end); it->next()) {
            operation(it->get());
        }
        operation(it->get());
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Operation<Callback, TYPE>
    auto original::algorithms::forEach(const iterator<TYPE>& begin, const uint32_t n,
                                       Callback operation) -> iterator<TYPE>* {
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
    requires original::Condition<Callback, TYPE>
    auto original::algorithms::copy(const iterator<TYPE>& begin_src, const iterator<TYPE>& end_src,
                               const iterator<TYPE>& begin_tar, Callback condition) -> iterator<TYPE>* {
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
            swap(left, right);
            left->next();
            right->prev();
        }
        delete right;
        return left;
    }

    template <typename TYPE, typename Callback>
    requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
    auto original::algorithms::compare(const iterator<TYPE>& it1, const iterator<TYPE>& it2,
                                       const Callback& compares) -> bool
    {
        return compares(it1.get(), it2.get());
    }

    template <typename TYPE, typename Callback>
    requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
    auto original::algorithms::heapAdjustDown(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                              const iterator<TYPE>& current, const Callback& compares) -> void
    {
        if (distance(current, begin) < 0)
            return;

        auto* it = current.clone();
        while (distance(*it, begin) * 2 + 1 <= distance(range, begin))
        {
            auto* child = heapGetPrior(begin, range, *it, compares);
            if (compare(it, child, compares))
            {
                delete child;
                break;
            }
            swap(it, child);
            delete it;
            it = child->clone();
            delete child;
        }
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
    auto original::algorithms::heapAdjustUp(const iterator<TYPE>& begin, const iterator<TYPE>& current,
                                            const Callback& compares) -> void
    {
        auto* it = current.clone();
        while (distance(*it, begin) > 0)
        {
            auto* parent = frontOf(begin, (distance(*it, begin) - 1) / 2);
            if (compare(it, parent, compares))
            {
                swap(it, parent);
            }else
            {
                delete parent;
                break;
            }
            delete parent;
        }
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
    auto original::algorithms::heapInit(const iterator<TYPE>& begin, const iterator<TYPE>& end,
                                        const Callback& compares) -> void
    {
        auto* it = frontOf(begin, (distance(end, begin) + 1) / 2 - 1);
        for (; distance(*it, begin) >= 0; it->prev())
        {
            heapAdjustDown(begin, end, *it, compares);
        }
        delete it;
    }

    template <typename TYPE, typename Callback>
    requires original::Comparable<TYPE> && original::Compare<Callback, TYPE>
    auto original::algorithms::heapGetPrior(const iterator<TYPE>& begin, const iterator<TYPE>& range,
                                            const iterator<TYPE>& parent, const Callback& compares) -> iterator<TYPE>*
    {
        if (distance(parent, begin) * 2 + 2 <= distance(range, begin))
        {
            auto* left = frontOf(begin, distance(parent, begin) * 2 + 1);
            auto* right = frontOf(begin, distance(parent, begin) * 2 + 2);
            if (compare(left, right, compares))
            {
                delete right;
                return left;
            }
            delete left;
            return right;
        }
        return frontOf(begin, distance(parent, begin) * 2 + 1);
    }

#endif // ALGORITHMS_H
