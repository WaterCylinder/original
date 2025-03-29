#ifndef STACK_H
#define STACK_H

/**
 * @file stack.h
 * @brief Stack container adapter implementation
 * @details Provides a stack (LIFO) data structure adapter that uses underlying container types.
 * Supports common stack operations and works with different container implementations.
 */

#include "chain.h"
#include "containerAdapter.h"
#include <string>

namespace original {

    /**
     * @class stack
     * @tparam TYPE Type of elements stored in the stack
     * @tparam SERIAL Underlying container type (default: chain), see constraints from @ref original::containerAdapter
     * @brief Last-In-First-Out (LIFO) container adapter
     * @extends original::containerAdapter<TYPE, SERIAL>
     * @details Implements stack operations using the specified underlying container type.
     * Provides standard stack interface including push, pop, top, and comparison operations.
     * The adapter maintains elements through composition with the specified SERIAL container.
     *
     * @note The SERIAL template parameter inherits constraints from its base class
     * @ref original::containerAdapter. See base class documentation for requirements.
     */
    template<typename TYPE,
             template <typename, typename> typename SERIAL = chain,
             template <typename> typename ALLOC = allocator>
    class stack final : public containerAdapter<TYPE, SERIAL, ALLOC> {
    public:
        /**
         * @brief Constructs a stack with specified underlying container
         * @param serial Container instance to initialize stack (default: empty)
         */
        explicit stack(const SERIAL<TYPE, ALLOC<TYPE>>& serial = SERIAL<TYPE, ALLOC<TYPE>>{});

        /**
         * @brief Constructs stack from initializer list
         * @param lst List of elements to initialize the stack
         */
        stack(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Copy constructor
         * @param other Stack instance to copy from
         */
        stack(const stack& other);

        /**
         * @brief Copy assignment operator
         * @param other Stack instance to copy from
         * @return Reference to this stack
         */
        stack& operator=(const stack& other);

        /**
         * @brief Move constructor
         * @param other Stack instance to move from
         * @note noexcept guarantees no exceptions during move
         */
        stack(stack&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Stack instance to move from
         * @return Reference to this stack
         * @note noexcept guarantees no exceptions during move
         */
        stack& operator=(stack&& other) noexcept;

        /**
         * @brief Pushes element to the top of the stack
         * @param e Element to be pushed
         */
        void push(const TYPE& e);

        /**
         * @brief Removes and returns top element from the stack
         * @return The element removed from top of stack
         * @pre Stack should not be empty
         */
        TYPE pop();

        /**
         * @brief Accesses the top element of the stack
         * @return Const reference to the top element
         * @pre Stack should not be empty
         */
        TYPE top() const;

        /**
         * @brief Gets the class name identifier
         * @return "stack" string identifier
         */
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::stack<TYPE, SERIAL, ALLOC>::stack(const SERIAL<TYPE, ALLOC<TYPE>>& serial)
        : containerAdapter<TYPE, SERIAL, ALLOC>(serial) {}

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::stack<TYPE, SERIAL, ALLOC>::stack(const std::initializer_list<TYPE> &lst)
        : stack(SERIAL<TYPE, ALLOC<TYPE>>(lst)) {}

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::stack<TYPE, SERIAL, ALLOC>::stack(const stack& other)
        : containerAdapter<TYPE, SERIAL, ALLOC>(other.serial_) {}

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    auto original::stack<TYPE, SERIAL, ALLOC>::operator=(const stack& other) -> stack& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::stack<TYPE, SERIAL, ALLOC>::stack(stack&& other) noexcept : stack()
    {
        this->operator=(std::move(other));
    }

template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    auto original::stack<TYPE, SERIAL, ALLOC>::operator=(stack&& other) noexcept -> stack&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        other.serial_ = SERIAL<TYPE, ALLOC<TYPE>>{};
        return *this;
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    auto original::stack<TYPE, SERIAL, ALLOC>::push(const TYPE& e) -> void {
        this->serial_.pushEnd(e);
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    auto original::stack<TYPE, SERIAL, ALLOC>::pop() -> TYPE {
        return this->serial_.popEnd();
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    auto original::stack<TYPE, SERIAL, ALLOC>::top() const -> TYPE {
        return this->serial_.getEnd();
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    std::string original::stack<TYPE, SERIAL, ALLOC>::className() const {
        return "stack";
    }

#endif // STACK_H
