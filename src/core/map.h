#ifndef MAP_H
#define MAP_H

#include "allocator.h"
#include "container.h"
#include "couple.h"



/**
 * @file map.h
 * @brief Abstract base class for map-like container implementations
 * @details Provides the interface for key-value pair containers that will be
 * implemented by concrete classes like hashMap and treeMap. The map maintains
 * unique keys and allows efficient value lookup and modification.
 *
 * Key Features:
 * - Type-safe key-value pair storage
 * - Basic CRUD operations (add, remove, update, get)
 * - Const and non-const element access
 * - Key existence checking
 * - Memory management through allocator support
 */

namespace original {
    
    /**
     * @class map
     * @tparam K_TYPE The key type (must support equality comparison and hashing)
     * @tparam V_TYPE The value type to store
     * @tparam ALLOC Allocator type for memory management (default: allocator)
     * @brief Abstract base class for key-value mapping containers
     * @details This abstract class defines the common interface for all
     * map implementations in the framework. Concrete implementations should
     * inherit from this class and provide the actual storage mechanism.
     *
     * The map guarantees:
     * - Unique keys (no duplicates)
     * - Efficient value lookup by key
     * - Type-safe operations
     * - Exception safety (throws noElementError for invalid access)
     *
     * @note This class is not meant to be instantiated directly - use concrete
     * implementations like hashMap or treeMap instead.
     */
    template <typename K_TYPE,
              typename V_TYPE,
              typename ALLOC = allocator<couple<const K_TYPE, V_TYPE>>>
    class map : public container<couple<const K_TYPE, V_TYPE>, ALLOC> {
    protected:
        /**
         * @brief Constructs a map with the specified allocator
         * @param alloc Allocator to use for memory management
         */
        explicit map(ALLOC alloc = ALLOC{});

    public:
        /**
         * @brief Adds a new key-value pair to the map
         * @param k The key to add
         * @param v The value to associate with the key
         * @return true if the pair was added successfully,
         *         false if the key already exists
         */
        virtual bool add(const K_TYPE& k, const V_TYPE& v) = 0;

        /**
         * @brief Removes a key-value pair from the map
         * @param k The key to remove
         * @return true if the pair was removed,
         *         false if the key didn't exist
         */
        virtual bool remove(const K_TYPE& k) = 0;

        /**
         * @brief Checks if a key exists in the map
         * @param k The key to check
         * @return true if the key exists, false otherwise
         */
        [[nodiscard]] virtual bool containsKey(const K_TYPE& k) const = 0;

        /**
         * @brief Gets the value associated with a key
         * @param k The key to look up
         * @return The associated value
         * @throw noElementError if the key doesn't exist
         */
        virtual V_TYPE get(const K_TYPE& k) const = 0;

        /**
         * @brief Updates the value for an existing key
         * @param key The key to update
         * @param value The new value to associate with the key
         * @return true if the update succeeded (key existed),
         *         false if the key didn't exist
         * @note Unlike operator[], this will not insert new keys
         */
        virtual bool update(const K_TYPE &key, const V_TYPE &value) = 0;

        /**
         * @brief Const access to mapped values
         * @param k The key to access
         * @return const reference to the mapped value
         * @throw noElementError if the key doesn't exist
         * @note This version is for read-only access and will not modify the map
         */
        virtual const V_TYPE& operator[](const K_TYPE& k) const = 0;

        /**
         * @brief Non-const access to mapped values
         * @param k The key to access
         * @return reference to the mapped value
         * @note If the key doesn't exist, inserts a new key-value pair with:
         *       - The specified key
         *       - A default-constructed value
         *       Then returns reference to the new value
         */
        virtual V_TYPE& operator[](const K_TYPE& k) = 0;

        /**
         * @brief Virtual destructor
         */
        ~map() override;
    };

    template<typename K_TYPE, typename V_TYPE, typename ALLOC>
    map<K_TYPE, V_TYPE, ALLOC>::map(ALLOC alloc)
            : container<couple<const K_TYPE, V_TYPE>, ALLOC>(std::move(alloc)) {}

    template<typename K_TYPE, typename V_TYPE, typename ALLOC>
    map<K_TYPE, V_TYPE, ALLOC>::~map() = default;
}

#endif //MAP_H
