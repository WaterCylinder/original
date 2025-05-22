#ifndef MAPS_H
#define MAPS_H

#include "allocator.h"
#include "couple.h"
#include "hash.h"
#include "hashTable.h"
#include "map.h"
#include "ownerPtr.h"
#include "comparator.h"
#include "RBTree.h"
#include "skipList.h"


/**
 * @file maps.h
 * @brief Implementation of hashMap container
 * @details Provides a hash-based map implementation with:
 * - Average O(1) time complexity for basic operations
 * - Separate chaining collision resolution
 * - Customizable hash function and allocator
 * - Full iterator support
 *
 * Key Features:
 * - Inherits from map interface for polymorphic use
 * - Uses hashTable as underlying storage
 * - Provides both const and non-const element access
 * - Supports all standard map operations
 * - Exception-safe implementation
 */

namespace original {

    /**
     * @class hashMap
     * @tparam K_TYPE Key type (must be hashable)
     * @tparam V_TYPE Value type
     * @tparam HASH Hash function type (default: hash<K_TYPE>)
     * @tparam ALLOC Allocator type (default: allocator)
     * @brief Hash table based implementation of the map interface
     * @details This class provides a concrete implementation of the map interface
     * using a hash table with separate chaining. It combines the functionality of:
     * - map (interface)
     * - hashTable (storage)
     * - iterable (iteration support)
     *
     * Performance Characteristics:
     * - Insertion: Average O(1), Worst O(n)
     * - Lookup: Average O(1), Worst O(n)
     * - Deletion: Average O(1), Worst O(n)
     *
     * The implementation guarantees:
     * - Unique keys (no duplicates)
     * - Type safety
     * - Exception safety (basic guarantee)
     * - Iterator validity unless modified
     */
    template <typename K_TYPE,
              typename V_TYPE,
              typename HASH = hash<K_TYPE>,
              typename ALLOC = allocator<couple<const K_TYPE, V_TYPE>>>
    class hashMap final
                : public hashTable<K_TYPE, V_TYPE, ALLOC, HASH>,
                  public map<K_TYPE, V_TYPE, ALLOC>,
                  public iterable<couple<const K_TYPE, V_TYPE>>,
                  public printable{

        /**
         * @typedef hashNode
         * @brief Internal node type used for hash table storage
         */
        using hashNode = typename hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::hashNode;

        /**
         * @typedef rebind_alloc_pointer
         * @brief Rebound allocator type for pointer storage
         */
        using rebind_alloc_pointer = typename hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::rebind_alloc_pointer;
    public:

            /**
             * @class Iterator
             * @brief Bidirectional iterator for hashMap
             * @details Provides iteration over hashMap elements while maintaining:
             * - Consistent traversal order
             * - Safe invalidation detection
             * - Const-correct access
             *
             * Iterator Characteristics:
             * - Forward iteration only (throws on reverse operations)
             * - Invalidates on rehash
             * - Lightweight copy semantics
             */
            class Iterator final : public hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator,
                                   public baseIterator<couple<const K_TYPE, V_TYPE>> {

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
                bool equalPtr(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;
            public:
                friend class hashMap;

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
                 * @return "hashMap::Iterator"
                 */
                [[nodiscard]] std::string className() const override;

                /**
                 * @brief Advances iterator by steps
                 * @param steps Number of positions to advance
                 * @throw unSupportedMethodError if steps is negative
                 */
                void operator+=(integer steps) const override;

                /**
                 * @brief Not supported
                 * @throw unSupportedMethodError
                 */
                void operator-=(integer steps) const override;

                /**
                 * @brief Not supported
                 * @throw unSupportedMethodError
                 */
                integer operator-(const iterator<couple<const K_TYPE, V_TYPE>> &other) const override;

                /**
                 * @brief Checks if more elements exist
                 * @return true if more elements available
                 */
                [[nodiscard]] bool hasNext() const override;

                /**
                 * @brief Not supported
                 * @throw unSupportedMethodError
                 */
                [[nodiscard]] bool hasPrev() const override;

                /**
                 * @brief Checks if other is previous to this
                 * @param other Iterator to check
                 * @return true if other is previous
                 */
                bool atPrev(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;

                /**
                 * @brief Checks if other is next to this
                 * @param other Iterator to check
                 * @return true if other is next
                 */
                bool atNext(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;

                /**
                 * @brief Moves to next element
                 */
                void next() const override;

                /**
                 * @brief Not supported
                 * @throw unSupportedMethodError
                 */
                void prev() const override;

                /**
                 * @brief Not supported
                 * @throw unSupportedMethodError
                 */
                Iterator* getPrev() const override;

                /**
                 * @brief Gets current element (non-const)
                 * @return Reference to current key-value pair
                 */
                couple<const K_TYPE, V_TYPE> &get() override;

                /**
                 * @brief Gets current element (const)
                 * @return Copy of current key-value pair
                 */
                couple<const K_TYPE, V_TYPE> get() const override;

                /**
                 * @brief Not supported
                 * @throw unSupportedMethodError
                 */
                void set(const couple<const K_TYPE, V_TYPE> &data) override;

                /**
                 * @brief Checks if iterator is valid
                 * @return true if iterator points to valid element
                 */
                [[nodiscard]] bool isValid() const override;

                ~Iterator() override = default;
            };

            /**
             * @brief Constructs empty hashMap
             * @param hash Hash function to use
             * @param alloc Allocator to use
             */
            explicit hashMap(HASH hash = HASH{}, ALLOC alloc = ALLOC{});

            /**
             * @brief Copy constructor
             * @param other hashMap to copy
             * @details Performs deep copy of all elements and buckets
             * @note Allocator is copied if propagate_on_container_copy_assignment is true
             */
            hashMap(const hashMap& other);

            /**
             * @brief Copy assignment operator
             * @param other hashMap to copy
             * @return Reference to this hashMap
             * @details Performs deep copy of all elements and buckets
             * @note Allocator is copied if propagate_on_container_copy_assignment is true
             */
            hashMap& operator=(const hashMap& other);

            /**
             * @brief Move constructor
             * @param other hashMap to move from
             * @details Transfers ownership of resources from other
             * @note Leaves other in valid but unspecified state
             */
            hashMap(hashMap&& other) noexcept;

            /**
             * @brief Move assignment operator
             * @param other hashMap to move from
             * @return Reference to this hashMap
             * @details Transfers ownership of resources from other
             * @note Leaves other in valid but unspecified state
             * @note Allocator is moved if propagate_on_container_move_assignment is true
             */
            hashMap& operator=(hashMap&& other) noexcept;

            /**
             * @brief Gets number of elements
             * @return Current size
             */
            [[nodiscard]] u_integer size() const override;

            /**
             * @brief Checks if key-value pair exists
             * @param e Pair to check
             * @return true if both key exists and value matches
             */
            bool contains(const couple<const K_TYPE, V_TYPE> &e) const override;

            /**
             * @brief Adds new key-value pair
             * @param k Key to add
             * @param v Value to associate
             * @return true if added, false if key existed
             */
            bool add(const K_TYPE &k, const V_TYPE &v) override;

            /**
             * @brief Removes key-value pair
             * @param k Key to remove
             * @return true if removed, false if key didn't exist
             */
            bool remove(const K_TYPE &k) override;

            /**
             * @brief Checks if key exists
             * @param k Key to check
             * @return true if key exists
             */
            [[nodiscard]] bool containsKey(const K_TYPE &k) const override;

            /**
             * @brief Gets value for key
             * @param k Key to lookup
             * @return Associated value
             * @throw noElementError if key doesn't exist
             */
            V_TYPE get(const K_TYPE &k) const override;

            /**
             * @brief Updates value for existing key
             * @param key Key to update
             * @param value New value
             * @return true if updated, false if key didn't exist
             */
            bool update(const K_TYPE &key, const V_TYPE &value) override;

            /**
             * @brief Const element access
             * @param k Key to access
             * @return const reference to value
             * @throw noElementError if key doesn't exist
             */
            const V_TYPE & operator[](const K_TYPE &k) const override;

            /**
             * @brief Non-const element access
             * @param k Key to access
             * @return reference to value
             * @note Inserts default-constructed value if key doesn't exist
             */
            V_TYPE & operator[](const K_TYPE &k) override;

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
             * @return "hashMap"
             */
            [[nodiscard]] std::string className() const override;

            /**
             * @brief Converts to string representation
             * @param enter Add newline if true
             * @return String representation
             */
            [[nodiscard]] std::string toString(bool enter) const override;

            ~hashMap() override;
    };

    /**
     * @class treeMap
     * @tparam K_TYPE Key type (must be comparable)
     * @tparam V_TYPE Value type
     * @tparam Compare Comparison function type (default: increaseComparator<K_TYPE>)
     * @tparam ALLOC Allocator type (default: allocator)
     * @brief Red-Black Tree based implementation of the map interface
     * @details This class provides a concrete implementation of the map interface
     * using a Red-Black Tree. It combines the functionality of:
     * - map (interface)
     * - RBTree (storage)
     * - iterable (iteration support)
     *
     * Performance Characteristics:
     * - Insertion: O(log n)
     * - Lookup: O(log n)
     * - Deletion: O(log n)
     * - Traversal: O(n)
     *
     * The implementation guarantees:
     * - Elements sorted by key according to comparator
     * - Unique keys (no duplicates)
     * - Type safety
     * - Exception safety (basic guarantee)
     * - Iterator validity unless modified
     */
    template <typename K_TYPE,
              typename V_TYPE,
              typename Compare = increaseComparator<K_TYPE>,
              typename ALLOC = allocator<couple<const K_TYPE, V_TYPE>>>
    class treeMap final : public RBTree<K_TYPE, V_TYPE, ALLOC, Compare>,
                          public map<K_TYPE, V_TYPE, ALLOC>,
                          public iterable<couple<const K_TYPE, V_TYPE>>,
                          public printable {

        /**
         * @typedef RBTreeType
         * @brief Alias for the underlying red-black tree implementation.
         */
        using RBTreeType = RBTree<K_TYPE, V_TYPE, ALLOC, Compare>;

        /**
         * @typedef RBNode
         * @brief Internal node type used for Red-Black Tree storage
         */
        using RBNode = typename RBTreeType::RBNode;
    public:

        /**
         * @class Iterator
         * @brief Bidirectional iterator for treeMap
         * @details Provides iteration over treeMap elements while maintaining:
         * - Sorted traversal order (according to comparator)
         * - Safe invalidation detection
         * - Const-correct access
         *
         * Iterator Characteristics:
         * - Both forward and backward iteration
         * - Invalidates on tree modification
         * - Lightweight copy semantics
         */
        class Iterator final : public RBTreeType::Iterator,
                               public baseIterator<couple<const K_TYPE, V_TYPE>> {
       /**
         * @brief Constructs iterator pointing to specific tree node
         * @param tree Pointer to owning tree
         * @param cur Current node pointer
         * @note Internal constructor, not meant for direct use
         */
        explicit Iterator(RBTreeType* tree, RBNode* cur);

        /**
         * @brief Compares iterator pointers for equality
         * @param other Iterator to compare with
         * @return true if iterators point to same element
         * @internal
         */
        bool equalPtr(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;
    public:
        friend class treeMap;

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
         * @return "treeMap::Iterator"
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Advances iterator by steps
         * @param steps Number of positions to advance
         */
        void operator+=(integer steps) const override;

        /**
         * @brief Rewinds iterator by steps
         * @param steps Number of positions to rewind
         */
        void operator-=(integer steps) const override;

        /**
         * @brief Not supported (throws unSupportedMethodError)
         */
        integer operator-(const iterator<couple<const K_TYPE, V_TYPE>> &other) const override;

        /**
         * @brief Checks if more elements exist in forward direction
         * @return true if more elements available
         */
        [[nodiscard]] bool hasNext() const override;

        /**
         * @brief Checks if more elements exist in backward direction
         * @return true if more elements available
         */
        [[nodiscard]] bool hasPrev() const override;

        /**
         * @brief Checks if other is previous to this
         * @param other Iterator to check
         * @return true if other is previous
         */
        bool atPrev(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;

        /**
         * @brief Checks if other is next to this
         * @param other Iterator to check
         * @return true if other is next
         */
        bool atNext(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;

        /**
         * @brief Moves to next element
         */
        void next() const override;

        /**
         * @brief Moves to previous element
         */
        void prev() const override;

        /**
         * @brief Gets previous iterator
         * @return New iterator at previous position
         */
        Iterator* getPrev() const override;

        /**
         * @brief Gets current element (non-const)
         * @return Reference to current key-value pair
         */
        couple<const K_TYPE, V_TYPE>& get() override;

        /**
         * @brief Gets current element (const)
         * @return Copy of current key-value pair
         */
        couple<const K_TYPE, V_TYPE> get() const override;

        /**
         * @brief Not supported
         * @throw unSupportedMethodError
         */
        void set(const couple<const K_TYPE, V_TYPE> &data) override;

        /**
         * @brief Checks if iterator is valid
         * @return true if iterator points to valid element
         */
        [[nodiscard]] bool isValid() const override;

        ~Iterator() override = default;
    };

        friend class Iterator;

       /**
         * @brief Constructs empty treeMap
         * @param comp Comparison function to use
         * @param alloc Allocator to use
         */
        explicit treeMap(Compare comp = Compare{}, ALLOC alloc = ALLOC{});

        /**
         * @brief Copy constructor
         * @param other treeMap to copy
         * @details Performs deep copy of all elements and tree structure
         * @note Allocator is copied if propagate_on_container_copy_assignment is true
         */
        treeMap(const treeMap& other);

        /**
         * @brief Copy assignment operator
         * @param other treeMap to copy
         * @return Reference to this treeMap
         * @details Performs deep copy of all elements and tree structure
         * @note Allocator is copied if propagate_on_container_copy_assignment is true
         */
        treeMap& operator=(const treeMap& other);

        /**
         * @brief Move constructor
         * @param other treeMap to move from
         * @details Transfers ownership of resources from other
         * @note Leaves other in valid but unspecified state
         */
        treeMap(treeMap&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other treeMap to move from
         * @return Reference to this treeMap
         * @details Transfers ownership of resources from other
         * @note Leaves other in valid but unspecified state
         * @note Allocator is moved if propagate_on_container_move_assignment is true
         */
        treeMap& operator=(treeMap&& other) noexcept;

        /**
         * @brief Gets number of elements
         * @return Current size
         */
        [[nodiscard]] u_integer size() const override;

        /**
         * @brief Checks if key-value pair exists
         * @param e Pair to check
         * @return true if both key exists and value matches
         */
        bool contains(const couple<const K_TYPE, V_TYPE> &e) const override;

        /**
         * @brief Adds new key-value pair
         * @param k Key to add
         * @param v Value to associate
         * @return true if added, false if key existed
         */
        bool add(const K_TYPE &k, const V_TYPE &v) override;

        /**
         * @brief Removes key-value pair
         * @param k Key to remove
         * @return true if removed, false if key didn't exist
         */
        bool remove(const K_TYPE &k) override;

        /**
         * @brief Checks if key exists
         * @param k Key to check
         * @return true if key exists
         */
        [[nodiscard]] bool containsKey(const K_TYPE &k) const override;

        /**
         * @brief Gets value for key
         * @param k Key to lookup
         * @return Associated value
         * @throw noElementError if key doesn't exist
         */
        V_TYPE get(const K_TYPE &k) const override;

        /**
         * @brief Updates value for existing key
         * @param key Key to update
         * @param value New value
         * @return true if updated, false if key didn't exist
         */
        bool update(const K_TYPE &key, const V_TYPE &value) override;

        /**
         * @brief Const element access
         * @param k Key to access
         * @return const reference to value
         * @throw noElementError if key doesn't exist
         */
        const V_TYPE & operator[](const K_TYPE &k) const override;

        /**
         * @brief Non-const element access
         * @param k Key to access
         * @return reference to value
         * @note Inserts default-constructed value if key doesn't exist
         */
        V_TYPE & operator[](const K_TYPE &k) override;

        /**
         * @brief Gets begin iterator
         * @return New iterator at first element (minimum key)
         */
        Iterator* begins() const override;

        /**
         * @brief Gets end iterator
         * @return New iterator at last element (maximum key)
         */
        Iterator* ends() const override;

        /**
         * @brief Gets class name
         * @return "treeMap"
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Converts to string representation
         * @param enter Add newline if true
         * @return String representation of key-value pairs
         */
        [[nodiscard]] std::string toString(bool enter) const override;

        /**
         * @brief Destructor
         * @details Cleans up all tree nodes and allocated memory
         */
        ~treeMap() override;
    };

    // todo
    template <typename K_TYPE,
              typename V_TYPE,
              typename Compare = increaseComparator<K_TYPE>,
              typename ALLOC = allocator<couple<const K_TYPE, V_TYPE>>>
    class JMap final : public skipList<const K_TYPE, V_TYPE, ALLOC, Compare>,
                       public map<K_TYPE, V_TYPE, ALLOC>,
                       public iterable<couple<const K_TYPE, V_TYPE>>,
                       public printable {

        using skipListType = skipList<const K_TYPE, V_TYPE, ALLOC, Compare>;

        using skipListNode = skipListType::skipListNode;

    public:

        class Iterator : public skipListType::Iterator,
                         public baseIterator<couple<const K_TYPE, V_TYPE>> {

            bool equalPtr(const iterator<couple<const K_TYPE, V_TYPE>>* other) const override;
        public:
            friend class JMap;

            explicit Iterator(skipListNode* cur);

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

            couple<const K_TYPE, V_TYPE>& get() override;

            couple<const K_TYPE, V_TYPE> get() const override;

            void set(const couple<const K_TYPE, V_TYPE> &data) override;

            [[nodiscard]] bool isValid() const override;

            ~Iterator() override = default;
        };

        friend class Iterator;

        explicit JMap(Compare comp = Compare{}, ALLOC alloc = ALLOC{});

        JMap(const JMap& other);

        JMap& operator=(const JMap& other);

        JMap(JMap&& other) noexcept;

        JMap& operator=(JMap&& other) noexcept;

        [[nodiscard]] u_integer size() const override;

        bool contains(const couple<const K_TYPE, V_TYPE> &e) const override;

        bool add(const K_TYPE &k, const V_TYPE &v) override;

        bool remove(const K_TYPE &k) override;

        [[nodiscard]] bool containsKey(const K_TYPE &k) const override;

        V_TYPE get(const K_TYPE &k) const override;

        bool update(const K_TYPE &key, const V_TYPE &value) override;

        const V_TYPE & operator[](const K_TYPE &k) const override;

        V_TYPE & operator[](const K_TYPE &k) override;

        Iterator* begins() const override;

        Iterator* ends() const override;

        [[nodiscard]] std::string className() const override;

        [[nodiscard]] std::string toString(bool enter) const override;

        ~JMap() override;
    };
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::Iterator(
    vector<hashNode *, rebind_alloc_pointer> *buckets, u_integer bucket, hashNode *node)
    : hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator(
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

    hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator=(other);
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
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator+=(integer steps) const {
    hashTable<K_TYPE, V_TYPE, ALLOC, HASH>::Iterator::operator+=(steps);
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator-=(integer) const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::integer original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::operator-(
        const iterator<couple<const K_TYPE, V_TYPE>>&) const {
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
        const iterator<couple<const K_TYPE, V_TYPE>> *other) const {
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
        const iterator<couple<const K_TYPE, V_TYPE>> *other) const {
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
typename original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator*
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
void original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::Iterator::set(const couple<const K_TYPE, V_TYPE>&) {
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

    this->buckets = this->bucketsCopy(other.buckets);
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
    other.size_ = 0;
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
bool original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::update(const K_TYPE &key, const V_TYPE &value) {
    return this->modify(key, value);
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
std::string original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::toString(const bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(";
    bool first = true;
    for (auto it = this->begin(); it != this->end(); it.next()){
        if (!first){
            ss << ", ";
        }
        ss << "{" << printable::formatString(it.get().template get<0>()) << ": "
           << printable::formatString(it.get().template get<1>()) << "}";
        first = false;
    }
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename K_TYPE, typename V_TYPE, typename HASH, typename ALLOC>
original::hashMap<K_TYPE, V_TYPE, HASH, ALLOC>::~hashMap() = default;

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::Iterator(RBTreeType* tree, RBNode* cur)
    : RBTreeType::Iterator(tree, cur)  {}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::equalPtr(
    const iterator<couple<const K_TYPE, V_TYPE>>* other) const
{
    auto other_it = dynamic_cast<const Iterator*>(other);
    return other_it &&
           this->tree_ == other_it->tree_ &&
           this->cur_ == other_it->cur_;
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::Iterator(const Iterator& other) : Iterator(nullptr, nullptr)
{
    this->operator=(other);
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
typename original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator&
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator=(const Iterator& other)
{
    if (this == &other) {
        return *this;
    }

    this->tree_ = other.tree_;
    this->cur_ = other.cur_;
    return *this;
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
typename original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::clone() const
{
    return new Iterator(*this);
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
std::string original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::className() const
{
    return "treeMap::Iterator";
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator+=(integer steps) const
{
    RBTreeType::Iterator::operator+=(steps);
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator-=(integer steps) const
{
    RBTreeType::Iterator::operator-=(steps);
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::integer
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator-(
    const iterator<couple<const K_TYPE, V_TYPE>>&) const
{
    throw unSupportedMethodError();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::hasNext() const
{
    return RBTreeType::Iterator::hasNext();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::hasPrev() const
{
    return RBTreeType::Iterator::hasPrev();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::atPrev(
    const iterator<couple<const K_TYPE, V_TYPE>>* other) const
{
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

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::atNext(
    const iterator<couple<const K_TYPE, V_TYPE>>* other) const
{
    return other->atNext(*this);
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::next() const
{
    RBTreeType::Iterator::next();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::prev() const
{
    RBTreeType::Iterator::prev();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
typename original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::getPrev() const
{
    auto it = this->clone();
    it->prev();
    return it;
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::couple<const K_TYPE, V_TYPE>& original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::get()
{
    return RBTreeType::Iterator::get();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::couple<const K_TYPE, V_TYPE> original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::get() const
{
    return RBTreeType::Iterator::get();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::set(const couple<const K_TYPE, V_TYPE>&)
{
    throw unSupportedMethodError();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::isValid() const
{
    return RBTreeType::Iterator::isValid();
}


template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::treeMap(Compare comp, ALLOC alloc)
    : RBTreeType(std::move(comp)),
      map<K_TYPE, V_TYPE, ALLOC>(std::move(alloc)) {}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::treeMap(const treeMap& other) : treeMap() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>&
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator=(const treeMap& other) {
    if (this == &other){
        return *this;
    }

    this->destroyTree();
    this->root_ = other.treeCopy();
    this->size_ = other.size_;
    if constexpr(ALLOC::propagate_on_container_copy_assignment::value) {
        this->allocator = other.allocator;
        this->rebind_alloc = other.rebind_alloc;
    }
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::treeMap(treeMap&& other) noexcept : treeMap() {
    this->operator=(std::move(other));
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>&
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator=(treeMap&& other) noexcept {
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

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::u_integer original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::size() const {
    return this->size_;
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::contains(const couple<const K_TYPE, V_TYPE> &e) const {
    return this->containsKey(e.first()) && this->get(e.first()) == e.second();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::add(const K_TYPE &k, const V_TYPE &v) {
    return this->insert(k, v);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::remove(const K_TYPE &k) {
    return this->erase(k);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::containsKey(const K_TYPE &k) const {
    return this->find(k);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
V_TYPE original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::get(const K_TYPE &k) const {
    auto node = this->find(k);
    if (!node)
        throw noElementError();
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::update(const K_TYPE &key, const V_TYPE &value) {
    return this->modify(key, value);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
const V_TYPE &original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator[](const K_TYPE &k) const {
    auto node = this->find(k);
    if (!node)
        throw noElementError();
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
V_TYPE &original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator[](const K_TYPE &k) {
    auto node = this->find(k);
    if (!node) {
        this->insert(k, V_TYPE{});
        node = this->find(k);
    }
    return node->getValue();
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
typename original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::begins() const
{
    return new Iterator(const_cast<treeMap*>(this), this->getMinNode());
}

template <typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
typename original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::ends() const
{
    return new Iterator(const_cast<treeMap*>(this), this->getMaxNode());
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
std::string original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::className() const {
    return "treeMap";
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
std::string original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::toString(const bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(";
    bool first = true;
    for (auto it = this->begin(); it != this->end(); it.next()){
        if (!first){
            ss << ", ";
        }
        ss << "{" << printable::formatString(it.get().template get<0>()) << ": "
           << printable::formatString(it.get().template get<1>()) << "}";
        first = false;
    }
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::treeMap<K_TYPE, V_TYPE, Compare, ALLOC>::~treeMap() = default;

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::equalPtr(
        const iterator<couple<const K_TYPE, V_TYPE>> *other) const {
    auto other_it = dynamic_cast<const Iterator*>(other);
    return other_it && this->cur_ == other_it->cur_;
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::Iterator(skipListNode* cur)
    : skipListType::Iterator(cur) {}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::Iterator(const Iterator& other)
    : skipListType::Iterator(other) {}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator&
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator=(const Iterator& other) {
    skipListType::Iterator::operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::clone() const {
    return new Iterator(*this);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
std::string
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::className() const {
    return "JMap::Iterator";
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator+=(integer steps) const {
    skipListType::Iterator::operator+=(steps);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator-=(integer) const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::integer
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::operator-(
        const iterator<couple<const K_TYPE, V_TYPE>> &other) const {
    auto other_it = dynamic_cast<const Iterator*>(&other);
    if (other_it == nullptr)
        return this > &other ?
               std::numeric_limits<integer>::max() :
               std::numeric_limits<integer>::min();
    return skipListType::Iterator::operator-(*other_it);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::hasNext() const {
    return skipListType::Iterator::hasNext();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::hasPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::atPrev(
        const iterator<couple<const K_TYPE, V_TYPE>>* other) const {
    const auto other_it = dynamic_cast<const Iterator*>(other);
    if (!other_it){
        return false;
    }
    auto cloned_it = ownerPtr(other_it->clone());
    return this->equalPtr(cloned_it.get());
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::atNext(
        const original::iterator<original::couple<const K_TYPE, V_TYPE>>* other) const {
    return other->atPrev(*this);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::next() const {
    skipListType::Iterator::next();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::prev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::getPrev() const {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::couple<const K_TYPE, V_TYPE> &original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::get() {
    return skipListType::Iterator::get();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::couple<const K_TYPE, V_TYPE> original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::get() const {
    return skipListType::Iterator::get();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
void original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::set(const original::couple<const K_TYPE, V_TYPE>&) {
    throw unSupportedMethodError();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator::isValid() const {
    return skipListType::Iterator::isValid();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::JMap(Compare comp, ALLOC alloc)
    : skipListType(std::move(comp)) ,
      map<K_TYPE, V_TYPE, ALLOC>(std::move(alloc)) {}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::JMap(const JMap& other) : JMap() {
    this->operator=(other);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>&
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator=(const JMap& other) {
    if (this == &other){
        return *this;
    }

    this->listDestroy();
    this->head_ = other.listCopy();
    this->size_ = other.size_;
    this->compare_ = other.compare_;
    if constexpr(ALLOC::propagate_on_container_copy_assignment::value) {
        this->allocator = other.allocator;
        this->rebind_alloc = other.rebind_alloc;
    }
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::JMap(JMap&& other) noexcept : JMap() {
    this->operator=(std::move(other));
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>&
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator=(JMap&& other) noexcept {
    if (this == &other){
        return *this;
    }

    this->listDestroy();
    this->head_ = other.head_;
    other.head_ = other.createNode();
    this->size_ = other.size_;
    other.size_ = 0;
    this->compare_ = std::move(other.compare_);
    if constexpr(ALLOC::propagate_on_container_move_assignment::value) {
        this->allocator = std::move(other.allocator);
        this->rebind_alloc = std::move(other.rebind_alloc);
    }
    return *this;
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::u_integer original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::size() const {
    return this->size_;
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::contains(const couple<const K_TYPE, V_TYPE> &e) const {
    return this->containsKey(e.first()) && this->get(e.first()) == e.second();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::add(const K_TYPE &k, const V_TYPE &v) {
    return this->insert(k, v);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::remove(const K_TYPE &k) {
    return this->erase(k);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::containsKey(const K_TYPE &k) const {
    return this->find(k);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
V_TYPE original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::get(const K_TYPE &k) const {
    auto node = this->find(k);
    if (!node)
        throw noElementError();
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
bool original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::update(const K_TYPE &key, const V_TYPE &value) {
    return this->modify(key, value);
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
const V_TYPE& original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator[](const K_TYPE &k) const {
    auto node = this->find(k);
    if (!node)
        throw noElementError();
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
V_TYPE &original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::operator[](const K_TYPE &k) {
    auto node = this->find(k);
    if (!node) {
        this->insert(k, V_TYPE{});
        node = this->find(k);
    }
    return node->getValue();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::begins() const {
    return new Iterator(this->head_->getPNext(1));
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::Iterator*
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::ends() const {
    return new Iterator(this->findLastNode());
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
std::string original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::className() const {
    return "JMap";
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
std::string original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::toString(bool enter) const {
    std::stringstream ss;
    ss << this->className();
    ss << "(";
    bool first = true;
    for (auto it = this->begin(); it != this->end(); it.next()){
        if (!first){
            ss << ", ";
        }
        ss << "{" << printable::formatString(it.get().template get<0>()) << ": "
           << printable::formatString(it.get().template get<1>()) << "}";
        first = false;
    }
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename K_TYPE, typename V_TYPE, typename Compare, typename ALLOC>
original::JMap<K_TYPE, V_TYPE, Compare, ALLOC>::~JMap() = default;

#endif //MAPS_H
