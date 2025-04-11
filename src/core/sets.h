#ifndef SETS_H
#define SETS_H

#include "allocator.h"
#include "couple.h"
#include "hash.h"
#include "hashTable.h"
#include "set.h"


namespace original {
    template <typename TYPE,
              typename HASH = hash<TYPE>,
              typename ALLOC = allocator<couple<TYPE, std::void_t<TYPE>>>>
    class hashSet final : public hashTable<TYPE, std::void_t<TYPE>, ALLOC, HASH>,
                          public set<TYPE, ALLOC> {
    public:
        explicit hashSet(HASH hash = HASH{}, ALLOC alloc = ALLOC{});

        [[nodiscard]] u_integer size() const override;

        bool contains(const TYPE &e) const override;

        bool add(const TYPE &e) override;

        bool remove(const TYPE &e) override;
    };
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::hashSet(HASH hash, ALLOC alloc)
    : hashTable<TYPE, std::void_t<TYPE>, HASH, ALLOC>(std::move(hash)),
      set<TYPE, ALLOC>(std::move(alloc)) {}

template<typename TYPE, typename HASH, typename ALLOC>
original::u_integer original::hashSet<TYPE, HASH, ALLOC>::size() const {
    return this->size_;
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::contains(const TYPE &e) const {
    return this->find(e);
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::add(const TYPE &e) {
    return this->insert(e, std::void_t<TYPE>{});
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::remove(const TYPE &e) {
    return this->erase(e);
}

#endif //SETS_H
