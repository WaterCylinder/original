#ifndef SET_H
#define SET_H


#include "allocator.h"
#include "container.h"


/**
 * @file set.h
 * @brief Abstract base class for set container implementations
 * @details Provides the interface for set containers that will be
 * implemented by concrete classes like hashSet and treeSet. The set maintains
 * unique elements and provides efficient membership testing.
 *
 * Key Features:
 * - Type-safe element storage
 * - Unique element guarantee (no duplicates)
 * - Basic operations (add, remove)
 * - Memory management through allocator support
 */

namespace original {

    /**
     * @class set
     * @tparam K_TYPE The element type (must support equality comparison)
     * @tparam ALLOC Allocator type for memory management (default: allocator)
     * @brief Abstract base class for unique element containers
     * @details This abstract class defines the common interface for all
     * set implementations in the framework. Concrete implementations should
     * inherit from this class and provide the actual storage mechanism.
     *
     * The set guarantees:
     * - Unique elements (no duplicates)
     * - Efficient membership testing
     * - Type-safe operations
     * - Exception safety
     *
     * @note This class is not meant to be instantiated directly - use concrete
     * implementations like hashSet or treeSet instead.
     */
    template <typename K_TYPE, typename ALLOC = allocator<K_TYPE>>
    class set : public container<K_TYPE, ALLOC> {
    protected:

        /**
         * @brief Constructor implementation
         * @tparam K_TYPE The element type
         * @tparam ALLOC The allocator type
         * @param alloc Allocator to use for memory management
         * @details Initializes the base container with the provided allocator
         */
        explicit set(ALLOC alloc = ALLOC{});

    public:

        /**
         * @brief Adds a new element to the set
         * @param e The element to add
         * @return true if the element was added successfully,
         *         false if the element already exists
         */
        virtual bool add(const K_TYPE& e) = 0;

        /**
         * @brief Removes an element from the set
         * @param e The element to remove
         * @return true if the element was removed,
         *         false if the element didn't exist
         */
        virtual bool remove(const K_TYPE& e) = 0;

        /**
         * @brief Destructor implementation
         * @tparam K_TYPE The element type
         * @tparam ALLOC The allocator type
         * @details Default implementation allows proper destruction through base pointer
         */
        ~set() override;
    };
}

template<typename K_TYPE, typename ALLOC>
original::set<K_TYPE, ALLOC>::set(ALLOC alloc)
    : container<K_TYPE, ALLOC>(std::move(alloc)) {}

template<typename K_TYPE, typename ALLOC>
original::set<K_TYPE, ALLOC>::~set() = default;

#endif //SET_H
