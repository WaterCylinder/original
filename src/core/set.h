#ifndef SET_H
#define SET_H


#include "allocator.h"
#include "container.h"
#include "comparator.h"

namespace original {
    template <typename K_TYPE, typename COMP = increaseComparator<K_TYPE>, typename ALLOC = allocator<K_TYPE>>
    class set : public container<K_TYPE, ALLOC> {
    protected:
        COMP compare;

        explicit set(COMP comp = COMP{}, ALLOC alloc = ALLOC{});
    public:
        virtual bool add(const K_TYPE& e) = 0;

        virtual bool remove(const K_TYPE& e) = 0;

        ~set() override;
    };
}

template<typename K_TYPE, typename COMP, typename ALLOC>
original::set<K_TYPE, COMP, ALLOC>::set(COMP comp, ALLOC alloc)
    : container<K_TYPE, ALLOC>(std::move(alloc)), compare(std::move(comp)) {}

template<typename K_TYPE, typename COMP, typename ALLOC>
original::set<K_TYPE, COMP, ALLOC>::~set() = default;

#endif //SET_H
