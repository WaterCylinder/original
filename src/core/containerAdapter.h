#ifndef CONTAINERADAPTER_H
#define CONTAINERADAPTER_H
#include "baseList.h"
#include "types.h"
#include "printable.h"
#include "comparable.h"


namespace original {
    /**
     * @file containerAdapter.h
     * @brief Base class for container adapters with common interfaces
     * @details Provides unified foundation for stack, queue, deque, priority_queue and other
     * container adapters that need to work with different underlying container types.
     * Implements core functionality required by standard container adapters.
     */

    /**
     * @class containerAdapter
     * @tparam TYPE The type of elements stored in the container
     * @tparam SERIAL The underlying serial container type
     * @brief Adapter class that provides unified interface for various container types
     * @extends printable
     * @extends container
     * @details This class template adapts different serial container types (like vectors, lists)
     * to provide consistent interfaces including size checking, element existence verification,
     * and formatted string output. Inherits both printable interface and container interface.
     *
     * @note The SERIAL template parameter is constrained by C++20 requires clause to ensure
     * it derives from baseList<TYPE>. This guarantees the availability of required container
     * interfaces like size(), clear(), and contains() methods.
     *
     * @tparam SERIAL Must satisfy:
     * @code
     * ExtendsOf<baseList<TYPE>, SERIAL<TYPE>>
     * @endcode
     * Which enforces SERIAL to be a subclass of baseList<TYPE>.
     */
    template <typename TYPE, template <typename> typename SERIAL>
    requires ExtendsOf<baseList<TYPE>, SERIAL<TYPE>>
    class containerAdapter
            : public printable,
              public container<TYPE>,
              public comparable<containerAdapter<TYPE, SERIAL>>{
    protected:
        SERIAL<TYPE> serial_; ///< Underlying container instance used for storage

        /**
         * @brief Protected constructor for derived class initialization
         * @param serial The underlying container instance to adapt
         * @details Only accessible to derived classes (stack/queue/etc adapters).
         * Enforces proper initialization through concrete adapter types.
         */
        explicit containerAdapter(const SERIAL<TYPE>& serial);

    public:
        /**
         * @brief Returns the number of elements in the adapter
         * @return uint32_t Current number of stored elements
         * @details Directly delegates to underlying container's size() method
         */
        [[nodiscard]] uint32_t size() const override;

        /**
         * @brief Removes all elements from the adapter
         * @details Clears content through underlying container's clear() method
         */
        void clear();

        /**
         * @brief Checks for element existence in the adapter
         * @param e Element to search for
         * @return bool True if element exists, false otherwise
         * @details Uses underlying container's contains() method for verification
         */
        bool contains(const TYPE &e) const override;

        int64_t compareTo(const containerAdapter<TYPE, SERIAL> &other) const override;

        /**
         * @brief Gets class name identifier for type information
         * @return std::string Fixed identifier "containerAdapter"
         * @note Derived classes should override this with specific names (e.g. "stack")
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Generates formatted string representation
         * @param enter Add newline at end when true
         * @return std::string Formatted elements in "containerAdapter(e1, e2,...)" format
         * @details Provides uniform formatting across all container adapters
         */
        [[nodiscard]] std::string toString(bool enter) const override;

        /// @brief Virtual destructor for proper polymorphic cleanup
        ~containerAdapter() override = default;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    original::containerAdapter<TYPE, SERIAL>::containerAdapter(const SERIAL<TYPE>& serial)
        : serial_(serial) {}

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::size() const -> uint32_t {
        return serial_.size();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, template <typename> class SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::contains(const TYPE &e) const -> bool {
        return serial_.contains(e);
    }

    template<typename TYPE, template <typename> class SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    int64_t original::containerAdapter<TYPE, SERIAL>::compareTo(const containerAdapter& other) const {
        return serial_.compareTo(other.serial_);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::className() const -> std::string {
        return "containerAdapter";
    }

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className() << "(";
        bool first = true;
        for (const auto e : this->serial_)
        {
            if (!first) ss << ", ";
            ss << printable::formatString(e);
            first = false;
        }
        ss << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //CONTAINERADAPTER_H
