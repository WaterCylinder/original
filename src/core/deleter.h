#ifndef DELETER_H
#define DELETER_H

/**
* @file deleter.h
* @brief Default deleters for resource management
* @details Provides template classes for safely deleting single objects and arrays.
* Designed to be used with smart pointer implementations for automatic resource cleanup.
*/

namespace original {
    /**
    * @class deleter
    * @tparam TYPE Type of resource to manage
    * @brief Default deletion policy for single objects
    * @details Invokes standard delete operator on the managed pointer.
    * Suitable for use with non-array types allocated with new.
    */
    template<typename TYPE>
    class deleter{
    public:
        constexpr deleter() noexcept = default;
        virtual ~deleter() = default;

        /**
        * @brief Deletes a single object
        * @param ptr Pointer to the object to delete
        * @note Uses standard delete operator. Do not use with array types.
        */
        virtual void operator()(const TYPE* ptr) const noexcept;
    };

    /**
    * @class deleter
    * @tparam TYPE[] Element type of array
    * @brief Specialization for array deletion
    * @details Invokes array delete[] operator for proper array cleanup.
    * Must be used with array types allocated with new[].
    */
    template<typename TYPE>
    class deleter<TYPE[]>{
    public:
        constexpr deleter() noexcept = default;
        virtual ~deleter() = default;

        /**
        * @brief Deletes an array
        * @param ptr Pointer to the array to delete
        * @note Uses delete[] operator. Required for array type cleanup.
        */
        virtual void operator()(const TYPE* ptr) const noexcept;
    };
}

template<typename TYPE>
void original::deleter<TYPE>::operator()(const TYPE* ptr) const noexcept {
    delete ptr;
}

template<typename TYPE>
void original::deleter<TYPE[]>::operator()(const TYPE* ptr) const noexcept {
    delete[] ptr;
}

#endif //DELETER_H
