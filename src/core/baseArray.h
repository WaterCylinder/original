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
     * @brief Base class for fixed-size serial containers.
     * @extends serial
     * @details This class is used as the foundation for containers with a fixed size, such as static arrays.
     *          It inherits from @ref original::serial and provides the standard serial container functionality
     *          while maintaining a constant size throughout its existence.
     *
     *          Derived classes must implement specific behavior for accessing and modifying elements, but cannot
     *          alter the size of the container.
     */
    template <typename TYPE>
    class baseArray : public serial<TYPE> {};

} // namespace original

#endif //BASEARRAY_H
