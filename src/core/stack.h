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
     * @tparam SERIAL Underlying container type (default: chain)
     * @tparam ALLOC Allocator type for memory management (default: allocator<TYPE>)
     * @brief Last-In-First-Out (LIFO) container adapter
     * @extends original::containerAdapter
     * @details Implements stack operations using the specified underlying container type.
     * Provides standard stack interface including push, pop, top, and comparison operations.
     * The adapter maintains elements through composition with the specified SERIAL container.
     *
     * The allocator is propagated to both the stack adapter and the underlying serial
     * container for consistent memory management. The ALLOC type must meet the
     * C++ allocator requirements and provide:
     * - allocate()/deallocate() methods
     * - construct()/destroy() methods
     * - propagate_on_container_copy_assignment/move_assignment typedefs
     */
    template<typename TYPE,
             template <typename, typename> typename SERIAL = chain,
             template <typename> typename ALLOC = allocator>
    class stack final : public containerAdapter<TYPE, SERIAL, ALLOC> {
    public:
        /**
         * @brief Constructs a stack with specified underlying container and allocator
         * @param serial Container instance to initialize stack (default: empty)
         * @details The allocator from the provided container will be used for all memory
         * operations. If no container is provided, a default-constructed container with
         * default allocator will be used.
         */
        explicit stack(const SERIAL<TYPE, ALLOC<TYPE>>& serial = SERIAL<TYPE, ALLOC<TYPE>>{});

        /**
         * @brief Constructs stack from initializer list with allocator
         * @param lst List of elements to initialize the stack
         * @details Uses the default allocator to construct the underlying container
         * and populate it with elements from the initializer list.
         */
        stack(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Copy constructs a stack with allocator propagation
         * @param other Stack instance to copy from
         * @note The allocator is copied if ALLOC::propagate_on_container_copy_assignment is true
         */
        stack(const stack& other);

        /**
         * @brief Copy assignment operator
         * @param other Stack instance to copy from
         * @return Reference to this stack
         * @note The allocator is copied if ALLOC::propagate_on_container_copy_assignment is true
         */
        stack& operator=(const stack& other);

        /**
         * @brief Move constructs a stack with allocator propagation
         * @param other Stack instance to move from
         * @note The allocator is moved if ALLOC::propagate_on_container_move_assignment is true
         * @note noexcept guarantees exception safety during move
         */
        stack(stack&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Stack instance to move from
         * @return Reference to this stack
         * @note noexcept guarantees no exceptions during move
         * @note The allocator is moved if ALLOC::propagate_on_container_move_assignment is true
         */
        stack& operator=(stack&& other) noexcept;

        /**
         * @brief Pushes element to the top of the stack
         * @param e Element to be pushed
         * @details Uses the stack's allocator to construct the new element at the top.
         * @note Strong exception guarantee - if an exception is thrown, the stack remains unchanged
         * @complexity Depends on underlying container (typically O(1) amortized)
         */
        void push(const TYPE& e);

        /**
         * @brief Removes and returns top element from the stack
         * @return The element removed from top of stack
         * @pre Stack should not be empty
         * @details Uses the stack's allocator to destroy the removed element.
         * @throw original::noElementError if stack is empty
         * @note No-throw guarantee if TYPE's destructor doesn't throw
         * @complexity Depends on underlying container (typically O(1))
         */
        TYPE pop();

        /**
         * @brief Accesses the top element of the stack
         * @return Const reference to the top element
         * @pre Stack should not be empty
         * @details Provides O(1) access to the top element.
         * @throw original::noElementError if stack is empty
         * @complexity O(1)
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
