#ifndef PRIQUE_H
#define PRIQUE_H

/**
 * @file prique.h
 * @brief Priority queue container implementation
 * @details Provides a heap-based priority queue with configurable comparison logic
 * and underlying storage. Supports efficient insertion and extraction of highest
 * priority elements according to the specified comparator.
 */

#include "algorithms.h"
#include "blocksList.h"
#include "comparator.h"
#include "containerAdapter.h"
#include "types.h"

namespace original
{
    /**
     * @class prique
     * @tparam TYPE Type of elements stored in the priority queue
     * @tparam Callback Comparison functor type (default: increaseComparator)
     * @tparam SERIAL Underlying container type (default: blocksList)
     * @brief Heap-based priority queue container
     * @extends containerAdapter<TYPE, SERIAL>
     * @details Implements a priority queue using heap algorithms over an underlying
     * container. The element priority is determined by the provided comparator.
     *
     * @note Template parameters are constrained by:
     * @code
     * requires Compare<Callback<TYPE>, TYPE>
     * @endcode
     * Ensures the Callback type provides valid comparison operations for TYPE.
     *
     * @see @ref original::containerAdapter for SERIAL container requirements
     */
    template<typename TYPE,
            template <typename> typename Callback = increaseComparator,
            template <typename> typename SERIAL = blocksList>
    requires Compare<Callback<TYPE>, TYPE>
    class prique final : public containerAdapter<TYPE, SERIAL>
    {
        Callback<TYPE> compare_; ///< Comparison functor instance for priority ordering

    public:
        /**
         * @brief Constructs priority queue with container and comparator
         * @param serial Underlying container instance (default: empty)
         * @param compare Comparison functor instance (default: default-constructed)
         * @details Initializes heap structure using algorithms::heapInit
         */
        explicit prique(const SERIAL<TYPE>& serial = SERIAL<TYPE>{},
                        const Callback<TYPE>& compare = Callback<TYPE>{});

        /**
         * @brief Constructs from initializer list with comparator
         * @param lst Elements to initialize the queue
         * @param compare Comparison functor instance
         */
        prique(const std::initializer_list<TYPE>& lst,
               const Callback<TYPE>& compare = Callback<TYPE>{});

        /**
         * @brief Copy constructor
         * @param other Priority queue to copy from
         */
        prique(const prique& other);

        /**
         * @brief Copy assignment operator
         * @param other Priority queue to copy from
         * @return Reference to this queue
         */
        prique& operator=(const prique& other);

        /**
         * @brief Equality comparison operator
         * @param other Queue to compare with
         * @return True if both queues have identical elements and ordering
         */
        bool operator==(const prique& other) const;

        /**
         * @brief Move constructor
         * @param other Queue to move from
         * @note noexcept guarantees exception safety during move
         */
        prique(prique&& other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Queue to move from
         * @return Reference to this queue
         * @note noexcept guarantees exception safety during move
         */
        prique& operator=(prique&& other) noexcept;

        /**
         * @brief Inserts element maintaining heap property
         * @param e Element to insert
         * @details Uses algorithms::heapAdjustUp after insertion
         */
        void push(const TYPE& e);

        /**
         * @brief Extracts highest priority element
         * @return The extracted element
         * @throw original::noElementError if queue is empty
         * @details Uses algorithms::heapAdjustDown after extraction
         */
        TYPE pop();

        /**
         * @brief Accesses highest priority element
         * @return Const reference to top element
         * @pre Queue must not be empty
         */
        TYPE top() const;

        /**
         * @brief Gets class name identifier
         * @return "prique" string identifier
         */
        [[nodiscard]] std::string className() const override;
    };
}


template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    original::prique<TYPE, Callback, SERIAL>::prique(const SERIAL<TYPE>& serial, const Callback<TYPE>& compare)
        : containerAdapter<TYPE, SERIAL>(serial), compare_(compare)
    {
        algorithms::heapInit(this->serial_.begin(), this->serial_.last(), this->compare_);
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    original::prique<TYPE, Callback, SERIAL>::prique(const std::initializer_list<TYPE>& lst, const Callback<TYPE>& compare)
        : prique(SERIAL<TYPE>(lst), compare) {}

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    original::prique<TYPE, Callback, SERIAL>::prique(const prique& other)
        : containerAdapter<TYPE, SERIAL>(other.serial_), compare_(other.compare_) {}

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::operator=(const prique& other) -> prique&
    {
        if (this == &other) return *this;
        this->serial_ = other.serial_;
        compare_ = other.compare_;
        return *this;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::operator==(const prique& other) const -> bool
    {
        return this->serial_ == other.serial_;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    original::prique<TYPE, Callback, SERIAL>::prique(prique&& other) noexcept : prique()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::operator=(prique&& other) noexcept -> prique&
    {
        if (this == &other)
            return *this;

        this->serial_ = std::move(other.serial_);
        this->compare_ = std::move(other.compare_);
        other.serial_ = SERIAL<TYPE>{};
        other.compare_ = Callback<TYPE>{};
        return *this;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::push(const TYPE& e) -> void
    {
        this->serial_.pushEnd(e);
        algorithms::heapAdjustUp(this->serial_.begin(), this->serial_.last(), this->compare_);
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::pop() -> TYPE
    {
        if (this->empty()) throw noElementError();

        algorithms::swap(this->serial_.begin(), this->serial_.last());
        TYPE res = this->serial_.popEnd();
        algorithms::heapAdjustDown(this->serial_.begin(), this->serial_.last(), this->serial_.begin(), compare_);
        return res;
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::top() const -> TYPE
    {
        return this->serial_.getBegin();
    }

    template <typename TYPE, template <typename> class Callback, template <typename> typename SERIAL>
    requires original::Compare<Callback<TYPE>, TYPE>
    auto original::prique<TYPE, Callback, SERIAL>::className() const -> std::string
    {
        return "prique";
    }

#endif //PRIQUE_H
