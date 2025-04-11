#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "allocator.h"
#include "couple.h"
#include "hash.h"
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
            friend class hashTable;

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
        static constexpr u_integer BUCKETS_SIZE_INIT = 16;
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
    u_integer current = this->getBucketCount();
    for (u_integer i = BUCKETS_SIZES_COUNT - 1; i > 0; --i){
        if (BUCKETS_SIZES[i] < current){
            return BUCKETS_SIZES[i];
        }
    }
    return BUCKETS_SIZES[0];
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename HASH>
void original::hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::rehash(original::u_integer new_bucket_count) {
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
    this->buckets = vector<hashNode*, rebind_alloc_pointer>(BUCKETS_SIZE_INIT, rebind_alloc_pointer{}, nullptr);
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
