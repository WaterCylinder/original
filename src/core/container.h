#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once

#include "config.h"

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
 * @brief Abstract base class for containers.
 * @details Provides a common interface for container classes to manage a collection of elements.
 *          Supports querying the size, checking for emptiness, and checking if an element is contained.
 */
template <typename TYPE>
class container {
public:
    /**
     * @brief Gets the number of elements in the container.
     * @return The size of the container.
     * @details This method must be implemented by derived classes to return the correct size.
     *
     * @code{.cpp}
     * container<int>* c = new vector<int>();
     * uint32_t size = c->size(); // Returns the number of elements
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

template <typename TYPE>
bool original::container<TYPE>::empty() const {
    return this->size() == 0;
}

#endif //CONTAINER_H
