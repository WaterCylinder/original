#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once

#include "config.h"
#include "allocator.h"


namespace original {

/**
 * @file container.h
 * @brief Abstract base class for container types.
 * @details Defines a generic interface for container classes that store elements of type `TYPE`.
 *          Provides basic functionality like querying the size, checking if the container is empty,
 *          and checking if an element exists in the container.
 */

/**
 * @class container
 * @tparam TYPE Type of the elements stored in the container.
 * @tparam ALLOC Allocator type for memory management (default is allocator<TYPE>)
 * @brief Abstract base class for containers.
 * @details Provides a common interface for container classes to manage a collection of elements.
 *          Supports querying the size, checking for emptiness, and checking if an element is contained.
 *          Uses the provided allocator for all memory management operations.
 */
template <typename TYPE, typename ALLOC>
class container {
protected:
    /**
     * @brief The allocator instance used for memory management
     * @details Used by derived classes to allocate/deallocate memory and construct/destroy elements
     */
    ALLOC allocator;

    /**
     * @brief Constructs a container with specified allocator
     * @param alloc Allocator instance to use for memory management (has default-constructed ALLOC)
     * @details Initializes the container with the given allocator. The allocator will be used for
     *          all memory operations performed by the container.
     */
    explicit container(const ALLOC& alloc = ALLOC{});

    /**
     * @brief Allocates raw memory for elements
     * @param size Number of elements to allocate memory for
     * @return Pointer to the allocated memory
     * @note The memory is allocated but elements are not constructed
     */
    TYPE* allocate(u_integer size);

    /**
     * @brief Deallocates memory previously allocated by allocate()
     * @param ptr Pointer to the memory to deallocate
     * @param size Number of elements the memory was allocated for
     * @note Elements must be destroyed before de-allocation
     */
    void deallocate(TYPE* ptr, u_integer size);

    /**
     * @brief Constructs an element in-place
     * @tparam O_TYPE Type of object to construct (must be compatible with TYPE)
     * @tparam Args Types of constructor arguments
     * @param o_ptr Pointer to the memory where the object should be constructed
     * @param args Arguments to forward to the constructor
     * @details Uses the container's allocator to construct an object of type O_TYPE
     */
    template<typename O_TYPE, typename... Args>
    void construct(O_TYPE* o_ptr, Args&&... args);

    /**
     * @brief Destroys an element
     * @tparam O_TYPE Type of object to destroy (must be compatible with TYPE)
     * @param o_ptr Pointer to the object to destroy
     * @details Uses the container's allocator to properly destroy the object
     */
    template<typename O_TYPE>
    void destroy(O_TYPE* o_ptr);
public:
    /**
     * @brief Gets the number of elements in the container.
     * @return The size of the container.
     * @details This method must be implemented by derived classes to return the correct size.
     *
     * @code{.cpp}
     * container<int>* c = new vector<int>();
     * u_integer size = c->size(); // Returns the number of elements
     * @endcode
     */
    [[nodiscard]] virtual u_integer size() const = 0;

    /**
     * @brief Checks if the container is empty.
     * @return True if the container contains no elements, false otherwise.
     * @details This is implemented using `size()`, so it returns true if `size()` is zero.
     *
     * @code{.cpp}
     * container<int>* c = new vector<int>();
     * bool isEmpty = c->empty(); // Returns true if the container is empty
     * @endcode
     */
    [[nodiscard]] bool empty() const;

    /**
     * @brief Checks if an element is contained in the container.
     * @param e Element to check for presence in the container.
     * @return True if the element is found in the container, false otherwise.
     * @details This method must be implemented by derived classes to provide the actual containment check.
     *
     * @code{.cpp}
     * container<int>* c = new vector<int>();
     * bool contains = c->contains(42); // Returns true if 42 is in the container
     * @endcode
     */
    virtual bool contains(const TYPE& e) const = 0;

    /**
     * @brief Destructor for the container class.
     * @details Ensures proper cleanup of resources if necessary.
     */
    virtual ~container() = default;
};

} // namespace original

// ----------------- Definitions of container.h -----------------

template<typename TYPE, typename ALLOC>
original::container<TYPE, ALLOC>::container(const ALLOC& alloc)
    : allocator(alloc) {}

template<typename TYPE, typename ALLOC>
TYPE* original::container<TYPE, ALLOC>::allocate(u_integer size) {
    return this->allocator.allocate(size);
}

template<typename TYPE, typename ALLOC>
void original::container<TYPE, ALLOC>::deallocate(TYPE *ptr, u_integer size) {
    this->allocator.deallocate(ptr, size);
}

template<typename TYPE, typename ALLOC>
template<typename O_TYPE, typename... Args>
void original::container<TYPE, ALLOC>::construct(O_TYPE *o_ptr, Args &&... args) {
    this->allocator.construct(o_ptr, std::forward<Args>(args)...);
}

template<typename TYPE, typename ALLOC>
template<typename O_TYPE>
void original::container<TYPE, ALLOC>::destroy(O_TYPE *o_ptr) {
    this->allocator.destroy(o_ptr);
}

template <typename TYPE, typename ALLOC>
bool original::container<TYPE, ALLOC>::empty() const {
    return this->size() == 0;
}

#endif //CONTAINER_H
