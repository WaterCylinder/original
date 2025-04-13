#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "allocator.h"
#include "couple.h"
#include "hash.h"
#include "singleDirectionIterator.h"
#include "vector.h"
#include "wrapper.h"


namespace original {
    // TODO
    template<typename K_TYPE, typename V_TYPE, typename ALLOC = allocator<K_TYPE>, typename HASH = hash<K_TYPE>>
    class hashTable : public printable{
    protected:
        class hashNode final : public wrapper<couple<K_TYPE, V_TYPE>> {
            couple<K_TYPE, V_TYPE> data_;
            hashNode* next_;
        public:
            explicit hashNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{}, hashNode* next = nullptr);

            hashNode(const hashNode& other);

            hashNode& operator=(const hashNode& other);

            couple<K_TYPE, V_TYPE>& getVal() override;

            const couple<K_TYPE, V_TYPE>& getVal() const override;

            const K_TYPE& getKey() const;

            const V_TYPE& getValue() const;

            V_TYPE& getValue();

            void setVal(couple<K_TYPE, V_TYPE> data) override;

            void setValue(const V_TYPE& value);

            hashNode* getPPrev() const override;

            hashNode* getPNext() const override;

            void setPNext(hashNode* new_next);

            static void connect(hashNode* prev, hashNode* next);
        };

        using rebind_alloc_node = typename ALLOC::template rebind_alloc<hashNode>;
        using rebind_alloc_pointer = typename ALLOC::template rebind_alloc<hashNode*>;
        static constexpr floating LOAD_FACTOR_MIN = 0.25;
        static constexpr floating LOAD_FACTOR_MAX = 0.75;
        static constexpr u_integer BUCKETS_SIZES_COUNT = 30;
        static constexpr u_integer BUCKETS_SIZES[] = {
                17,          29,          53,          97,          193,
                389,         769,         1543,        3079,        6151,
                12289,       24593,       49157,       98317,       196613,

                393241,      786433,      1572869,     3145739,     6291469,
                12582917,    25165843,    50331653,    100663319,   201326611,

                402653189,   805306457,   1610612741,  3221225473,  4294967291
        };

        u_integer size_;
        vector<hashNode*, rebind_alloc_pointer> buckets;
        HASH hash_;
        rebind_alloc_node rebind_alloc{};

        class Iterator : public baseIterator<couple<K_TYPE, V_TYPE>> {
        protected:
            mutable vector<hashNode*, rebind_alloc_pointer>* p_buckets;
            mutable u_integer cur_bucket;
            mutable hashNode* p_node;

            static u_integer findNextValidBucket(vector<hashNode *, rebind_alloc_pointer> *buckets,
                                                                      u_integer bucket);

            static u_integer findPrevValidBucket(vector<hashNode *, rebind_alloc_pointer> *buckets,
                                                                     u_integer bucket);

            explicit Iterator(vector<hashNode*, rebind_alloc_pointer>* buckets = nullptr,
                              u_integer bucket = 0, hashNode* node = nullptr);

            bool equalPtr(const iterator<couple<K_TYPE, V_TYPE>>* other) const override;

            Iterator(const Iterator& other);

            Iterator& operator=(const Iterator& other);

            Iterator* clone() const override;
        public:
            [[nodiscard]] bool hasNext() const override;

            [[nodiscard]] bool hasPrev() const override;

            void next() const override;

            void prev() const override;

            void operator+=(integer steps) const override;

            void operator-=(integer steps) const override;

            integer operator-(const iterator<couple<K_TYPE, V_TYPE>>& other) const override;

            Iterator* getPrev() const override;

            Iterator* getNext() const override;

            couple<K_TYPE, V_TYPE>& get() override;

            couple<K_TYPE, V_TYPE> get() const override;

            void set(const couple<K_TYPE, V_TYPE>& data) override;

            [[nodiscard]] bool isValid() const override;

            bool atPrev(const iterator<couple<K_TYPE, V_TYPE>>* other) const override;

            bool atNext(const iterator<couple<K_TYPE, V_TYPE>>* other) const override;

            [[nodiscard]] std::string className() const override;
        };

        hashNode* createNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{}, hashNode* next = nullptr);

        void destroyNode(hashNode* node) noexcept;

        u_integer getHashCode(const K_TYPE& key) const;

        u_integer getBucketCount() const;

        hashNode* getBucket(const K_TYPE& key) const;

        floating loadFactor() const;

        u_integer getNextSize() const;

        u_integer getPrevSize() const;

        void rehash(u_integer new_bucket_count);

        void adjust();

        explicit hashTable(HASH hash = HASH{});

        hashNode* find(const K_TYPE& key) const;

        bool modify(const K_TYPE& key, const V_TYPE& value);

        bool insert(const K_TYPE& key, const V_TYPE& value);

        bool erase(const K_TYPE& key);

        [[nodiscard]] std::string className() const override;

        ~hashTable() override;
    };
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::hashNode(const K_TYPE& key, const V_TYPE& value, hashNode* next)
    : data_({key, value}), next_(next) {}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::hashNode(const hashNode& other) : hashNode() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode&
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::operator=(const hashNode& other) {
    if (this == &other)
        return *this;
    this->data_ = other.data_;
    this->next_ = other.next_;
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::couple<K_TYPE, V_TYPE>&
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getVal() {
    return this->data_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
const original::couple<K_TYPE, V_TYPE>&
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getVal() const {
    return this->data_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
const K_TYPE& original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getKey() const {
    return this->getVal().first();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
const V_TYPE& original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getValue() const {
    return this->getVal().second();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
V_TYPE& original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getValue() {
    return this->getVal().second();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::setVal(couple<K_TYPE, V_TYPE> data) {
    this->data_ = data;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::setValue(const V_TYPE &value) {
    this->data_.template set<1>(value);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getPPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::getPNext() const {
    return this->next_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::setPNext(hashNode *new_next) {
    this->next_ = new_next;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode::connect(hashNode *prev, hashNode *next) {
    if (prev != nullptr) prev->setPNext(next);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::u_integer
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::findNextValidBucket(
    vector<hashNode *, rebind_alloc_pointer> *buckets, const u_integer bucket) {
    for (u_integer i = bucket + 1; i < buckets->size(); i++) {
        if ((*buckets)[i])
            return i;
    }
    return buckets->size();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::u_integer
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::findPrevValidBucket(
    vector<hashNode*, rebind_alloc_pointer> *buckets, const u_integer bucket) {
    if (bucket == 0) return buckets->size();
    for (u_integer i = bucket - 1; i > 0; i--) {
        if ((*buckets)[i])
            return i;
    }
    if ((*buckets)[0]) {
        return 0;
    }
    return buckets->size();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::Iterator(
    vector<hashNode *, rebind_alloc_pointer> *buckets, const u_integer bucket, hashNode *node)
    : p_buckets(buckets), cur_bucket(bucket), p_node(node) {}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::equalPtr(
    const iterator<couple<K_TYPE, V_TYPE>> *other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    return other_it &&
           this->p_buckets == other_it->p_buckets &&
           this->cur_bucket == other_it->cur_bucket &&
           this->p_node == other_it->p_node;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::Iterator(const Iterator &other) : Iterator() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator&
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator=(const Iterator &other) {
    if (this == &other)
        return *this;

    this->p_buckets = other.p_buckets;
    this->p_node = other.p_node;
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::clone() const {
    return new Iterator(*this);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::hasNext() const {
    if (this->p_node && this->p_node->getPNext()) {
        return true;
    }

    return Iterator::findNextValidBucket(this->p_buckets, this->cur_bucket) != this->p_buckets->size();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::hasPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::next() const {
    if (!this->isValid()) {
        throw outOfBoundError();
    }

    if (this->p_node->getPNext()) {
        this->p_node = this->p_node->getPNext();
        return;
    }

    if (auto next_bucket = Iterator::findNextValidBucket(this->p_buckets, this->cur_bucket);
        next_bucket != this->p_buckets->size()) {
        this->cur_bucket = next_bucket;
        this->p_node = this->p_buckets->get(next_bucket);
        return;
    }

    this->cur_bucket = this->p_buckets->size();
    this->p_node = nullptr;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::prev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator+=(const integer steps) const {
    if (steps < 0) {
        throw unSupportedMethodError();
    }

    for (integer i = 0; i < steps; ++i) {
        this->next();
    }
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator-=(integer steps) const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::integer original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator-(
    const iterator<couple<K_TYPE, V_TYPE>> &other) const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::getPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::getNext() const {
    if (!this->isValid()) {
        throw outOfBoundError();
    }
    auto* new_it = this->clone();
    new_it->next();
    return new_it;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::couple<K_TYPE, V_TYPE>&
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::get() {
    if (!this->isValid()) {
        throw outOfBoundError();
    }

    return this->p_node->getVal();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::couple<K_TYPE, V_TYPE>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::get() const {
    if (!this->isValid()) {
        throw outOfBoundError();
    }

    return this->p_node->getVal();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::set(const couple<K_TYPE, V_TYPE> &data) {
    if (!this->isValid()) {
        throw outOfBoundError();
    }

    this->p_node->setVal(data);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::isValid() const {
    return this->p_node;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::atPrev(
    const iterator<couple<K_TYPE, V_TYPE>> *other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    if (!other_it) {
        return false;
    }

    return this->clone()->equalPtr(other_it);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::atNext(
    const iterator<couple<K_TYPE, V_TYPE>> *other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    if (!other_it) {
        return false;
    }

    return other_it->clone()->equalPtr(this);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
std::string original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::className() const {
    return "hashTable::Iterator";
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::createNode(const K_TYPE& key, const V_TYPE& value, hashNode* next) {
    auto node = this->rebind_alloc.allocate(1);
    this->rebind_alloc.construct(node, key, value, next);
    return node;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::destroyNode(hashNode* node) noexcept {
    this->rebind_alloc.destroy(node);
    this->rebind_alloc.deallocate(node, 1);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::u_integer
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::getHashCode(const K_TYPE &key) const {
    return this->hash_(key) % this->getBucketCount();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::u_integer
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::getBucketCount() const {
    return this->buckets.size();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::getBucket(const K_TYPE &key) const {
    u_integer code = this->getHashCode(key);
    return this->buckets[code];
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::floating
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::loadFactor() const {
    return static_cast<floating>(this->size_) / this->getBucketCount();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::u_integer original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::getNextSize() const {
    for (u_integer i : BUCKETS_SIZES){
        if (this->getBucketCount() < i){
            return i;
        }
    }
    throw outOfBoundError();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::u_integer original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::getPrevSize() const {
    const u_integer current = this->getBucketCount();
    for (u_integer i = BUCKETS_SIZES_COUNT - 1; i > 0; --i){
        if (BUCKETS_SIZES[i] < current){
            return BUCKETS_SIZES[i];
        }
    }
    return BUCKETS_SIZES[0];
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::rehash(u_integer new_bucket_count) {
    if (new_bucket_count == this->getBucketCount())
        return;

    auto new_buckets = vector<hashNode*, rebind_alloc_pointer>(new_bucket_count, rebind_alloc_pointer{}, nullptr);
    for (hashNode*& bucket: this->buckets){
        while (bucket){
            auto next = bucket->getPNext();
            auto cur = bucket;
            hashNode::connect(bucket, next);
            hashNode::connect(cur, nullptr);
            auto code = this->hash_(cur->getKey()) % new_bucket_count;
            hashNode::connect(cur, new_buckets[code]);
            new_buckets[code] = cur;
        }
    }
    this->buckets = std::move(new_buckets);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::adjust() {
    if (this->loadFactor() <= LOAD_FACTOR_MIN){
        this->rehash(this->getPrevSize());
    } else if (this->loadFactor() >= LOAD_FACTOR_MAX){
        this->rehash(this->getNextSize());
    }
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashTable(HASH hash)
    : size_(0), hash_(std::move(hash)) {
    this->buckets = vector<hashNode*, rebind_alloc_pointer>(BUCKETS_SIZES[0], rebind_alloc_pointer{}, nullptr);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
typename original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode*
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::find(const K_TYPE& key) const {
    if (this->size_ == 0)
        return nullptr;

    for (auto cur = this->getBucket(key); cur; cur = cur->getPNext()){
        if (cur->getKey() == key)
            return cur;
    }
    return nullptr;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::modify(const K_TYPE &key, const V_TYPE &value) {
    if (auto cur = this->find(key)){
        cur->setValue(value);
        return true;
    }
    return false;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::insert(const K_TYPE &key, const V_TYPE &value) {
    this->adjust();

    auto cur = this->getBucket(key);
    if (!cur){
        this->buckets[this->getHashCode(key)] = this->createNode(key, value);
    } else{
        for (; cur->getPNext(); cur = cur->getPNext()){
            if (cur->getKey() == key)
                return false;
        }
        hashNode::connect(cur, this->createNode(key, value));
    }

    this->size_ += 1;
    return true;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
bool original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::erase(const K_TYPE &key) {
    if (this->size_ == 0)
        return false;

    this->adjust();

    u_integer code = this->getHashCode(key);
    hashNode* cur = this->buckets[code];
    hashNode* prev = nullptr;

    while (cur){
        if (cur->getKey() == key) {
            if (prev) {
                hashNode::connect(prev, cur->getPNext());
            } else {
                this->buckets[code] = cur->getPNext();
            }
            this->destroyNode(cur);
            this->size_ -= 1;
            return true;
        }
        prev = cur;
        cur = cur->getPNext();
    }

    return false;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
std::string original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::className() const {
    return "hashTable";
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::~hashTable() {
    for (hashNode*& bucket: this->buckets) {
        while (bucket){
            auto next = bucket->getPNext();
            this->destroyNode(bucket);
            bucket = next;
        }
    }
}

#endif //HASHTABLE_H
