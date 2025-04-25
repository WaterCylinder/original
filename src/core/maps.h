#ifndef MAPS_H
#define MAPS_H

#include "allocator.h"
#include "couple.h"
#include "hash.h"
#include "hashTable.h"
#include "map.h"
#include "ownerPtr.h"


namespace original {
    template <typename K_TYPE,
              typename V_TYPE,
              typename HASH = hash<K_TYPE>,
              typename ALLOC = allocator<couple<const K_TYPE, V_TYPE>>>
    class hashMap final
                : public hashTable<K_TYPE, V_TYPE, ALLOC, HASH>,
                  public map<K_TYPE, V_TYPE, ALLOC>,
                  public iterable<couple<const K_TYPE, V_TYPE>>{

            using hashNode = typename hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode;
            using rebind_alloc_pointer = typename hashTable<K_TYPE, V_TYPE, ALLOC>::rebind_alloc_pointer;
    public:
            class Iterator final : public hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator, public baseIterator<couple<const K_TYPE, V_TYPE>> {

                explicit Iterator(vector<hashNode*, rebind_alloc_pointer>* buckets = nullptr,
                                  u_integer bucket = 0, hashNode* node = nullptr);

                bool equalPtr(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;
            public:
                friend class hashMap;

                Iterator(const Iterator& other);

                Iterator& operator=(const Iterator& other);

                Iterator* clone() const override;

                [[nodiscard]] std::string className() const override;

                void operator+=(integer steps) const override;

                void operator-=(integer steps) const override;

                integer operator-(const iterator<couple<const K_TYPE, V_TYPE>> &other) const override;

                [[nodiscard]] bool hasNext() const override;

                [[nodiscard]] bool hasPrev() const override;

                bool atPrev(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;

                bool atNext(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;

                void next() const override;

                void prev() const override;

                Iterator* getPrev() const override;

                couple<const K_TYPE, V_TYPE> &get() override;

                couple<const K_TYPE, V_TYPE> get() const override;

                void set(const couple<const K_TYPE, V_TYPE> &data) override;

                [[nodiscard]] bool isValid() const override;

                ~Iterator() override = default;
            };

            explicit hashMap(HASH hash = HASH{}, ALLOC alloc = ALLOC{});

            hashMap(const hashMap& other);

            hashMap& operator=(const hashMap& other);

            hashMap(hashMap&& other) noexcept;

            hashMap& operator=(hashMap&& other) noexcept;

            [[nodiscard]] u_integer size() const override;

            bool contains(const couple<const K_TYPE, V_TYPE> &e) const override;

            bool add(const K_TYPE &k, const V_TYPE &v) override;

            bool remove(const K_TYPE &k) override;

            [[nodiscard]] bool containsKey(const K_TYPE &k) const override;

            V_TYPE get(const K_TYPE &k) const override;

            const V_TYPE & operator[](const K_TYPE &k) const override;

            V_TYPE & operator[](const K_TYPE &k) override;

            Iterator* begins() const override;

            Iterator* ends() const override;

            [[nodiscard]] std::string className() const override;

            [[nodiscard]] std::string toString(bool enter) const override;

            ~hashMap() override;
        };
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::Iterator(
    vector<hashNode *, rebind_alloc_pointer> *buckets, u_integer bucket, hashNode *node)
    : hashTable<K_TYPE, V_TYPE, ALLOC>::Iterator(
        const_cast<vector<hashNode*, rebind_alloc_pointer>*>(buckets), bucket, node) {}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::equalPtr(
        const iterator<couple<const K_TYPE, V_TYPE>>* other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    return other_it &&
           this->p_buckets == other_it->p_buckets &&
           this->cur_bucket == other_it->cur_bucket &&
           this->p_node == other_it->p_node;
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::Iterator(const Iterator &other) : Iterator() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
typename original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator&
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator=(const Iterator &other) {
    if (this == &other)
        return *this;

    hashTable<K_TYPE, V_TYPE, ALLOC>::Iterator::operator=(other);
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
typename original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator*
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::clone() const {
    return new Iterator(*this);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
std::string original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::className() const {
    return "hashMap::Iterator";
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator+=(original::integer steps) const {
    hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator+=(steps);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator-=(original::integer) const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::integer original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator-(
        const original::iterator<original::couple<const K_TYPE, V_TYPE>>&) const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::hasNext() const {
    return hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::hasNext();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::hasPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::atPrev(
        const original::iterator<original::couple<const K_TYPE, V_TYPE>> *other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    if (!other_it) {
        return false;
    }
    auto next = ownerPtr(this->clone());
    if (!next->isValid()){
        return false;
    }

    next->next();
    return next->equalPtr(other_it);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::atNext(
        const original::iterator<original::couple<const K_TYPE, V_TYPE>> *other) const {
    return other->atPrev(*this);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::next() const {
    hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::next();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::prev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator*
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::getPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::couple<const K_TYPE, V_TYPE>&
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::get() {
    return hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::get();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::couple<const K_TYPE, V_TYPE>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::get() const {
    return hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::get();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::set(const original::couple<const K_TYPE, V_TYPE>&) {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::isValid() const {
    return hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::isValid();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::hashMap(HASH hash, ALLOC alloc)
    : hashTable<K_TYPE, V_TYPE, ALLOC, HASH>(std::move(hash)),
      map<K_TYPE, V_TYPE, ALLOC>(std::move(alloc)) {}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::hashMap(const hashMap &other) : hashMap() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>&
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::operator=(const hashMap &other) {
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

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::hashMap(hashMap &&other) noexcept : hashMap() {
    this->operator=(std::move(other));
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>&
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::operator=(hashMap &&other) noexcept {
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

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::u_integer
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::size() const {
    return this->size_;
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::contains(const couple<const K_TYPE, V_TYPE> &e) const {
    return this->containsKey(e.first()) && this->get(e.first()) == e.second();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::add(const K_TYPE &k, const V_TYPE &v) {
    return this->insert(k, v);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::remove(const K_TYPE &k) {
    return this->erase(k);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::containsKey(const K_TYPE &k) const {
    return this->find(k);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
V_TYPE original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::get(const K_TYPE &k) const {
    auto node = this->find(k);
    if (!node)
        throw noElementError();
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
const V_TYPE& original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::operator[](const K_TYPE &k) const {
    auto node = this->find(k);
    if (!node)
        throw noElementError();
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
V_TYPE& original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::operator[](const K_TYPE &k) {
    auto node = this->find(k);
    if (!node) {
        this->insert(k, V_TYPE{});
        node = this->find(k);
    }
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
typename original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator*
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::begins() const {
    auto p_buckets = const_cast<vector<hashNode*, rebind_alloc_pointer>*>(&this->buckets);
    if (this->buckets[0]) {
        return new Iterator(p_buckets, 0, this->buckets[0]);
    }
    auto bucket = Iterator::findNextValidBucket(p_buckets, 0);
    return new Iterator(p_buckets, bucket, p_buckets->get(bucket));
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
typename original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator*
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::ends() const {
    auto p_buckets = const_cast<vector<hashNode*, rebind_alloc_pointer>*>(&this->buckets);
    auto bucket = Iterator::findPrevValidBucket(p_buckets, this->buckets.size());
    auto node = this->buckets[bucket];
    while (node && node->getPNext()) {
        node = node->getPNext();
    }
    return new Iterator(p_buckets, bucket, node);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
std::string original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::className() const {
    return "hashMap";
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
std::string original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::toString(bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(";
    bool first = true;
    for (auto it = this->begin(); it != this->end(); it.next()){
        if (!first){
            ss << ", ";
        }
        ss << "{" << it.get().template get<0>() << ": " << it.get().template get<1>() << "}";
        first = false;
    }
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::~hashMap() = default;

#endif //MAPS_H
