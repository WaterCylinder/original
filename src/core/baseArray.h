#ifndef BASEARRAY_H
#define BASEARRAY_H
#include "serial.h"

namespace original {

    /**
     * @file baseArray.h
     * @brief Provides a base class for fixed-size serial containers.
     * @details This header defines the `baseArray` class, which serves as the base class for containers
     *          that have a fixed size. The `baseArray` class extends from the `serial` class, ensuring
     *          the encapsulation of a serial data structure while preventing size changes at runtime.
     *          It is designed for scenarios where the container size is predetermined and should remain
     *          constant throughout its lifetime.
     */

    /**
     * @class baseArray
     * @tparam TYPE Type of the elements contained in the base array
     * @tparam ALLOC Allocator type for memory management (default: allocator<TYPE>)
     * @brief Base class for fixed-size serial containers.
     * @extends serial
     * @details This class is used as the foundation for containers with a fixed size, such as static arrays.
     *          It inherits from @ref original::serial and provides the standard serial container functionality
     *          while maintaining a constant size throughout its existence.
     *
     *          The allocator is used to manage the fixed-size memory block for the array elements. Unlike
     *          dynamic containers, the allocation occurs only once during construction and remains fixed
     *          throughout the container's lifetime.
     *
     *          Key characteristics:
     *          - Single allocation during construction
     *          - No reallocation operations
     *          - Fixed capacity matching the size
     *          - Allocator propagated to derived classes
     *
     *          The ALLOC type must meet the C++ allocator requirements and should provide:
     *          - allocate()/deallocate() methods
     *          - construct()/destroy() methods
     *          - propagate_on_container_copy_assignment/move_assignment typedefs
     */
    template <typename TYPE, typename ALLOC = allocator<TYPE>>
    class baseArray : public serial<TYPE, ALLOC> {
    protected:
        /**
         * @brief Constructs a baseArray with specified allocator
         * @param alloc Allocator instance to use (default: default-constructed ALLOC)
         * @details The allocator will be used to allocate and manage the fixed-size memory block.
         *          Derived classes should properly initialize their fixed-size storage using this allocator.
         * @note The size/capacity is fixed after construction and cannot be changed.
         */
        using serial<TYPE, ALLOC>::serial;
    };

} // namespace original

#endif //BASEARRAY_H
