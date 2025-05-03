#ifndef RBTREE_H
#define RBTREE_H

#include "allocator.h"
#include "comparator.h"
#include "couple.h"
#include "queue.h"


namespace original {
    template<typename K_TYPE,
             typename V_TYPE,
             typename ALLOC = allocator<K_TYPE>,
             typename Compare = increaseComparator<K_TYPE>>
    class RBTree {
    protected:
        class RBNode {
        public:
            enum class color { BLACK, RED };

        private:
            couple<const K_TYPE, V_TYPE> data_;
            color color_;
            RBNode* parent_;
            RBNode* left_;
            RBNode* right_;
        public:
            explicit RBNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{},
                            color color = color::RED, RBNode* parent = nullptr, RBNode* left = nullptr, RBNode* right = nullptr);

            RBNode(const RBNode& other);

            RBNode& operator=(const RBNode& other);

            void swapData(RBNode& other) noexcept;

            void swapColor(RBNode& other) noexcept;

            const K_TYPE& getKey() const;

            const V_TYPE& getValue() const;

            V_TYPE& getValue();

            void setValue(const V_TYPE& value);

            color getColor() const;

            RBNode* getPParent() const;

            RBNode* getPLeft() const;

            RBNode* getPRight() const;

            void setColor(color new_color);

            void setPParent(RBNode* new_parent);

            void setPLeft(RBNode* new_left);

            void setPRight(RBNode* new_right);

            static void connect(RBNode* parent, RBNode* child, bool is_left);
        };

        using color = typename RBNode::color;
        static constexpr color BLACK = color::BLACK;
        static constexpr color RED = color::RED;
        using rebind_alloc_node = typename ALLOC::template rebind_alloc<RBNode>;

        RBNode* root_;
        u_integer size_;
        Compare compare_;
        mutable rebind_alloc_node rebind_alloc{};

        class Iterator
        {
        protected:
            mutable RBTree* tree_;
            mutable RBNode* cur_;

            explicit Iterator(RBTree* tree = nullptr, RBNode* cur = nullptr);

            Iterator(const Iterator& other);

            Iterator& operator=(const Iterator& other);

        public:
            [[nodiscard]] bool hasNext() const;

            [[nodiscard]] bool hasPrev() const;

            void next() const;

            void prev() const;

            void operator+=(integer steps) const;

            void operator-=(integer steps) const;

            couple<const K_TYPE, V_TYPE>& get();

            couple<const K_TYPE, V_TYPE> get() const;

            [[nodiscard]] bool isValid() const;
        };

        friend Iterator;

        RBNode* treeCopy() const;

        RBNode* getPrecursorNode(RBNode* cur) const;

        RBNode* getSuccessorNode(RBNode* cur) const;

        RBNode* createNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{},
                           color color = RED, RBNode* parent = nullptr, RBNode* left = nullptr, RBNode* right = nullptr) const;

        void destroyNode(RBNode* node) noexcept;

        bool highPriority(RBNode* cur, RBNode* other) const;

        bool highPriority(const K_TYPE& key, RBNode* other) const;

        RBNode* rotateLeft(RBNode* cur);

        RBNode* rotateRight(RBNode* cur);

        void adjustInsert(RBNode* cur);

        void adjustErase(RBNode* cur);

        void destroyTree() noexcept;

        explicit RBTree(Compare compare = Compare{});

        RBNode* find(const K_TYPE& key) const;

        bool modify(const K_TYPE& key, const V_TYPE& value);

        bool insert(const K_TYPE& key, const V_TYPE& value);

        bool erase(const K_TYPE& key);

        ~RBTree();
    };

}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::RBNode(const K_TYPE &key, const V_TYPE &value,
                                                                 const color color, RBNode *parent, RBNode *left, RBNode *right)
                                                                 : data_({key, value}), color_(color), parent_(parent), left_(left), right_(right) {}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::RBNode(const RBNode &other) : RBNode() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode&
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::operator=(const RBNode &other) {
    if (this == &other)
        return *this;

    this->data_ = other.data_;
    this->color_ = other.color_;
    this->parent_ = other.parent_;
    this->left_ = other.left_;
    this->right_ = other.right_;
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::swapData(RBNode &other) noexcept {
    std::swap(this->data_, other.data_);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::swapColor(RBNode &other) noexcept {
    std::swap(this->color_, other.color_);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
const K_TYPE &original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getKey() const {
    return this->data_.first();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
const V_TYPE &original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getValue() const {
    return this->data_.second();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
V_TYPE &original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getValue() {
    return this->data_.second();
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::setValue(const V_TYPE &value) {
    this->data_.template set<1>(value);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::color
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getColor() const {
    return this->color_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getPParent() const {
    return this->parent_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getPLeft() const {
    return this->left_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::getPRight() const {
    return this->right_;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::setColor(color new_color) {
    this->color_ = new_color;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::setPParent(RBNode* new_parent) {
    this->parent_ = new_parent;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::setPLeft(RBNode* new_left) {
    this->left_ = new_left;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::setPRight(RBNode* new_right) {
    this->right_ = new_right;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::connect(RBNode* parent,
                                                                       RBNode* child, bool is_left) {
    if (parent){
        is_left ? parent->left_ = child : parent->right_ = child;
    }
    if (child){
        child->parent_ = parent;
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::Iterator(RBTree* tree, RBNode* cur)
    : tree_(tree), cur_(cur) {}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::Iterator(const Iterator& other) : Iterator() {
    this->operator=(other);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator&
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::operator=(const Iterator& other)
{
    if (this == &other)
        return *this;

    this->tree_ = other.tree_;
    this->cur_ = other.cur_;
    return *this;
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::hasNext() const
{
    return this->cur_ && this->tree_->getSuccessorNode(this->cur_);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::hasPrev() const
{
    return this->cur_ && this->tree_->getPrecursorNode(this->cur_);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::next() const
{
    this->cur_ = this->tree_->getSuccessorNode(this->cur_);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::prev() const
{
    this->cur_ = this->tree_->getPrecursorNode(this->cur_);
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::operator+=(const integer steps) const
{
    for (integer i = 0; i < steps; ++i) {
        this->next();
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::operator-=(integer steps) const
{
    for (integer i = 0; i < steps; ++i) {
        this->prev();
    }
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::couple<const K_TYPE, V_TYPE>& original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::get()
{
    if (!this->isValid()) {
        throw outOfBoundError();
    }

    return this->cur_->getVal();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::couple<const K_TYPE, V_TYPE> original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::get() const
{
    if (!this->isValid()) {
        throw outOfBoundError();
    }

    return this->cur_->getVal();
}

template <typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::Iterator::isValid() const
{
    return this->cur_;
}


template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::treeCopy() const {
    if (!this->root_) {
        return nullptr;
    }

    RBNode* copied_root =
    this->createNode(this->root_->getKey(), this->root_->getValue(), this->root_->getColor());
    queue<RBNode*> src = {this->root_};
    queue<RBNode*> tar = {copied_root};
    while (!src.empty()){
        RBNode* src_cur = src.head();
        RBNode* tar_cur = tar.head();
        RBNode* src_child;
        RBNode* tar_child;
        if (src_cur->getPLeft()){
            src_child = src_cur->getPLeft();
            tar_child = this->createNode(src_child->getKey(), src_child->getValue(), src_child->getColor());
            RBNode::connect(tar_cur, tar_child, true);
            src.push(src_child);
            tar.push(tar_child);
        }
        if (src_cur->getPRight()){
            src_child = src_cur->getPRight();
            tar_child = this->createNode(src_child->getKey(), src_child->getValue(), src_child->getColor());
            RBNode::connect(tar_cur, tar_child, false);
            src.push(src_child);
            tar.push(tar_child);
        }
        src.pop();
        tar.pop();
    }
    return copied_root;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::getPrecursorNode(RBNode *cur) const {
    if (!cur)
        return nullptr;

    RBNode* pre;
    if (cur->getPLeft()){
        pre = cur->getPLeft();
        while (pre->getPRight()){
            pre = pre->getPRight();
        }
    } else{
        pre = cur;
        while (pre->getPParent() && pre->getPParent()->getPLeft() == pre){
            pre = pre->getPParent();
        }
    }
    return pre;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::getSuccessorNode(RBNode *cur) const {
    if (!cur)
        return nullptr;

    RBNode* nxt;
    if (cur->getPRight()){
        nxt = cur->getPRight();
        while (nxt->getPLeft()){
            nxt = nxt->getPLeft();
        }
    } else{
        nxt = cur;
        while (nxt->getPParent() && nxt->getPParent()->getPRight() == nxt){
            nxt = nxt->getPParent();
        }
    }
    return nxt;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::createNode(const K_TYPE &key, const V_TYPE &value,
                                                             color color, RBNode* parent,
                                                             RBNode* left, RBNode* right) const {
    auto node = this->rebind_alloc.allocate(1);
    this->rebind_alloc.construct(node, key, value, color, parent, left, right);
    return node;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::destroyNode(RBNode* node) noexcept {
    this->rebind_alloc.destroy(node);
    this->rebind_alloc.deallocate(node, 1);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::highPriority(RBNode *cur, RBNode *other) const {
    if (!cur){
        return false;
    }
    return this->highPriority(cur->getKey(), other);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::highPriority(const K_TYPE& key, RBNode* other) const {
    if (!other){
        return true;
    }
    return this->compare_(key, other->getKey());
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::rotateLeft(RBNode *cur) {
    RBNode* child_left = cur;
    RBNode* child_root = child_left->getPRight();
    RBNode* child_left_child = child_root->getPLeft();
    RBNode* child_right = child_root->getPRight();
    RBNode::connect(nullptr, child_root, true);
    RBNode::connect(child_root, child_left, true);
    RBNode::connect(child_left, child_left_child, false);
    RBNode::connect(child_root, child_right, false);
    return child_root;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::rotateRight(RBNode *cur) {
    RBNode* child_right = cur;
    RBNode* child_root = cur->getPLeft();
    RBNode* child_right_child = child_root->getPRight();
    RBNode* child_left = child_root->getPLeft();
    RBNode::connect(nullptr, child_root, true);
    RBNode::connect(child_root, child_left, true);
    RBNode::connect(child_right, child_right_child, true);
    RBNode::connect(child_root, child_right, false);
    return child_root;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::adjustInsert(RBNode *cur) {
    while (cur != this->root_ && cur->getPParent()->getColor() == RED) {
        RBNode* parent = cur->getPParent();
        RBNode* grand_parent = parent->getPParent();
        RBNode* uncle = grand_parent->getPLeft() == parent ? grand_parent->getPRight() : grand_parent->getPLeft();
        if (!uncle || uncle->getColor() == BLACK) {

            RBNode* rotated_root;
            if (grand_parent->getPLeft() == uncle) {

                if (parent->getPLeft() == cur) {
                    RBNode::connect(grand_parent, this->rotateRight(parent), false);
                }

                if (grand_parent == this->root_) {
                    rotated_root = this->rotateLeft(grand_parent);
                    this->root_ = rotated_root;
                }else {
                    RBNode* grand_grand = grand_parent->getPParent();
                    bool is_left = grand_grand->getPLeft() == grand_parent;
                    rotated_root = this->rotateLeft(grand_parent);
                    RBNode::connect(grand_grand, rotated_root, is_left);
                }
                rotated_root->setColor(BLACK);
                rotated_root->getPLeft()->setColor(RED);
            } else {

                if (parent->getPRight() == cur) {
                    RBNode::connect(grand_parent, this->rotateLeft(parent), true);
                }

                if (grand_parent == this->root_) {
                    rotated_root = this->rotateRight(grand_parent);
                    this->root_ = rotated_root;
                }else {
                    RBNode* grand_grand = grand_parent->getPParent();
                    bool is_left = grand_grand->getPLeft() == grand_parent;
                    rotated_root = this->rotateRight(grand_parent);
                    RBNode::connect(grand_grand, rotated_root, is_left);
                }
                rotated_root->setColor(BLACK);
                rotated_root->getPRight()->setColor(RED);
            }

            cur = rotated_root;
        } else {
            parent->setColor(BLACK);
            uncle->setColor(BLACK);
            grand_parent->setColor(RED);

            cur = grand_parent;
        }
    }

    this->root_->setColor(BLACK);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::adjustErase(RBNode *cur) {
    while (cur != this->root_ && cur->getColor() == BLACK) {
        RBNode* parent = cur->getPParent();
        RBNode* brother;
        RBNode* grand_parent;

        if (parent->getPLeft() == cur) {
            brother = parent->getPRight(); // R

            if (brother->getColor() == RED) {
                brother->swapColor(*parent);
                grand_parent = parent->getPParent();
                if (grand_parent) {
                    bool is_left = grand_parent->getPLeft() == parent;
                    RBNode::connect(grand_parent, this->rotateRight(parent), is_left);
                } else {
                    this->root_ = this->rotateRight(parent);
                    RBNode::connect(nullptr, parent, true);
                }
            } else {
                RBNode* nephew;
                if (brother->getPLeft() && brother->getPLeft()->getColor() == RED) { // RL
                    nephew = brother->getPLeft();
                    nephew->setColor(parent->getColor());
                    parent->setColor(BLACK);
                    RBNode::connect(parent, this->rotateRight(brother), true);
                    grand_parent = parent->getPParent();
                    if (grand_parent) {
                        bool is_left = grand_parent->getPLeft() == parent;
                        RBNode::connect(grand_parent, this->rotateLeft(parent), is_left);
                    } else {
                        this->root_ = this->rotateLeft(parent);
                        RBNode::connect(nullptr, this->root_, true);
                    }
                    break;
                }

                if (brother->getPRight() && brother->getPRight()->getColor() == RED) { // RR
                    nephew = brother->getPRight();
                    nephew->setColor(brother->getColor());
                    brother->setColor(parent->getColor());
                    parent->setColor(BLACK);
                    grand_parent = parent->getPParent();
                    if (grand_parent) {
                        bool is_left = grand_parent->getPLeft() == parent;
                        RBNode::connect(grand_parent, this->rotateLeft(parent), is_left);
                    } else {
                        this->root_ = this->rotateLeft(parent);
                        RBNode::connect(nullptr, this->root_, true);
                    }
                    break;
                }


            }
        }else {
            brother = parent->getPLeft(); // L

            if (brother->getColor() == RED) {
                brother->swapColor(*parent);
                grand_parent = parent->getPParent();
                if (grand_parent) {
                    bool is_left = grand_parent->getPLeft() == parent;
                    RBNode::connect(grand_parent, this->rotateLeft(parent), is_left);
                } else {
                    this->root_ = this->rotateLeft(parent);
                    RBNode::connect(nullptr, parent, true);
                }
            } else {
                RBNode* nephew;
                if (brother->getPRight() && brother->getPRight()->getColor() == RED) { // LR
                    nephew = brother->getPRight();
                    nephew->setColor(parent->getColor());
                    parent->setColor(BLACK);
                    RBNode::connect(parent, this->rotateLeft(brother), true);
                    grand_parent = parent->getPParent();
                    if (grand_parent) {
                        bool is_left = grand_parent->getPLeft() == parent;
                        RBNode::connect(grand_parent, this->rotateRight(parent), is_left);
                    } else {
                        this->root_ = this->rotateRight(parent);
                        RBNode::connect(nullptr, this->root_, true);
                    }
                    break;
                }

                if (brother->getPLeft() && brother->getPLeft()->getColor() == RED) { // LL
                    nephew = brother->getPLeft();
                    nephew->setColor(brother->getColor());
                    brother->setColor(parent->getColor());
                    parent->setColor(BLACK);
                    grand_parent = parent->getPParent();
                    if (grand_parent) {
                        bool is_left = grand_parent->getPLeft() == parent;
                        RBNode::connect(grand_parent, this->rotateRight(parent), is_left);
                    } else {
                        this->root_ = this->rotateRight(parent);
                        RBNode::connect(nullptr, this->root_, true);
                    }
                    break;
                }
            }
        }
    }

    cur->setColor(BLACK);
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::destroyTree() noexcept {
    if (!this->root_) {
        return;
    }

    queue<RBNode*> queue = {this->root_};
    while (!queue.empty()) {
        RBNode* node = queue.head();
        if (node->getPLeft()) {
            queue.push(node->getPLeft());
        }
        if (node->getPRight()) {
            queue.push(node->getPRight());
        }
        this->destroyNode(queue.pop());
    }
    this->root_ = nullptr;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBTree(Compare compare)
    : root_(nullptr), size_(0), compare_(std::move(compare)) {}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
typename original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode*
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::find(const K_TYPE &key) const {
    auto cur = this->root_;
    while (cur){
        if (cur->getKey() == key){
            return cur;
        }
        cur = this->highPriority(key, cur) ? cur->getPLeft() : cur->getPRight();
    }
    return cur;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::modify(const K_TYPE &key, const V_TYPE &value) {
    if (auto cur = this->find(key)){
        cur->setValue(value);
        return true;
    }
    return false;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::insert(const K_TYPE &key, const V_TYPE &value) {
    auto& cur = this->root_;
    RBNode* parent = nullptr;
    while (cur) {
        if (cur->getKey() == key) {
            return false;
        }
        parent = cur;
        cur = this->highPriority(key, cur) ? cur->getPLeft() : cur->getPRight();
    }
    RBNode* child;
    if (!cur) {
        cur = this->createNode(key, value, BLACK);
        child = cur;
    } else {
        child = this->createNode(key, value, RED);
        RBNode::connect(parent, child, this->highPriority(key, parent));
    }

    this->size_ += 1;
    this->adjustInsert(child);
    return true;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::erase(const K_TYPE &key) {
    if (!this->root_) {
        return false;
    }

    auto cur = this->find(key);
    if (!cur) {
        return false;
    }

    if (cur->getPLeft() && cur->getPRight()) {
        RBNode* replace = this->getPrecursorNode(cur) ?
                          this->getPrecursorNode(cur) : this->getSuccessorNode(cur);
        cur->swapData(*replace);
        cur = replace;
    }

    RBNode* parent = cur->getPParent();
    if (cur->getPLeft() && !cur->getPRight()) {
        if (!parent) {
            this->root_ = cur->getPLeft();
            RBNode::connect(nullptr, this->root_, true);
            this->root_->setColor(BLACK);
        } else {
            bool is_left = parent->getPLeft() == cur;
            RBNode::connect(parent, cur->getPLeft(), is_left);
            parent->setColor(BLACK);
        }
    } else if (!cur->getPLeft() && cur->getPRight()) {
        if (!parent) {
            this->root_ = cur->getPRight();
            RBNode::connect(nullptr, this->root_, true);
            this->root_->setColor(BLACK);
        } else {
            bool is_left = parent->getPLeft() == cur;
            RBNode::connect(parent, cur->getPRight(), is_left);
            parent->setColor(BLACK);
        }
    } else {
        if (!parent) {
            this->root_ = nullptr;
        } else if (cur->getColor() == BLACK) {
            this->adjustErase(cur);
        } else {
            RBNode::connect(nullptr, cur, true);
        }
    }

    if (cur->getPParent()) {
        RBNode::connect(cur->getPParent(), nullptr, cur->getPParent()->getPLeft() == cur);
    }
    this->destroyNode(cur);
    this->size_ -= 1;
    return true;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::~RBTree() {
    this->destroyTree();
}

#endif //RBTREE_H
