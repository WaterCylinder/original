#ifndef WRAPPER_H
#define WRAPPER_H
#pragma once
#include <sstream>
#include <string>
#include "printable.h"

/**
 * @file wrapper.h
 * @brief Abstract polymorphic container with value encapsulation and navigation support.
 * @details Defines a template-based interface for objects that wrap values while maintaining
 *          bidirectional links to other containers. Integrates with printable for diagnostics.
 */

namespace original {

/**
 * @class wrapper
 * @tparam TYPE Type of encapsulated value
 * @brief Base class for linked value containers with formatted output.
 * @extends printable
 * @details Combines three core capabilities:
 *          1. Value storage and access
 *          2. Node navigation
 *          3. Polymorphic string representation through printable interface
 */
template <typename TYPE>
class wrapper : public printable {
public:
    /**
     * @brief Accesses the contained value (mutable).
     * @return Reference to the stored value.
     */
    virtual TYPE& getVal() = 0;

    /**
     * @brief Accesses the contained value (immutable).
     * @return Const reference to the stored value.
     */
    virtual const TYPE& getVal() const = 0;

    /**
     * @brief Updates the contained value.
     * @param data New value to store.
     * @post getVal() == data
     */
    virtual void setVal(TYPE data) = 0;

    /**
     * @brief Retrieves previous node in container sequence.
     * @return Pointer to predecessor node or nullptr if first element.
     */
    virtual wrapper* getPPrev() const = 0;

    /**
     * @brief Retrieves next node in container sequence.
     * @return Pointer to successor node or nullptr if last element.
     */
    virtual wrapper* getPNext() const = 0;

    /**
     * @brief Destructor for the wrapper class.
     * @details The destructor ensures proper cleanup of the object. Since `wrapper` is an abstract base class,
     *          the destructor is virtual to allow derived classes to perform their own cleanup as needed
     *          before the base class destructor is invoked.
     */
    ~wrapper() override = default;

    /**
     * @brief Provides class identifier for diagnostics.
     * See @ref original::printable::className()
     * @details Base implementation returns "wrapper".
     * @code{.cpp}
     * class my_wrapper : public wrapper<int> {
     *     std::string className() const override { return "my_wrapper"; }
     * };
     * @endcode
     */
    [[nodiscard]] std::string className() const override;

    /**
     * @brief Generates formatted diagnostic string.
     * See @ref original::printable::toString()
     * @param enter Add trailing newline when true.
     * @return String in format "ClassName(address, value)".
     *
     * @code{.cpp}
     * wrapper<int>* w = new node<int>(42);
     * std::cout << w->toString(true); // "wrapper(@0x1234, 42)\n"
     * @endcode
     */
    [[nodiscard]] std::string toString(bool enter) const override;
};

} // namespace original

// ----------------- Definitions of wrapper.h -----------------

template <typename TYPE>
auto original::wrapper<TYPE>::className() const -> std::string
{
    return "wrapper";
}

template<typename TYPE>
auto original::wrapper<TYPE>::toString(const bool enter) const -> std::string
{
    std::stringstream ss;
    ss << this->className() << "("
       << formatString(this) << ", "
       << formatString(this->getVal()) << ")";
    if (enter) ss << "\n";
    return ss.str();
}

#endif // WRAPPER_H