#ifndef CLONEABLE_H
#define CLONEABLE_H
#include "config.h"


/**
 * @file cloneable.h
 * @brief Abstract base class for cloneable objects.
 * @details Provides functionality for cloning derived objects using the "Prototype" design pattern.
 */

namespace original {
/**
 * @class baseCloneable
 * @tparam DERIVED The type of the derived class.
 * @brief Base class that defines a cloneable interface.
 * @details This class provides the mechanism to clone derived objects by calling `clone()` in derived classes.
 *          It is designed for situations where the cloning behavior does not need to be customized.
 *          Derived classes can simply use the default implementation of `clone()` provided by this class.
 */
template<typename DERIVED>
class baseCloneable {
protected:
    /**
     * @brief Default constructor for baseCloneable.
     * @details The constructor is protected to prevent direct instantiation of baseCloneable.
     */
    baseCloneable() = default;

public:
    /**
     * @brief Creates a clone of the current object.
     * @return Pointer to a newly created object of the derived type, identical to the current object.
     * @details This function uses dynamic casting to create a new instance of the derived class.
     *          This default implementation is sufficient when derived classes do not need to customize
     *          their cloning behavior.
     *
     * @code{.cpp}
     * class Derived : public baseCloneable<Derived> {
     *     // No need to override clone() if default behavior is sufficient.
     * };
     * @endcode
     */
    [[nodiscard]] virtual DERIVED* clone() const;

    /**
     * @brief Virtual destructor for baseCloneable.
     * @details Ensures proper destruction of derived objects.
     */
    virtual ~baseCloneable() = default;
};

/**
 * @class cloneable
 * @brief Concrete cloneable class with custom cloning behavior.
 * @details This class is for cases where the derived class requires custom cloning behavior.
 *          Derived classes must override the `clone()` function to provide their own cloning logic.
 */
class cloneable : public baseCloneable<cloneable> {
protected:
    /**
     * @brief Default constructor for cloneable.
     * @details The constructor is protected to control instantiation in derived classes.
     */
    cloneable() = default;

public:
    /**
     * @brief Virtual destructor for cloneable.
     * @details Ensures proper cleanup when a `cloneable` object is destroyed.
     */
    ~cloneable() override = default;
};

} // namespace original

// ----------------- Definitions of cloneable.h -----------------

template<typename DERIVED>
auto original::baseCloneable<DERIVED>::clone() const -> DERIVED* {
    return new DERIVED(static_cast<const DERIVED&>(*this));
}

#endif //CLONEABLE_H
