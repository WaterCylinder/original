#ifndef SETS_H
#define SETS_H

#include "allocator.h"
#include "couple.h"
#include "hash.h"
#include "hashTable.h"
#include "set.h"
#include "ownerPtr.h"
#include "comparator.h"
#include "RBTree.h"


/**
 * @file sets.h
 * @brief Implementation of hashSet container
 * @details Provides a hash-based set implementation with:
 * - Average O(1) time complexity for basic operations
 * - Separate chaining collision resolution
 * - Customizable hash function and allocator
 * - Full iterator support
 *
 * Key Features:
 * - Inherits from set interface for polymorphic use
 * - Uses hashTable as underlying storage (with bool values)
 * - Provides efficient membership testing
 * - Supports all standard set operations
 * - Exception-safe implementation
 */

namespace original {
    /**
     * @class hashSet
     * @tparam TYPE Element type (must be hashable)
     * @tparam HASH Hash function type (default: hash<TYPE>)
     * @tparam ALLOC Allocator type (default: allocator<couple<const TYPE, const bool>>)
     * @brief Hash table based implementation of the set interface
     * @details This class provides a concrete implementation of the set interface
     * using a hash table with separate chaining. It combines the functionality of:
     * - set (interface)
     * - hashTable (storage with bool values)
     * - iterable (iteration support)
     *
     * Performance Characteristics:
     * - Insertion: Average O(1), Worst O(n)
     * - Lookup: Average O(1), Worst O(n)
     * - Deletion: Average O(1), Worst O(n)
     *
     * The implementation guarantees:
     * - Unique elements (no duplicates)
     * - Type safety
     * - Exception safety (basic guarantee)
     * - Iterator validity unless modified
     */
    template <typename TYPE,
              typename HASH = hash<TYPE>,
              typename ALLOC = allocator<couple<const TYPE, const bool>>>
    class hashSet final : public hashTable<TYPE, const bool, ALLOC, HASH>,
                          public set<TYPE, ALLOC>,
                          public iterable<const TYPE>,
                          public printable{

        /**
         * @typedef hashNode
         * @brief Internal node type used for hash table storage
         */
        using hashNode = typename hashTable<TYPE, const bool, ALLOC, HASH>::hashNode;

        /**
         * @typedef rebind_alloc_pointer
         * @brief Rebound allocator type for pointer storage
         */
        using rebind_alloc_pointer = typename hashTable<TYPE, const bool, ALLOC, HASH>::rebind_alloc_pointer;

    public:
        /**
         * @class Iterator
         * @brief Forward iterator for hashSet
         * @details Provides iteration over hashSet elements while maintaining:
         * - Consistent traversal order
         * - Safe invalidation detection
         * - Const-correct access
         *
         * Iterator Characteristics:
         * - Forward iteration only (throws on reverse operations)
         * - Invalidates on rehash
         * - Lightweight copy semantics
         */
        class Iterator final : public hashTable<TYPE, const bool, ALLOC, HASH>::Iterator,
                             public baseIterator<const TYPE> {

            /**
             * @brief Constructs iterator pointing to specific bucket/node
             * @param buckets Pointer to buckets vector
             * @param bucket Current bucket index
             * @param node Current node pointer
             * @note Internal constructor, not meant for direct use
             */
            explicit Iterator(vector<hashNode*, rebind_alloc_pointer>* buckets = nullptr,
                              u_integer bucket = 0, hashNode* node = nullptr);

            /**
             * @brief Compares iterator pointers for equality
             * @param other Iterator to compare with
             * @return true if iterators point to same element
             * @internal
             */
            bool equalPtr(const iterator<const TYPE>* other) const override;

        public:
            friend class hashSet;

            /**
             * @brief Copy constructor
             * @param other Iterator to copy
             */
            Iterator(const Iterator& other);

            /**
             * @brief Copy assignment operator
             * @param other Iterator to copy
             * @return Reference to this iterator
             */
            Iterator& operator=(const Iterator& other);

            /**
             * @brief Creates a copy of this iterator
             * @return New iterator instance
             */
            Iterator* clone() const override;

            /**
             * @brief Gets iterator class name
             * @return "hashSet::Iterator"
             */
            [[nodiscard]] std::string className() const override;

            /**
             * @brief Advances iterator by steps
             * @param steps Number of positions to advance
             * @throw unSupportedMethodError if steps is negative
             */
            void operator+=(integer steps) const override;

            /**
             * @brief Not supported (throws unSupportedMethodError)
             */
            void operator-=(integer steps) const override;

            /**
             * @brief Not supported (throws unSupportedMethodError)
             */
            integer operator-(const iterator<const TYPE>& other) const override;

            /**
             * @brief Checks if more elements exist
             * @return true if more elements available
             */
            [[nodiscard]] bool hasNext() const override;

            /**
             * @brief Not supported (throws unSupportedMethodError)
             */
            [[nodiscard]] bool hasPrev() const override;

            /**
             * @brief Checks if other is previous to this
             * @param other Iterator to check
             * @return true if other is previous
             */
            bool atPrev(const iterator<const TYPE>* other) const override;

            /**
             * @brief Checks if other is next to this
             * @param other Iterator to check
             * @return true if other is next
             */
            bool atNext(const iterator<const TYPE>* other) const override;

            /**
             * @brief Moves to next element
             */
            void next() const override;

            /**
             * @brief Not supported (throws unSupportedMethodError)
             */
            void prev() const override;

            /**
             * @brief Not supported (throws unSupportedMethodError)
             */
            Iterator* getPrev() const override;

            /**
             * @brief Gets current element (non-const)
             * @return Reference to current element
             */
            const TYPE& get() override;

            /**
             * @brief Gets current element (const)
             * @return Copy of current element
             */
            const TYPE get() const override;

            /**
             * @brief Not supported (throws unSupportedMethodError)
             */
            void set(const TYPE& data) override;

            /**
             * @brief Checks if iterator is valid
             * @return true if iterator points to valid element
             */
            [[nodiscard]] bool isValid() const override;

            ~Iterator() override = default;
        };

        /**
         * @brief Constructs empty hashSet
         * @param hash Hash function to use
         * @param alloc Allocator to use
         */
        explicit hashSet(HASH hash = HASH{}, ALLOC alloc = ALLOC{});

        /**
         * @brief Copy constructor
         * @param other hashSet to copy
         * @details Performs deep copy of all elements and buckets
         * @note Allocator is copied if propagate_on_container_copy_assignment is true
         */
        hashSet(const hashSet& other);

        /**
         * @brief Copy assignment operator
         * @param other hashSet to copy
         * @return Reference to this hashSet
         * @details Performs deep copy of all elements and buckets
         * @note Allocator is copied if propagate_on_container_copy_assignment is true
         */
        hashSet& operator=(const hashSet& other);

        /**
         * @brief Move constructor
         * @param other hashSet to move from
         * @details Transfers ownership of resources from other
         * @note Leaves other in valid but unspecified state
         */
        hashSet(hashSet&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other hashSet to move from
         * @return Reference to this hashSet
         * @details Transfers ownership of resources from other
         * @note Leaves other in valid but unspecified state
         * @note Allocator is moved if propagate_on_container_move_assignment is true
         */
        hashSet& operator=(hashSet&& other) noexcept;

        /**
         * @brief Gets number of elements
         * @return Current size
         */
        [[nodiscard]] u_integer size() const override;

        /**
         * @brief Checks if element exists
         * @param e Element to check
         * @return true if element exists
         */
        bool contains(const TYPE &e) const override;

        /**
         * @brief Adds new element
         * @param e Element to add
         * @return true if added, false if element existed
         */
        bool add(const TYPE &e) override;

        /**
         * @brief Removes element
         * @param e Element to remove
         * @return true if removed, false if element didn't exist
         */
        bool remove(const TYPE &e) override;

        /**
         * @brief Gets begin iterator
         * @return New iterator at first element
         */
        Iterator* begins() const override;

        /**
         * @brief Gets end iterator
         * @return New iterator at position past last element
         */
        Iterator* ends() const override;

        /**
         * @brief Gets class name
         * @return "hashSet"
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Converts to string representation
         * @param enter Add newline if true
         * @return String representation
         */
        [[nodiscard]] std::string toString(bool enter) const override;

        ~hashSet() override;
    };

    // todo
    template <typename TYPE,
              typename Compare = increaseComparator<TYPE>,
              typename ALLOC = allocator<couple<const TYPE, const bool>>>
    class treeSet final : public RBTree<TYPE, const bool, ALLOC, Compare>,
                          public set<TYPE, ALLOC>,
                          public iterable<const TYPE>,
                          public printable {
    public:
        explicit treeSet(Compare comp = Compare{}, ALLOC alloc = ALLOC{});

        treeSet(const treeSet& other);

        treeSet& operator=(const treeSet& other);

        treeSet(treeSet&& other) noexcept;

        treeSet& operator=(treeSet&& other) noexcept;

        [[nodiscard]] u_integer size() const override;

        bool contains(const TYPE &e) const override;

        bool add(const TYPE &e) override;

        bool remove(const TYPE &e) override;

//        Iterator* begins() const override;

//        Iterator* ends() const override;

        [[nodiscard]] std::string className() const override;

        [[nodiscard]] std::string toString(bool enter) const override;

        ~treeSet() override;
    };
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::Iterator::Iterator(vector<hashNode*, rebind_alloc_pointer> *buckets,
u_integer bucket, hashNode *node) : hashTable<TYPE, const bool, ALLOC, HASH>::Iterator(
    const_cast<vector<hashNode*, rebind_alloc_pointer>*>(buckets), bucket, node) {}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::Iterator::equalPtr(const iterator<const TYPE> *other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    return other_it &&
           this->p_buckets == other_it->p_buckets &&
           this->cur_bucket == other_it->cur_bucket &&
           this->p_node == other_it->p_node;
}

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

    hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::operator=(other);
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
void original::hashSet<TYPE, HASH, ALLOC>::Iterator::operator+=(integer steps) const {
    hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::operator+=(steps);
}

template<typename TYPE, typename HASH, typename ALLOC>
void original::hashSet<TYPE, HASH, ALLOC>::Iterator::operator-=(integer) const {
    throw unSupportedMethodError();
}

template<typename TYPE, typename HASH, typename ALLOC>
original::integer
original::hashSet<TYPE, HASH, ALLOC>::Iterator::operator-(const iterator<const TYPE>&) const {
    throw unSupportedMethodError();
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::Iterator::hasNext() const {
    return hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::hasNext();
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::Iterator::hasPrev() const {
    throw unSupportedMethodError();
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::Iterator::atPrev(const iterator<const TYPE> *other) const {
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

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::Iterator::atNext(const iterator<const TYPE> *other) const {
    return other->atPrev(*this);
}

template<typename TYPE, typename HASH, typename ALLOC>
void original::hashSet<TYPE, HASH, ALLOC>::Iterator::next() const {
    hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::next();
}

template<typename TYPE, typename HASH, typename ALLOC>
void original::hashSet<TYPE, HASH, ALLOC>::Iterator::prev() const {
    throw unSupportedMethodError();
}

template<typename TYPE, typename HASH, typename ALLOC>
typename original::hashSet<TYPE, HASH, ALLOC>::Iterator*
original::hashSet<TYPE, HASH, ALLOC>::Iterator::getPrev() const {
    throw unSupportedMethodError();
}

template<typename TYPE, typename HASH, typename ALLOC>
const TYPE& original::hashSet<TYPE, HASH, ALLOC>::Iterator::get() {
    return hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::get().template get<0>();
}

template<typename TYPE, typename HASH, typename ALLOC>
const TYPE original::hashSet<TYPE, HASH, ALLOC>::Iterator::get() const {
    return hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::get().template get<0>();
}

template<typename TYPE, typename HASH, typename ALLOC>
void original::hashSet<TYPE, HASH, ALLOC>::Iterator::set(const TYPE&) {
    throw unSupportedMethodError();
}

template<typename TYPE, typename HASH, typename ALLOC>
bool original::hashSet<TYPE, HASH, ALLOC>::Iterator::isValid() const {
    return hashTable<TYPE, const bool, ALLOC, HASH>::Iterator::isValid();
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::hashSet(HASH hash, ALLOC alloc)
    : hashTable<TYPE, const bool, ALLOC, HASH>(std::move(hash)),
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

    this->buckets = this->bucketsCopy(other.buckets);
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
    other.size_ = 0;
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
std::string original::hashSet<TYPE, HASH, ALLOC>::className() const {
    return "hashSet";
}

template<typename TYPE, typename HASH, typename ALLOC>
std::string original::hashSet<TYPE, HASH, ALLOC>::toString(bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(";
    bool first = true;
    for (auto it = this->begin(); it != this->end(); it.next()){
        if (!first){
            ss << ", ";
        }
        ss << printable::formatString(it.get());
        first = false;
    }
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename TYPE, typename HASH, typename ALLOC>
original::hashSet<TYPE, HASH, ALLOC>::~hashSet() = default;

template<typename TYPE, typename Compare, typename ALLOC>
original::treeSet<TYPE, Compare, ALLOC>::treeSet(Compare comp, ALLOC alloc)
    : RBTree<TYPE, const bool, ALLOC, Compare>(std::move(comp)),
      set<TYPE, ALLOC>(std::move(alloc)) {}

template<typename TYPE, typename Compare, typename ALLOC>
original::treeSet<TYPE, Compare, ALLOC>::treeSet(const treeSet& other) : treeSet() {
    this->operator=(other);
}

template<typename TYPE, typename Compare, typename ALLOC>
original::treeSet<TYPE, Compare, ALLOC>&
original::treeSet<TYPE, Compare, ALLOC>::operator=(const treeSet& other) {
    if (this == &other){
        return *this;
    }

    this->root_ = other.treeCopy();
    this->size_ = other.size_;
    if constexpr(ALLOC::propagate_on_container_copy_assignment::value) {
        this->allocator = other.allocator;
        this->rebind_alloc = other.rebind_alloc;
    }
    return *this;
}

template<typename TYPE, typename Compare, typename ALLOC>
original::treeSet<TYPE, Compare, ALLOC>::treeSet(treeSet&& other) noexcept : treeSet() {
    this->operator=(std::move(other));
}

template<typename TYPE, typename Compare, typename ALLOC>
original::treeSet<TYPE, Compare, ALLOC>&
original::treeSet<TYPE, Compare, ALLOC>::operator=(treeSet&& other) noexcept {
    if (this == &other){
        return *this;
    }

    this->root_ = other.root_;
    other.root_ = nullptr;
    this->size_ = other.size_;
    other.size_ = 0;
    if constexpr(ALLOC::propagate_on_container_move_assignment::value) {
        this->allocator = std::move(other.allocator);
        this->rebind_alloc = std::move(other.rebind_alloc);
    }
    return *this;
}

template<typename TYPE, typename Compare, typename ALLOC>
original::u_integer original::treeSet<TYPE, Compare, ALLOC>::size() const {
    return this->size_;
}

template<typename TYPE, typename Compare, typename ALLOC>
bool original::treeSet<TYPE, Compare, ALLOC>::contains(const TYPE &e) const {
    return this->find(e);
}

template<typename TYPE, typename Compare, typename ALLOC>
bool original::treeSet<TYPE, Compare, ALLOC>::add(const TYPE &e) {
    return this->insert(e, true);
}

template<typename TYPE, typename Compare, typename ALLOC>
bool original::treeSet<TYPE, Compare, ALLOC>::remove(const TYPE &e) {
    return this->erase(e);
}

template<typename TYPE, typename Compare, typename ALLOC>
std::string original::treeSet<TYPE, Compare, ALLOC>::className() const {
    return "treeSet";
}

template<typename TYPE, typename Compare, typename ALLOC>
std::string original::treeSet<TYPE, Compare, ALLOC>::toString(bool enter) const {
    return std::string();
}

template<typename TYPE, typename Compare, typename ALLOC>
original::treeSet<TYPE, Compare, ALLOC>::~treeSet() = default;

#endif //SETS_H
