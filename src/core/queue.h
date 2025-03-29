#ifndef QUEUE_H
#define QUEUE_H

/**
 * @file queue.h
 * @brief Queue container adapter implementation
 * @details Provides a FIFO (First-In-First-Out) container adapter that uses
 * underlying container types. Supports standard queue operations.
 */

#include "containerAdapter.h"
#include "chain.h"
#include <string>

namespace original {

    /**
     * @class queue
     * @tparam TYPE Type of elements stored in the queue
     * @tparam SERIAL Underlying container type (default: chain)
     * @tparam ALLOC Allocator type for memory management (default: allocator<TYPE>)
     * @brief First-In-First-Out (FIFO) container adapter
     * @extends original::containerAdapter
     * @details Implements queue operations using the specified underlying container.
     * Supports insertion at back and removal from front. Inherits template constraints
     * from @ref original::containerAdapter.
     *
     * The allocator is propagated to both the queue adapter and the underlying serial
     * container for consistent memory management. The ALLOC type must meet the
     * C++ allocator requirements.
     */
    template<typename TYPE,
             template <typename, typename> typename SERIAL = chain,
             template <typename> typename ALLOC = allocator>
    class queue final : public containerAdapter<TYPE, SERIAL, ALLOC> {
    public:
        /**
         * @brief Constructs queue with specified underlying container and allocator
         * @param serial Container instance to initialize queue (default: empty)
         * @details The allocator from the provided container will be used for all memory
         * operations. If no container is provided, a default-constructed container with
         * default allocator will be used.
         */
        explicit queue(const SERIAL<TYPE, ALLOC<TYPE>>& serial = SERIAL<TYPE, ALLOC<TYPE>>{});

        /**
         * @brief Constructs queue from initializer list with allocator
         * @param lst List of elements for initial content
         * @details Uses the default allocator to construct the underlying container
         * and populate it with elements from the initializer list.
         */
        queue(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Copy constructs a queue with allocator propagation
         * @param other Queue instance to copy from
         * @note The allocator is copied if ALLOC::propagate_on_container_copy_assignment is true
         */
        queue(const queue& other);

        /**
         * @brief Copy assignment operator
         * @param other Queue instance to copy from
         * @return Reference to this queue
         * @note The allocator is copied if ALLOC::propagate_on_container_copy_assignment is true
         */
        queue& operator=(const queue& other);

        /**
         * @brief Move constructs a queue with allocator propagation
         * @param other Queue instance to move from
         * @note The allocator is moved if ALLOC::propagate_on_container_move_assignment is true
         * @note noexcept guarantees exception safety during move
         */
        queue(queue&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Queue instance to move from
         * @return Reference to this queue
         * @note noexcept guarantees exception safety during move
         * @note The allocator is moved if ALLOC::propagate_on_container_move_assignment is true
         */
        queue& operator=(queue&& other) noexcept;

        /**
         * @brief Inserts element at the back of the queue
         * @param e Element to be inserted
         * @details Uses the queue's allocator to construct the new element at the back.
         * @note Strong exception guarantee - if an exception is thrown, the queue remains unchanged
         */
        void push(const TYPE& e);

        /**
         * @brief Removes and returns front element from the queue
         * @return The element removed from front of queue
         * @pre Queue must not be empty
         * @details Uses the queue's allocator to destroy the removed element.
         * @throw original::noElementError if queue is empty
         * @note No-throw guarantee if TYPE's destructor doesn't throw
         */
        TYPE pop();

        /**
         * @brief Accesses front element of the queue
         * @return Const reference to front element
         * @pre Queue must not be empty
         * @details Provides O(1) access to the front element.
         * @throw original::noElementError if queue is empty
         */
        TYPE head() const;

        /**
         * @brief Accesses back element of the queue
         * @return Const reference to back element
         * @pre Queue must not be empty
         * @details Provides O(1) access to the back element.
         * @throw original::noElementError if queue is empty
         */
        TYPE tail() const;

        /**
         * @brief Gets class name identifier
         * @return "queue" string identifier
         */
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::queue<TYPE, SERIAL, ALLOC>::queue(const SERIAL<TYPE, ALLOC<TYPE>>& serial)
        : containerAdapter<TYPE, SERIAL, ALLOC>(serial) {}

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::queue<TYPE, SERIAL, ALLOC>::queue(const std::initializer_list<TYPE> &lst)
        : queue(SERIAL<TYPE, ALLOC<TYPE>>(lst)) {}

    template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    original::queue<TYPE, SERIAL, ALLOC>::queue(const queue& other)
        : containerAdapter<TYPE, SERIAL, ALLOC>(other.serial_) {}

    template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    auto original::queue<TYPE, SERIAL, ALLOC>::operator=(const queue& other) -> queue& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    original::queue<TYPE, SERIAL, ALLOC>::queue(queue&& other) noexcept : queue()
    {
        this->operator=(std::move(other));
    }

    template<typename TYPE,
             template <typename, typename> typename SERIAL,
             template <typename> typename ALLOC>
    auto original::queue<TYPE, SERIAL, ALLOC>::operator=(queue&& other) noexcept -> queue&
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
    auto original::queue<TYPE, SERIAL, ALLOC>::push(const TYPE& e) -> void {
        this->serial_.pushEnd(e);
    }

    template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    auto original::queue<TYPE, SERIAL, ALLOC>::pop() -> TYPE {
        return this->serial_.popBegin();
    }

    template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    auto original::queue<TYPE, SERIAL, ALLOC>::head() const -> TYPE {
        return this->serial_.getBegin();
    }

    template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    auto original::queue<TYPE, SERIAL, ALLOC>::tail() const -> TYPE {
        return this->serial_.getEnd();
    }

    template<typename TYPE,
         template <typename, typename> typename SERIAL,
         template <typename> typename ALLOC>
    std::string original::queue<TYPE, SERIAL, ALLOC>::className() const {
        return "queue";
    }

#endif //QUEUE_H
