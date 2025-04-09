#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "allocator.h"
#include "comparator.h"
#include "hash.h"
#include "vector.h"
#include "wrapper.h"


namespace original {
    // TODO
    template<typename TYPE, typename COMP = increaseComparator<TYPE>, typename ALLOC = allocator<TYPE>, typename HASH = hash<TYPE>>
    class hashTable {

        class hashNode final : public wrapper<TYPE> {
        private:
            TYPE data_;
            hashNode* next_;
        public:
            friend class hashTable;

            explicit hashNode(const TYPE& data = TYPE{}, hashNode* next = nullptr);

            hashNode(const hashNode& other);

            hashNode& operator=(const hashNode& other);

            TYPE& getVal() override;

            const TYPE& getVal() const override;

            void setVal(TYPE data) override;

            hashNode* getPPrev() const override;

            hashNode* getPNext() const override;

            void setPNext(hashNode* new_next);

            static void connect(hashNode* prev, hashNode* next);
        };

        using rebind_alloc_node = typename ALLOC::template rebind_alloc<hashNode>;
        using rebind_alloc_pointer = typename ALLOC::template rebind_alloc<hashNode*>;
        static constexpr u_integer BUCKETS_SIZE_INIT = 16;

        u_integer size_;
        vector<hashNode*, rebind_alloc_pointer> buckets;
        COMP comp_;
        HASH hash_;
        rebind_alloc_node rebind_alloc{};

    protected:
        hashTable(COMP comp = COMP{}, HASH hash = HASH{});
    };
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::hashNode(const TYPE &data, hashNode* next)
    : data_(data), next_(next) {}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::hashNode(const hashNode& other) : hashNode() {
    this->operator=(other);
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode&
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::operator=(const hashNode& other) {
    if (this == &other)
        return *this;
    this->data_ = other.data_;
    this->next_ = other.next_;
    return *this;
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
TYPE &original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::getVal() {
    return this->data_;
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
const TYPE &original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::getVal() const {
    return this->data_;
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
void original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::setVal(TYPE data) {
    this->data_ = data;
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode*
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::getPPrev() const {
    throw unSupportedMethodError();
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode*
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::getPNext() const {
    return this->next_;
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
void original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::setPNext(hashNode *new_next) {
    this->next_ = new_next;
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
void original::hashTable<TYPE, COMP, ALLOC, HASH>::hashNode::connect(hashNode *prev, hashNode *next) {
    if (prev != nullptr) prev->setPNext(next);
}

template<typename TYPE, typename COMP, typename ALLOC, typename HASH>
original::hashTable<TYPE, COMP, ALLOC, HASH>::hashTable(COMP comp, HASH hash)
    : size_(0), comp_(std::move(comp)), hash_(std::move(hash)) {
    this->buckets = vector<hashNode*, rebind_alloc_pointer>(BUCKETS_SIZE_INIT, rebind_alloc{}, nullptr);
}

#endif //HASHTABLE_H
