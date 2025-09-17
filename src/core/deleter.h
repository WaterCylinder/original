#ifndef DELETER_H
#define DELETER_H

/**
* @file deleter.h
* @brief Default deleters for resource management
* @details Provides template classes for safely deleting single objects and arrays.
* Designed to be used with smart pointer implementations for automatic resource cleanup.
*
* Key Features:
* - Type-safe deletion policies for single objects and arrays
* - CRTP (Curiously Recurring Template Pattern) design for extensibility
* - Exception-safe operations with noexcept guarantees
* - Support for custom deletion policies through inheritance
*/

namespace original {
    /**
      * @class deleterBase
      * @tparam TYPE Type of the resource.
      * @tparam DERIVED CRTP Derived deleter type.
      * @brief Base class for deleters defining the deletion policy interface.
      * @details This class serves as the base for all specific deleters and provides
      * a pure virtual function `operator()` to handle resource deletion.
      * Since this is an abstract class, it cannot be instantiated directly.
      *
      * @note Inherit from this class to create custom deletion policies.
      */
    template<typename TYPE, template<typename> class DERIVED>
    class deleterBase {
    public:
        /**
         * @brief Rebound deleter type for type transformation scenarios
         * @tparam T New type for the rebound deleter
         */
        template<typename T>
        using rebound_deleter = DERIVED<T>;

        /**
         * @brief Default constructor.
         * @details The constexpr specifier allows compile-time constant initialization.
         */
        constexpr deleterBase() noexcept = default;

        /**
         * @brief Pure virtual destructor.
         * @details Must be implemented by derived classes to ensure proper cleanup.
         */
        virtual ~deleterBase() = 0;

        /**
         * @brief Deletion operator.
         * @param ptr Pointer to the object to delete.
         * @details This method must be implemented by derived classes to free the resource pointed to by `ptr`.
         * @note The operation is noexcept to ensure exception safety in resource management.
         */
        virtual void operator()(const TYPE* ptr) const noexcept = 0;
    };

    /**
     * @class deleterBase
     * @tparam TYPE[] Element type of the array.
     * @tparam DERIVED CRTP Derived deleter type.
     * @brief Base class for array deleters.
     * @details A specialization of `deleterBase` designed specifically for deleting arrays.
     * Provides the interface for array-specific deletion policies.
     */
    template<typename TYPE, template<typename> class DERIVED>
    class deleterBase<TYPE[], DERIVED> {
    public:
        /**
         * @brief Rebound deleter type for type transformation scenarios
         * @tparam T New type for the rebound deleter
         */
        template<typename T>
        using rebound_deleter = DERIVED<T>;

        /**
         * @brief Default constructor.
         */
        constexpr deleterBase() noexcept = default;

        /**
         * @brief Pure virtual destructor.
         * @details Must be implemented by derived classes to ensure proper array cleanup.
         */
        virtual ~deleterBase() = 0;

        /**
         * @brief Deletion operator for arrays.
         * @param ptr Pointer to the array to delete.
         * @details This method must be implemented by derived classes to free the array memory using `delete[]`.
         * @note The operation is noexcept to ensure exception safety in resource management.
         */
        virtual void operator()(const TYPE* ptr) const noexcept = 0;
    };

    /**
    * @class deleter
    * @tparam TYPE Type of resource to manage
    * @brief Default deletion policy for single objects
    * @details Invokes standard delete operator on the managed pointer.
    * Suitable for use with non-array types allocated with new.
    *
    * @extends deleterBase<TYPE, deleter>
    * @note This is the default deleter for single objects in smart pointer implementations.
    */
    template<typename TYPE>
    class deleter final : public deleterBase<TYPE, deleter>{
    public:
        /**
         * @brief Default constructor
         */
        constexpr deleter() noexcept = default;

        /**
         * @brief Destructor
         */
        ~deleter() override = default;

        /**
        * @brief Deletes a single object
        * @param ptr Pointer to the object to delete
        * @note Uses standard delete operator. Do not use with array types.
        * @throws Nothing - operation is noexcept
        */
        void operator()(const TYPE* ptr) const noexcept override;
    };

    /**
    * @class deleter
    * @tparam TYPE[] Element type of array
    * @brief Specialization for array deletion
    * @details Invokes array delete[] operator for proper array cleanup.
    * Must be used with array types allocated with new[].
    *
    * @extends deleterBase<TYPE[], deleter>
    * @note This is the default deleter for array objects in smart pointer implementations.
    */
    template<typename TYPE>
    class deleter<TYPE[]> final : public deleterBase<TYPE[], deleter>{
    public:
        /**
         * @brief Default constructor
         */
        constexpr deleter() noexcept = default;

        /**
         * @brief Destructor
         */
        ~deleter() override = default;

        /**
        * @brief Deletes an array
        * @param ptr Pointer to the array to delete
        * @note Uses delete[] operator. Required for array type cleanup.
        */
        void operator()(const TYPE* ptr) const noexcept override;
    };
}

template<typename TYPE, template<typename> class DERIVED>
original::deleterBase<TYPE, DERIVED>::~deleterBase() = default;

template<typename TYPE, template<typename> class DERIVED>
original::deleterBase<TYPE[], DERIVED>::~deleterBase() = default;

template<typename TYPE>
void original::deleter<TYPE>::operator()(const TYPE* ptr) const noexcept {
    delete ptr;
}

template<typename TYPE>
void original::deleter<TYPE[]>::operator()(const TYPE* ptr) const noexcept {
    delete[] ptr;
}

#endif //DELETER_H
