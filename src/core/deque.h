#ifndef DEQUE_H
#define DEQUE_H

/**
 * @file deque.h
 * @brief Double-ended queue (deque) container adapter implementation
 * @details Provides a deque adapter supporting insertion/removal at both ends.
 * Works with different underlying container types through template parameters.
 */

#include "chain.h"
#include "containerAdapter.h"

namespace original {

    /**
     * @class deque
     * @tparam TYPE Type of elements stored in the deque
     * @tparam SERIAL Underlying container type (default: chain)
     * @tparam ALLOC Allocator template for memory management (default: allocator)
     * @brief Double-ended queue container adapter
     * @extends original::containerAdapter
     * @details Implements deque operations using the specified underlying container.
     * Supports efficient insertion and removal at both front and back ends.
     * Inherits template constraints from @ref original::containerAdapter.
     *
     * The allocator is propagated to both the deque adapter and the underlying serial
     * container for consistent memory management.
     */
    template <typename TYPE,
              template <typename, typename> typename SERIAL = chain,
              template <typename> typename ALLOC = allocator>
    class deque final : public containerAdapter<TYPE, SERIAL, ALLOC> {
    public:

        /**
         * @brief Constructs deque with specified underlying container and allocator
         * @param serial Container instance to initialize deque (default: empty)
         * @details The allocator from the provided container will be used for all memory
         * operations. If no container is provided, a default-constructed container with
         * default allocator will be used.
         */
        explicit deque(const SERIAL<TYPE, ALLOC<TYPE>>& serial = SERIAL<TYPE, ALLOC<TYPE>>{});

        /**
         * @brief Constructs deque from initializer list with allocator
         * @param lst List of elements for initial content
         * @details Uses the default allocator to construct the underlying container
         * and populate it with elements from the initializer list.
         */
        deque(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Copy constructs a deque with allocator propagation
         * @param other Deque instance to copy from
         * @note The allocator is copied if ALLOC::propagate_on_container_copy_assignment is true
         */
        deque(const deque& other);

        /**
         * @brief Copy assignment operator
         * @param other Deque instance to copy from
         * @return Reference to this deque
         */
        deque& operator=(const deque& other);

        /**
         * @brief Move constructs a deque with allocator propagation
         * @param other Deque instance to move from
         * @note The allocator is moved if ALLOC::propagate_on_container_move_assignment is true
         * @note noexcept guarantees exception safety during move
         */
        deque(deque&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Deque instance to move from
         * @return Reference to this deque
         * @note noexcept guarantees exception safety during move
         */
        deque& operator=(deque&& other) noexcept;

        /**
         * @brief Inserts element at the front
         * @param e Element to insert
         * @details Uses the deque's allocator to construct the new element at the front
         */
        void pushBegin(const TYPE &e);

        /**
         * @brief Inserts element at the back
         * @param e Element to insert
         * @details Uses the deque's allocator to construct the new element at the back
         */
        void pushEnd(const TYPE &e);

        /**
         * @brief Removes and returns front element
         * @return The removed front element
         * @pre Deque must not be empty
         * @note Uses the deque's allocator to destroy the removed element
         */
        TYPE popBegin();

        /**
         * @brief Removes and returns back element
         * @return The removed back element
         * @pre Deque must not be empty
         * @note Uses the deque's allocator to destroy the removed element
         */
        TYPE popEnd();

        /**
         * @brief Accesses front element
         * @return Const reference to front element
         * @pre Deque must not be empty
         */
        TYPE head() const;

        /**
         * @brief Accesses back element
         * @return Const reference to back element
         * @pre Deque must not be empty
         */
        TYPE tail() const;

        /**
         * @brief Gets class name identifier
         * @return "deque" string identifier
         */
        [[nodiscard]] std::string className() const override;
    };
}

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    original::deque<TYPE, SERIAL, ALLOC>::deque(const SERIAL<TYPE, ALLOC<TYPE>>& serial)
        : containerAdapter<TYPE, SERIAL, ALLOC>(serial) {}

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    original::deque<TYPE, SERIAL, ALLOC>::deque(const std::initializer_list<TYPE> &lst)
        : deque(SERIAL<TYPE, ALLOC<TYPE>>(lst)) {}

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    original::deque<TYPE, SERIAL, ALLOC>::deque(const deque& other) : deque() {
        this->operator=(other);
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::operator=(const deque& other) -> deque& {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        return *this;
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    original::deque<TYPE, SERIAL, ALLOC>::deque(deque&& other) noexcept : deque()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::operator=(deque&& other) noexcept -> deque&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        return *this;
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::pushBegin(const TYPE &e) -> void {
        this->serial_.pushBegin(e);
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::pushEnd(const TYPE &e) -> void {
        this->serial_.pushEnd(e);
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::popBegin() -> TYPE {
        return this->serial_.popBegin();
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::popEnd() -> TYPE {
        return this->serial_.popEnd();
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::head() const -> TYPE {
        return this->serial_.getBegin();
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::tail() const -> TYPE {
        return this->serial_.getEnd();
    }

    template <typename TYPE,
              template <typename, typename> typename SERIAL,
              template <typename> typename ALLOC>
    auto original::deque<TYPE, SERIAL, ALLOC>::className() const -> std::string {
        return "deque";
    }

#endif //DEQUE_H
