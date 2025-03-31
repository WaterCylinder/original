#ifndef TRANSFORM_H
#define TRANSFORM_H


#include "allocator.h"
#include "baseList.h"
#include "cloneable.h"
#include "maths.h"

/**
 * @file transform.h
 * @brief Transformation classes for various operations on elements.
 * @details This file provides abstract and concrete transformation classes that apply different operations on elements.
 *          The operations include addition, multiplication, assignment, absolute value, and copying elements into a container.
 */

namespace original {

    /**
     * @class transform
     * @tparam TYPE The type of element being transformed
     * @brief Base class for transformation operations.
     * @details This class provides an interface for transformations that apply operations to elements of type `TYPE`.
     *          Derived classes must implement the `apply` method to perform the specific transformation.
     *          The `operator()` allows the transformation to be applied by invoking the `apply` method.
     */
    template<typename TYPE>
    class transform : public cloneable {
    protected:
        /**
         * @brief Applies the transformation to the given element.
         * @param t The element to apply the transformation to.
         * @details Derived classes should implement this method to perform specific transformations on the element.
         */
        virtual void apply(TYPE& t);

    public:
        /**
         * @brief Virtual destructor for the transform class.
         */
        ~transform() override = default;

        /**
         * @brief Clones the transform object.
         * @return A pointer to a new instance of the transform object.
         */
        transform* clone() const override;

        /**
         * @brief Applies the transformation to the given element.
         * @param t The element to apply the transformation to.
         * @details This operator calls the `apply` method to perform the transformation.
         */
        virtual void operator()(TYPE& t);
    };

    /**
     * @class addOptTransform
     * @tparam TYPE The type of element being transformed
     * @brief Transformation that adds a given value to an element.
     * @extends transform
     * @details This transformation adds a specified value to an element.
     */
    template<typename TYPE>
    class addOptTransform final : public transform<TYPE> {
        TYPE t_; ///< The value to add to the element

        /**
         * @brief Applies the transformation by adding a value to the element.
         * @param t The element to transform.
         */
        void apply(TYPE &t) override;

    public:
        /**
         * @brief Clones the addOptTransform object.
         * @return A pointer to a new instance of addOptTransform.
         */
        addOptTransform* clone() const override;

        /**
         * @brief Constructs an addOptTransform with the specified value.
         * @param t The value to add to the element.
         */
        explicit addOptTransform(const TYPE& t);
    };

    /**
     * @class assignOptTransform
     * @tparam TYPE The type of element being transformed
     * @brief Transformation that assigns a specified value to an element.
     * @extends transform
     * @details This transformation assigns a specified value to an element.
     */
    template<typename TYPE>
    class assignOptTransform final : public transform<TYPE> {
        TYPE val_; ///< The value to assign to the element

        /**
         * @brief Applies the transformation by assigning the value to the element.
         * @param t The element to transform.
         */
        void apply(TYPE &t) override;

    public:
        /**
         * @brief Clones the assignOptTransform object.
         * @return A pointer to a new instance of assignOptTransform.
         */
        assignOptTransform* clone() const override;

        /**
         * @brief Constructs an assignOptTransform with the specified value.
         * @param val The value to assign to the element.
         */
        explicit assignOptTransform(const TYPE& val);
    };

    /**
     * @class multiOptTransform
     * @tparam TYPE The type of element being transformed
     * @brief Transformation that multiplies an element by a given value.
     * @extends transform
     * @details This transformation multiplies an element by a specified value.
     */
    template<typename TYPE>
    class multiOptTransform final : public transform<TYPE> {
        TYPE t_; ///< The value to multiply the element by

        /**
         * @brief Applies the transformation by multiplying the element by a value.
         * @param t The element to transform.
         */
        void apply(TYPE &t) override;

    public:
        /**
         * @brief Clones the multiOptTransform object.
         * @return A pointer to a new instance of multiOptTransform.
         */
        multiOptTransform* clone() const override;

        /**
         * @brief Constructs a multiOptTransform with the specified value.
         * @param t The value to multiply the element by.
         */
        explicit multiOptTransform(const TYPE& t);
    };

    /**
     * @class absTransform
     * @tparam TYPE The type of element being transformed
     * @brief Transformation that converts an element to its absolute value.
     * @extends transform
     * @details This transformation applies the absolute value operation to the element.
     */
    template<typename TYPE>
    class absTransform final : public transform<TYPE> {
        /**
         * @brief Applies the transformation by converting the element to its absolute value.
         * @param t The element to transform.
         */
        void apply(TYPE &t) override;

    public:
        /**
         * @brief Clones the absTransform object.
         * @return A pointer to a new instance of absTransform.
         */
        absTransform* clone() const override;

        /**
         * @brief Default constructor for absTransform.
         */
        explicit absTransform() = default;
    };

    /**
     * @class copyTransform
     * @tparam TYPE The type of element being transformed
     * @brief Transformation that copies an element into a container.
     * @extends transform
     * @details This transformation copies an element into a specified container.
     */
    template<typename TYPE, typename ALLOC = allocator<TYPE>>
    class copyTransform final : public transform<TYPE> {
        baseList<TYPE, ALLOC>* container_; ///< The container to copy the element into

        /**
         * @brief Applies the transformation by copying the element into the container.
         * @param t The element to transform.
         */
        void apply(TYPE &t) override;

    public:
        /**
         * @brief Clones the copyTransform object.
         * @return A pointer to a new instance of copyTransform.
         */
        copyTransform* clone() const override;

        /**
         * @brief Constructs a copyTransform with the specified container.
         * @param container The container to copy the element into.
         */
        explicit copyTransform(baseList<TYPE, ALLOC>& container);
    };

} // namespace original

    template<typename TYPE>
    void original::transform<TYPE>::apply(TYPE&) {}

    template <typename TYPE>
    auto original::transform<TYPE>::clone() const -> transform*
    {
        return new transform(*this);
    }

    template<typename TYPE>
    auto original::transform<TYPE>::operator()(TYPE& t) -> void
    {
        this->apply(t);
    }

    template<typename TYPE>
    original::assignOptTransform<TYPE>::assignOptTransform(const TYPE& val)
        : val_(val) {}

    template <typename TYPE>
    auto original::assignOptTransform<TYPE>::clone() const -> assignOptTransform*
    {
        return new assignOptTransform(*this);
    }

    template<typename TYPE>
    auto original::assignOptTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = this->val_;
    }

    template<typename TYPE>
    original::addOptTransform<TYPE>::addOptTransform(const TYPE &t) : t_(t) {}

    template <typename TYPE>
    auto original::addOptTransform<TYPE>::clone() const -> addOptTransform*
    {
        return new addOptTransform(*this);
    }

    template<typename TYPE>
    auto original::addOptTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = t + this->t_;
    }

    template<typename TYPE>
    original::multiOptTransform<TYPE>::multiOptTransform(const TYPE &t) : t_(t) {}

    template <typename TYPE>
    auto original::multiOptTransform<TYPE>::clone() const -> multiOptTransform*
    {
        return new multiOptTransform(*this);
    }

    template<typename TYPE>
    auto original::multiOptTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = t * this->t_;
    }

    template <typename TYPE>
    auto original::absTransform<TYPE>::clone() const -> absTransform*
    {
        return new absTransform(*this);
    }

    template<typename TYPE>
    auto original::absTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = original::abs(t);
    }

    template<typename TYPE, typename ALLOC>
    original::copyTransform<TYPE, ALLOC>::copyTransform(baseList<TYPE, ALLOC>& container)
        : container_(&container) {}

    template<typename TYPE, typename ALLOC>
    auto original::copyTransform<TYPE, ALLOC>::clone() const -> copyTransform*
    {
        return new copyTransform(*this);
    }

    template<typename TYPE, typename ALLOC>
    auto original::copyTransform<TYPE, ALLOC>::apply(TYPE& t) -> void
    {
        this->container_->add(t);
    }

#endif //TRANSFORM_H
