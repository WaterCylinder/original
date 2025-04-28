#ifndef RBTREE_H
#define RBTREE_H

#include "allocator.h"
#include "comparator.h"
#include "couple.h"
#include "queue.h"


namespace original {
    // todo
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


        RBNode* createNode(const K_TYPE& key = K_TYPE{}, const V_TYPE& value = V_TYPE{},
                           color color = RED, RBNode* parent = nullptr, RBNode* left = nullptr, RBNode* right = nullptr) const;

        void destroyNode(RBNode* node) noexcept;

        bool highPriority(RBNode* cur, RBNode* other) const;

        bool highPriority(const K_TYPE& key, RBNode* other) const;

        RBNode* rotateLeft(RBNode* cur);

        RBNode* rotateRight(RBNode* cur);

        void adjust(RBNode* cur);

        void destroyTree() noexcept;

        explicit RBTree(Compare compare = Compare{});

        RBNode* find(const K_TYPE& key) const;

        bool insert(const K_TYPE& key, const V_TYPE& value);

        bool erase(const K_TYPE& key);

        ~RBTree();
    };

}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::RBNode::RBNode(const K_TYPE &key, const V_TYPE &value,
                                                                 color color, RBNode *parent, RBNode *left, RBNode *right)
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
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::adjust(RBNode *cur) {
    if (cur == this->root_) {
        cur->setColor(BLACK);
        return;
    }
    RBNode* parent = cur->getParent();
    if (parent->getColor() == BLACK) {
        return;
    }

    RBNode* grand_parent = parent->getParent();
    // todo
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
void original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::destroyTree() noexcept {
    if (this->root_) {
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
    this->adjust(child);
    return true;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
bool original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::erase(const K_TYPE &key) {
    return false;
}

template<typename K_TYPE, typename V_TYPE, typename ALLOC, typename Compare>
original::RBTree<K_TYPE, V_TYPE, ALLOC, Compare>::~RBTree() {
    this->destroyTree();
}

#endif //RBTREE_H
