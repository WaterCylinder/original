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
     * @tparam SERIAL Underlying container type (default: chain), see constraints from @ref original::containerAdapter
     * @brief First-In-First-Out (FIFO) container adapter
     * @extends original::containerAdapter<TYPE, SERIAL>
     * @details Implements queue operations using the specified underlying container.
     * Supports insertion at back and removal from front. Inherits template constraints
     * from @ref original::containerAdapter.
     */
    template<typename TYPE, template <typename> typename SERIAL = chain>
    class queue final : public containerAdapter<TYPE, SERIAL> {
    public:
        /**
         * @brief Constructs queue with specified underlying container
         * @param serial Container instance to initialize queue (default: empty)
         */
        explicit queue(const SERIAL<TYPE>& serial = SERIAL<TYPE>{});

        /**
         * @brief Constructs queue from initializer list
         * @param lst List of elements for initial content
         */
        queue(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Copy constructor
         * @param other Queue instance to copy from
         */
        queue(const queue& other);

        /**
         * @brief Copy assignment operator
         * @param other Queue instance to copy from
         * @return Reference to this queue
         */
        queue& operator=(const queue& other);

        /**
         * @brief Move constructor
         * @param other Queue instance to move from
         * @note noexcept guarantees exception safety during move
         */
        queue(queue&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Queue instance to move from
         * @return Reference to this queue
         * @note noexcept guarantees exception safety during move
         */
        queue& operator=(queue&& other) noexcept;

        /**
         * @brief Equality comparison operator
         * @param other Queue to compare with
         * @return True if both queues have equal elements in same order
         */
        bool operator==(const queue& other) const;

        /**
         * @brief Inserts element at the back of the queue
         * @param e Element to be inserted
         */
        void push(const TYPE& e);

        /**
         * @brief Removes and returns front element from the queue
         * @return The element removed from front of queue
         * @pre Queue must not be empty
         */
        TYPE pop();

        /**
         * @brief Accesses front element of the queue
         * @return Const reference to front element
         * @pre Queue must not be empty
         */
        TYPE head() const;

        /**
         * @brief Accesses back element of the queue
         * @return Const reference to back element
         * @pre Queue must not be empty
         */
        TYPE tail() const;

        /**
         * @brief Gets class name identifier
         * @return "queue" string identifier
         */
        [[nodiscard]] std::string className() const override;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const SERIAL<TYPE>& serial)
        : containerAdapter<TYPE, SERIAL>(serial) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const std::initializer_list<TYPE> &lst)
        : queue(SERIAL<TYPE>(lst)) {}

    template<typename TYPE, template <typename> typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const queue& other)
        : containerAdapter<TYPE, SERIAL>(other.serial_) {}

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::operator=(const queue& other) -> queue& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::operator==(const queue& other) const -> bool {
        return this->serial_ == other.serial_;
    }

    template <typename TYPE, template <typename> class SERIAL>
    original::queue<TYPE, SERIAL>::queue(queue&& other) noexcept : queue()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class SERIAL>
    auto original::queue<TYPE, SERIAL>::operator=(queue&& other) noexcept -> queue&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        other.serial_ = SERIAL<TYPE>{};
        return *this;
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::push(const TYPE& e) -> void {
        this->serial_.pushEnd(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::pop() -> TYPE {
        return this->serial_.popBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::head() const -> TYPE {
        return this->serial_.getBegin();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    auto original::queue<TYPE, SERIAL>::tail() const -> TYPE {
        return this->serial_.getEnd();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    std::string original::queue<TYPE, SERIAL>::className() const {
        return "queue";
    }

#endif //QUEUE_H
