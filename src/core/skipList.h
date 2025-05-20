#ifndef SKIPLIST_H
#define SKIPLIST_H
#include <random>
#include "comparator.h"
#include "vector.h"
#include "couple.h"


namespace original {

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

          void expandLevels(u_integer new_levels);

          void shrinkLevels(u_integer new_levels);

          void setValue(const V_TYPE& value);

          skipListNode* getPNext(u_integer levels) const;

          skipListNode*& getPRef(u_integer levels);

          void setPNext(u_integer levels, skipListNode* next);

          static void connect(u_integer levels, skipListNode* prev, skipListNode* next);
      };

       using rebind_alloc_node = typename ALLOC::template rebind_alloc<skipListNode>;
       using rebind_alloc_pointer = typename ALLOC::template rebind_alloc<skipListNode*>;

       u_integer size_;
       skipListNode* head_;
       Compare compare_;
       mutable rebind_alloc_node rebind_alloc{};

       class Iterator {
       protected:
           mutable skipListNode* cur_;

           explicit Iterator(skipListNode* cur = nullptr);

           Iterator(const Iterator& other);

           Iterator& operator=(const Iterator& other);

           Iterator* clone() const override;

           [[nodiscard]] bool hasNext() const;

           void next() const;

           void operator+=(integer steps) const;

           integer operator-(const Iterator& other) const;

           couple<const K_TYPE, V_TYPE>& get();

           couple<const K_TYPE, V_TYPE> get() const;

           [[nodiscard]] bool isValid() const;

           static integer ptrDistance(const Iterator* start, const Iterator* end);
       };

       friend Iterator;

       skipListNode* createNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{},
                                u_integer levels = 1, std::initializer_list<skipListNode*> next = {}) const;

       void destroyNode(skipListNode* node) const;

       bool highPriority(skipListNode* cur, skipListNode* next) const;

       bool highPriority(const K_TYPE& key, skipListNode* next) const;

       static bool equal(const K_TYPE& key, skipListNode* next);

       static u_integer getRandomLevels();

       u_integer getCurLevels() const;

       void setCurLevels(u_integer new_levels);

       skipListNode* listCopy() const;

       skipListNode* findNode(const K_TYPE& key) const;

       explicit skipList(Compare compare = Compare{});

       skipListNode* find(const K_TYPE& key) const;

       bool modify(const K_TYPE& key, const V_TYPE& value);

       bool insert(const K_TYPE& key, const V_TYPE& value);

       bool erase(const K_TYPE& key);

       void listDestroy() noexcept;

       ~skipList();
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

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::expandLevels(original::u_integer new_levels) {
    if (this->getLevels() >= new_levels){
        return;
    }
    auto increment = new_levels - this->getLevels();
    for (u_integer i = 0; i < increment; ++i) {
        this->next_.pushEnd(nullptr);
    }
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::shrinkLevels(original::u_integer new_levels) {
    if (new_levels >= this->getLevels() || new_levels == 0){
        return;
    }
    auto decrement = this->getLevels() - new_levels;
    for (u_integer i = 0; i < decrement; ++i) {
        this->next_.popEnd();
    }
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
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode::getPRef(const u_integer levels) {
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

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::Iterator(skipListNode* cur)
    : cur_(cur) {}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::Iterator(const Iterator& other) {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::operator=(const Iterator& other) {
    if (this == &other){
        return *this;
    }

    this->cur_ = other.cur_;
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator*
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::clone() const {
    return new Iterator(*this);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::hasNext() const {
    return this->cur_->getPNext(1);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::next() const {
    this->cur_ = this->cur_->getPNext(1);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::operator+=(original::integer steps) const {
    if (steps < 0){
        throw unSupportedMethodError();
    }

    for (integer i = 0; i < steps; ++i) {
        this->next();
    }
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::integer
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::operator-(const Iterator& other) const {
    if (const integer pos_dis = ptrDistance(&other, this);
            pos_dis != std::numeric_limits<integer>::max()) return pos_dis;
    if (const integer neg_dis = ptrDistance(this, &other);
            neg_dis != std::numeric_limits<integer>::max()) return -neg_dis;
    return this->cur_ > other.cur_ ?
           std::numeric_limits<integer>::max() :
           std::numeric_limits<integer>::min();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::couple<const K_TYPE, V_TYPE>&
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::get() {
    if (!this->isValid()){
        throw outOfBoundError();
    }

    return this->cur_->getVal();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::couple<const K_TYPE, V_TYPE>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::get() const {
    if (!this->isValid()){
        throw outOfBoundError();
    }

    return this->cur_->getVal();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::isValid() const {
    return this->cur_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::integer
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::ptrDistance(const Iterator* start, const Iterator* end) {
    auto s = ownerPtr(start->clone());
    auto e = ownerPtr(end->clone());
    integer dis = 0;
    while (s->isValid()){
        if (s->_ptr == e->_ptr){
            return dis;
        }
        dis += 1;
        s->next();
    }
    if (e->isValid()){
        dis = std::numeric_limits<integer>::max();
    }
    return dis;
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
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::highPriority(skipListNode* cur, skipListNode* next) const
{
    if (!cur){
        return false;
    }
    return this->highPriority(cur->getKey(), next);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::highPriority(const K_TYPE& key, skipListNode* next) const
{
    if (!next){
        return true;
    }
    return this->compare_(key, next->getKey());
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::equal(const K_TYPE& key, skipListNode* next)
{
    if (!next) {
        return false;
    }

    return key == next->getKey();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::u_integer original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::getRandomLevels()
{
    constexpr floating p = 0.5;
    u_integer levels = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<floating> dis(0, 1);

    while (dis(gen) < p) {
        levels += 1;
    }
    return levels;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::u_integer
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::getCurLevels() const {
    return this->head_->getLevels();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::setCurLevels(original::u_integer new_levels) {
    if (this->getCurLevels() > new_levels){
        this->head_->shrinkLevels(new_levels);
    } else if (this->getCurLevels() < new_levels){
        this->head_->expandLevels(new_levels);
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*
original::skipList<K_TYPE, V_TYPE, ALLOC,Compare>::listCopy() const {
    auto copied_head =
    this->createNode(this->head_->getKey(), this->head_->getValue(), this->getCurLevels());

    vector<skipListNode*> copied_curs{this->getCurLevels(), rebind_alloc_pointer{}, copied_head};
    auto src_cur = this->head_;
    while (src_cur->getPNext(1)){
        auto src_next = src_cur->getPNext(1);
        auto copied_next = this->createNode(src_next->getKey(), src_next->getValue(), src_next->getLevels());
        for (u_integer i = 0; i < src_next->getLevels(); ++i) {
            skipListNode::connect(i + 1, copied_curs[i], copied_next);
            copied_curs[i] = copied_next;
        }
    }

    return copied_head;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::findNode(const K_TYPE &key) const {
    u_integer levels = this->getCurLevels();
    auto cur_p = this->head_;
    while (levels > 0) {
        while (cur_p->getPNext(levels)) {
            auto next_p = cur_p->getPNext(levels);
            if (this->highPriority(key, next_p)) {
                break;
            }
            if (equal(key, next_p)) {
                return next_p;
            }
            cur_p = next_p;
        }

        levels -= 1;
    }
    return cur_p;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipList(Compare compare)
    : size_(0), head_(this->createNode()), compare_(std::move(compare)) {}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::skipListNode*
original::skipList<K_TYPE, V_TYPE, ALLOC,Compare>::find(const K_TYPE& key) const
{
    auto cur_p = this->findNode(key);
    return cur_p && equal(key, cur_p) ? cur_p : nullptr;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::modify(const K_TYPE& key, const V_TYPE& value)
{
    auto cur_p = this->findNode(key);
    if (!cur_p || !equal(key, cur_p)){
        return false;
    }

    cur_p->setValue(value);
    return true;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::insert(const K_TYPE& key, const V_TYPE& value)
{
    auto cur_p = this->findNode(key);
    if (cur_p && equal(key, cur_p)){
        return false;
    }

    auto new_levels = getRandomLevels();
    auto new_node = this->createNode(key, value, new_levels);
    this->setCurLevels(new_levels);
    auto min_levels = min(this->getCurLevels(), new_levels);
    vector<skipListNode*> prev_nodes{min_levels, rebind_alloc_pointer{}, this->head_};
    vector<skipListNode*> next_nodes{min_levels, rebind_alloc_pointer{}, nullptr};
    for (u_integer i = 0; i < min_levels; ++i) {
        while (true){
            skipListNode* cur_node = prev_nodes[i];
            skipListNode* next_node = cur_node->getPNext(i + 1);
            if (!next_node || this->highPriority(key, next_node)){
                next_nodes[i] = next_node;
                break;
            }
            prev_nodes[i] = next_node;
        }
    }
    for (u_integer i = 0; i < min_levels; ++i) {
        skipListNode::connect(i + 1, prev_nodes[i], new_node);
        skipListNode::connect(i + 1, new_node, next_nodes[i]);
    }

    this->size_ += 1;
    return true;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::erase(const K_TYPE& key)
{
    auto cur_p = this->findNode(key);
    if (!cur_p || !equal(key, cur_p)){
        return false;
    }

    vector<skipListNode*> prev_nodes{cur_p->getLevels(), rebind_alloc_pointer{}, this->head_};
    vector<skipListNode*> next_nodes{cur_p->getLevels(), rebind_alloc_pointer{}, nullptr};
    for (u_integer i = 0; i < cur_p->getLevels(); ++i) {
        next_nodes[i] = cur_p->getPNext(i + 1);
        while (true){
            skipListNode* cur_node = prev_nodes[i];
            skipListNode* next_node = cur_node->getPNext(i + 1);
            if (!next_node || !this->highPriority(next_node, cur_p)){
                break;
            }
            prev_nodes[i] = next_node;
        }
    }
    for (u_integer i = 0; i < cur_p->getLevels(); ++i) {
        skipListNode::connect(i + 1, prev_nodes[i], next_nodes[i]);
    }
    this->destroyNode(cur_p);

    u_integer decrement = 0;
    for (u_integer i = this->getCurLevels() - 1; i > 0; --i) {
        if (!this->head_->getPNext(i)){
            break;
        }
        decrement += 1;
    }
    this->setCurLevels(this->getCurLevels() - decrement);
    this->size_ -= 1;
    return true;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::listDestroy() noexcept
{
    auto cur = this->head_;
    while (cur) {
        auto next = cur->getPNext(1);
        this->destroyNode(cur);
        cur = next;
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::skipList<K_TYPE, V_TYPE, ALLOC, Compare>::~skipList()
{
    this->listDestroy();
}

#endif //SKIPLIST_H
