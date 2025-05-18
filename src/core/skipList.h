#ifndef SKIPLIST_H
#define SKIPLIST_H
#include "comparator.h"
#include "vector.h"
#include "couple.h"


namespace original {

    // todo
   template<typename K_TYPE,
            typename V_TYPE,
            typename ALLOC = allocator<K_TYPE>,
            typename Compare = increaseComparator<K_TYPE>>
   class skipList {
   protected:
      class skipListNode {
          couple<const K_TYPE, V_TYPE> data_;
          vector<skipListNode*> next_;

          using rebind_alloc_pointer = typename ALLOC::template rebind_alloc<skipListNode*>;
      public:
          friend class skipList;

          explicit skipListNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{},
                                u_integer levels = 1, std::initializer_list<skipListNode*> next = {});

          couple<const K_TYPE, V_TYPE>& getVal();

          const couple<const K_TYPE, V_TYPE>& getVal() const;

          const K_TYPE& getKey() const;

          const V_TYPE& getValue() const;

          V_TYPE& getValue();

          [[nodiscard]] u_integer getLevels() const;

          void setValue(const V_TYPE& value);

          skipListNode* getPNext(u_integer levels) const;

          skipListNode*& getPNextRef(u_integer levels);

          void setPNext(u_integer levels, skipListNode* next);

          static void connect(u_integer levels, skipListNode* prev, skipListNode* next);
      };

       using rebind_alloc_node = typename ALLOC::template rebind_alloc<skipListNode>;
       using rebind_alloc_pointer = typename ALLOC::template rebind_alloc<skipListNode*>;

       u_integer cur_levels_;
       u_integer size_;
       skipListNode* head_;
       mutable rebind_alloc_node rebind_alloc{};

       // todo
       class Iterator {

       };

       friend Iterator;

       skipListNode* createNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{},
                                u_integer levels = 1, std::initializer_list<skipListNode*> next = {}) const;

       void destroyNode(skipListNode* node) const;

       skipListNode* listCopy() const;
   };
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::skipListNode(const K_TYPE& key, const V_TYPE& value,
    u_integer levels, std::initializer_list<skipListNode*> next)
    : data_({key, value}), next_(vector<skipListNode*>(levels, rebind_alloc_pointer{}, nullptr)) {
    if (next.size() != 0 && static_cast<u_integer>(next.size()) != levels) {
        throw outOfBoundError();
    }
    u_integer i = 0;
    for (auto&& ptr: next) {
        this->next_[i] = ptr;
        i += 1;
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::couple<const K_TYPE, V_TYPE>&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getVal() {
    return this->data_;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
const original::couple<const K_TYPE, V_TYPE>&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getVal() const {
    return this->data_;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
const K_TYPE&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getKey() const {
    return this->data_.template get<0>();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
const V_TYPE&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getValue() const {
    return this->data_.template get<1>();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
V_TYPE&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getValue() {
    return this->data_.template get<1>();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::u_integer original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getLevels() const {
    return this->next_.size();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::setValue(const V_TYPE& value) {
    this->data_.template set<1>(value);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*
original::skipList<K_TYPE, V_TYPE, ALLOC,Compare>::skipListNode::getPNext(const u_integer levels) const {
    return this->next_[levels - 1];
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getPNextRef(const u_integer levels) {
    return this->next_[levels - 1];
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::setPNext(const u_integer levels, skipListNode* next)
{
    this->next_[levels - 1] = next;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::connect(const u_integer levels, skipListNode* prev, skipListNode* next)
{
    if (prev) {
        prev->setPNext(levels, next);
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*
original::skipList<K_TYPE, V_TYPE, ALLOC,Compare>::createNode(const K_TYPE& key, const V_TYPE& value, u_integer levels,
    std::initializer_list<skipListNode*> next) const {
    auto node = this->rebind_alloc.allocate(1);
    this->rebind_alloc.construct(node, key, value, levels, next);
    return node;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::destroyNode(skipListNode* node) const {
    this->rebind_alloc.destroy(node);
    this->rebind_alloc.deallocate(node, 1);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*
original::skipList<K_TYPE, V_TYPE, ALLOC,Compare>::listCopy() const {
    auto copied_head =
    this->createNode(this->head_->getKey(), this->head_->getValue(), this->head_->getLevels());

    vector<skipListNode**> copied_curs{this->head_->getLevels(), rebind_alloc_pointer{}, nullptr};
    for (u_integer i = 0; i < copied_head->getLevels(); ++i) {
        copied_curs[i] = &copied_head->getPNextRef(i + 1);
    }

    auto src_cur = this->head_;
    while (src_cur->getPNext(1)) {
        auto src_next = src_cur->getPNext(1);
        auto copied_next = this->createNode(src_next->getKey(), src_next->getValue(), src_next->getLevels());
        for (u_integer i = 0; i < src_next->getLevels(); ++i) {
            *copied_curs[i] = copied_next;
            copied_curs[i] = &copied_next->getPNextRef(i + 1);
        }
    }

    return copied_head;
}

#endif //SKIPLIST_H
