#ifndef BASELIST_H
#define BASELIST_H
#include "serial.h"

namespace original {

    /**
     * @file baseList.h
     * @brief Provides a base class for variable-size serial containers.
     * @details This header defines the `baseList` class, which serves as the base class for containers
     *          that can dynamically change in size. The `baseList` class extends from the `serial` class,
     *          providing the core functionality for serial containers while allowing for dynamic resizing.
     *          It is intended for use in scenarios where the container's size can change during the container's
     *          lifetime, such as linked lists or dynamic arrays.
     */

    /**
     * @class baseList
     * @tparam TYPE Type of the elements contained in the base list
     * @tparam ALLOC Allocator type for dynamic memory management (default depends on derived class)
     * @brief Base class for variable-size serial containers.
     * @extends serial
     * @details This class serves as the base class for containers that can dynamically grow or shrink in size.
     *          It inherits from @ref original::serial and provides an interface for manipulating elements in a
     *          variable-size sequence.
     *
     *          The allocator is used for all dynamic memory operations including:
     *          - Memory allocation/de-allocation when growing/shrinking
     *          - Element construction/destruction
     *          - Node allocation in linked implementations
     *
     *          Key memory characteristics:
     *          - Dynamic resizing operations may trigger reallocations
     *          - Allocator state is preserved during copy/move operations
     *          - Strong exception safety guarantee for most operations
     *
     *          The ALLOC type must meet C++ allocator requirements and provide:
     *          - allocate()/deallocate() methods
     *          - construct()/destroy() methods
     *          - propagate_on_container_copy_assignment/move_assignment typedefs
     */
    template <typename TYPE, typename ALLOC>
    class baseList : public serial<TYPE, ALLOC> {
    protected:
        /**
         * @brief Constructs a baseList with specified allocator
         * @param alloc Allocator instance to use (default: default-constructed ALLOC)
         * @details The allocator will be used for all dynamic memory operations.
         *          Derived classes should use this allocator for their storage management.
         */
        using serial<TYPE, ALLOC>::serial;
    public:
        /**
         * @brief Adds an element to the end of the list.
         * @param e The element to be added to the list.
         * @details This method adds an element to the end of the container by calling `pushEnd`.
         */
        virtual void add(const TYPE& e);

        /**
         * @brief Removes an element from the list.
         * @param e The element to be removed.
         * @return The removed element.
         * @details This method finds the element in the list and removes it by calling `pop` at the found index.
         */
        virtual TYPE remove(const TYPE& e);

        /**
         * @brief Removes all elements
         * @details Uses the allocator to:
         *          - Destroy all elements
         *          - Deallocate storage if applicable
         * @note No-throw guarantee if element destructors don't throw
         */
        virtual void clear();

        /**
         * @brief Inserts an element at a specific index.
         * @param index The index where the element should be inserted.
         * @param e The element to be inserted.
         * @details This method must be implemented by derived classes.
         */
        virtual void push(integer index, const TYPE& e) = 0;

        /**
         * @brief Removes an element from a specific index.
         * @param index The index of the element to be removed.
         * @return The removed element.
         * @details This method must be implemented by derived classes.
         */
        virtual TYPE pop(integer index) = 0;

        /**
         * @brief Inserts an element at the beginning of the list.
         * @param e The element to be inserted.
         * @details This method must be implemented by derived classes.
         */
        virtual void pushBegin(const TYPE& e) = 0;

        /**
         * @brief Removes an element from the beginning of the list.
         * @return The removed element.
         * @details This method must be implemented by derived classes.
         */
        virtual TYPE popBegin() = 0;

        /**
         * @brief Inserts an element at the end of the list.
         * @param e The element to be inserted.
         * @details This method must be implemented by derived classes.
         */
        virtual void pushEnd(const TYPE& e) = 0;

        /**
         * @brief Removes an element from the end of the list.
         * @return The removed element.
         * @details This method must be implemented by derived classes.
         */
        virtual TYPE popEnd() = 0;
    };
}

    template <typename TYPE, typename ALLOC>
    auto original::baseList<TYPE, ALLOC>::add(const TYPE &e) -> void
    {
        this->pushEnd(e);
    }

    template <typename TYPE, typename ALLOC>
    auto original::baseList<TYPE, ALLOC>::remove(const TYPE& e) -> TYPE
    {
        const integer index = this->indexOf(e);
        return this->pop(index);
    }

    template <typename TYPE, typename ALLOC>
    auto original::baseList<TYPE, ALLOC>::clear() -> void {
        while (!this->empty()) {
            this->popEnd();
        }
    }

#endif //BASELIST_H
