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
              typename ALLOC = allocator<couple<const TYPE, bool>>>
    class hashSet final : public hashTable<TYPE, bool, ALLOC, HASH>,
                          public set<TYPE, ALLOC> {
    public:
        explicit hashSet(HASH hash = HASH{}, ALLOC alloc = ALLOC{});

        hashSet(const hashSet& other);

        hashSet& operator=(const hashSet& other);

        hashSet(hashSet&& other) noexcept;

        hashSet& operator=(hashSet&& other) noexcept;

        [[nodiscard]] u_integer size() const override;

        bool contains(const TYPE &e) const override;

        bool add(const TYPE &e) override;

        bool remove(const TYPE &e) override;
    };
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::hashSet(HASH hash, ALLOC alloc)
    : hashTable<TYPE, bool, ALLOC, HASH>(std::move(hash)),
      set<TYPE, ALLOC>(std::move(alloc)) {}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::hashSet(const hashSet &other) : hashSet() {
    this->operator=(other);
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>&
original::hashSet<TYPE, HASH, ALLOC>::operator=(const hashSet &other) {
    if (this == &other) {
        return *this;
    }

    this->buckets = other.buckets;
    this->size_ = other.size_;
    if constexpr(ALLOC::propagate_on_container_copy_assignment::value) {
        this->allocator = other.allocator;
        this->rebind_alloc = other.rebind_alloc;
    }
    return *this;
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::hashSet(hashSet &&other) noexcept : hashSet() {
    this->operator=(std::move(other));
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>&
original::hashSet<TYPE, HASH, ALLOC>::operator=(hashSet &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    this->buckets = std::move(other.buckets);
    this->size_ = other.size_;
    if constexpr(ALLOC::propagate_on_container_move_assignment::value) {
        this->allocator = std::move(other.allocator);
        this->rebind_alloc = std::move(other.rebind_alloc);
    }
    return *this;
}

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
    return this->insert(e, true);
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::remove(const TYPE &e) {
    return this->erase(e);
}

#endif //SETS_H
