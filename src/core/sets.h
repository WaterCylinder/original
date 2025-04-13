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
                          public set<TYPE, ALLOC>,
                          public iterable<couple<const TYPE, bool>> {
        using hashNode = typename hashTable<TYPE, bool, ALLOC, HASH>::hashNode;
        using rebind_alloc_pointer = typename hashTable<TYPE, bool, ALLOC>::rebind_alloc_pointer;
    public:
        class Iterator final : public hashTable<TYPE, bool, ALLOC, HASH>::Iterator {

            explicit Iterator(vector<hashNode*, rebind_alloc_pointer>* buckets = nullptr,
                              u_integer bucket = 0, hashNode* node = nullptr);
        public:
            friend class hashSet;

            Iterator(const Iterator& other);

            Iterator& operator=(const Iterator& other);

            Iterator* clone() const override;

            [[nodiscard]] std::string className() const override;
        };
        explicit hashSet(HASH hash = HASH{}, ALLOC alloc = ALLOC{});

        hashSet(const hashSet& other);

        hashSet& operator=(const hashSet& other);

        hashSet(hashSet&& other) noexcept;

        hashSet& operator=(hashSet&& other) noexcept;

        [[nodiscard]] u_integer size() const override;

        bool contains(const TYPE &e) const override;

        bool add(const TYPE &e) override;

        bool remove(const TYPE &e) override;

        Iterator* begins() const override;

        Iterator* ends() const override;

        ~hashSet() override;
    };
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::Iterator::Iterator(vector<hashNode*, rebind_alloc_pointer> *buckets,
u_integer bucket, hashNode *node) : hashTable<TYPE, bool, ALLOC>::Iterator(
    const_cast<vector<hashNode*, rebind_alloc_pointer>*>(buckets), bucket, node) {}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::Iterator::Iterator(const Iterator &other) : Iterator() {
    this->operator=(other);
}

template<typename TYPE, typename HASH, typename ALLOC>
typename original::hashSet<TYPE, HASH, ALLOC>::Iterator&
original::hashSet<TYPE, HASH, ALLOC>::Iterator::operator=(const Iterator &other) {
    if (this == &other) {
        return *this;
    }

    hashTable<TYPE, bool, ALLOC, HASH>::Iterator::operator=(other);
    return *this;
}

template<typename TYPE, typename HASH, typename ALLOC>
typename original::hashSet<TYPE, HASH, ALLOC>::Iterator*
original::hashSet<TYPE, HASH, ALLOC>::Iterator::clone() const {
    return new Iterator(*this);
}

template<typename TYPE, typename HASH, typename ALLOC>
std::string original::hashSet<TYPE, HASH, ALLOC>::Iterator::className() const {
    return "hashSet::Iterator";
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

template<typename TYPE, typename HASH, typename ALLOC>
typename original::hashSet<TYPE, HASH, ALLOC>::Iterator*
original::hashSet<TYPE, HASH, ALLOC>::begins() const {
    auto p_buckets = const_cast<vector<hashNode*, rebind_alloc_pointer>*>(&this->buckets);
    if (this->buckets[0]) {
        return new Iterator(p_buckets, 0, this->buckets[0]);
    }
    auto bucket = Iterator::findNextValidBucket(p_buckets, 0);
    return new Iterator(p_buckets, bucket, p_buckets->get(bucket));
}

template<typename TYPE, typename HASH, typename ALLOC>
typename original::hashSet<TYPE, HASH, ALLOC>::Iterator*
original::hashSet<TYPE, HASH, ALLOC>::ends() const {
    auto p_buckets = const_cast<vector<hashNode*, rebind_alloc_pointer>*>(&this->buckets);
    auto bucket = Iterator::findPrevValidBucket(p_buckets, this->buckets.size());
    auto node = this->buckets[bucket];
    while (node && node->getPNext()) {
        node = node->getPNext();
    }
    return new Iterator(p_buckets, bucket, node);
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::~hashSet() = default;

#endif //SETS_H
