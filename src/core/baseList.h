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
     * @brief Base class for variable-size serial containers.
     * @extends serial
     * @details This class serves as the base class for containers that can dynamically grow or shrink in size.
     *          It inherits from @ref original::serial and provides an interface for manipulating elements in a
     *          variable-size sequence.
     *
     *          Derived classes can implement their own logic for adding, removing, and managing elements in the list.
     *          It supports various operations for inserting and removing elements at the beginning, end, or specific
     *          indices within the container.
     *
     *          This class defines the basic functionality for variable-size containers but requires derived classes
     *          to implement certain actions like element insertion and removal at arbitrary positions.
     */
    template <typename TYPE>
    class baseList : public serial<TYPE> {
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
         * @brief Clears all elements from the list.
         * @details This method removes all elements from the container by repeatedly calling `popEnd`.
         */
        virtual void clear();

        /**
         * @brief Inserts an element at a specific index.
         * @param index The index where the element should be inserted.
         * @param e The element to be inserted.
         * @details This method must be implemented by derived classes.
         */
        virtual void push(int64_t index, const TYPE& e) = 0;

        /**
         * @brief Removes an element from a specific index.
         * @param index The index of the element to be removed.
         * @return The removed element.
         * @details This method must be implemented by derived classes.
         */
        virtual TYPE pop(int64_t index) = 0;

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

    template <typename TYPE>
    auto original::baseList<TYPE>::add(const TYPE &e) -> void
    {
        this->pushEnd(e);
    }

    template <typename TYPE>
    auto original::baseList<TYPE>::remove(const TYPE& e) -> TYPE
    {
        const int64_t index = this->indexOf(e);
        return this->pop(index);
    }

    template<typename TYPE>
    auto original::baseList<TYPE>::clear() -> void {
        while (!this->empty()) {
            this->popEnd();
        }
    }

#endif //BASELIST_H
