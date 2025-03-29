#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "type_traits"
#include "utility"

/**
* @file allocator.h
* @brief Memory allocation interface and default implementation
* @details Provides base allocator interface with CRTP support and a default
*          memory allocator implementation using global operator new/delete.
*/

namespace original {
    /**
    * @class allocatorBase
    * @tparam TYPE Type of objects to allocate
    * @tparam DERIVED CRTP template parameter for derived allocators
    * @brief Interface for other memory allocator implements
    * @details Provides common interface and utilities for memory management:
    * - Type-safe allocation/de-allocation
    * - Object construction/destruction
    * - Rebinding support for container needs
    * @note Derived classes must implement allocate/deallocate
    */
    template<typename TYPE, template <typename> typename DERIVED>
    class allocatorBase{
    public:
        using propagate_on_container_copy_assignment = std::false_type; ///< No propagation on copy
        using propagate_on_container_move_assignment = std::false_type; ///< No propagation on move

        /**
        * @brief Rebinds allocator to different type
        * @tparam O_TYPE New type to allocate
        */
        template <typename O_TYPE>
        using rebind_alloc = DERIVED<O_TYPE>;

        constexpr allocatorBase() = default; ///< Default constructor

        /**
        * @brief Allocates raw memory
        * @param size Number of elements to allocate
        * @return Pointer to allocated memory
        * @throws std::bad_alloc on allocation failure
        */
        virtual TYPE* allocate(u_integer size) = 0;

        /**
        * @brief Deallocates memory
        * @param ptr Pointer to memory to free
        * @param size Number of elements originally allocated
        */
        virtual void deallocate(TYPE* ptr, u_integer size) = 0;

        /**
        * @brief Constructs an object in allocated memory
        * @tparam O_TYPE Type of object to construct
        * @tparam Args Constructor argument types
        * @param o_ptr Pointer to allocated memory
        * @param args Arguments to forward to constructor
        */
        template<typename O_TYPE, typename... Args>
        void construct(O_TYPE* o_ptr, Args&&... args);

        /**
        * @brief Destroys an object without deallocating
        * @tparam O_TYPE Type of object to destroy
        * @param o_ptr Pointer to object to destroy
        */
        template<typename O_TYPE>
        static void destroy(O_TYPE* o_ptr);

        virtual ~allocatorBase() = 0; ///< Virtual destructor
    };

    /**
    * @class allocator
    * @tparam TYPE Type of objects to allocate
    * @brief Default memory allocator using global new/delete
    * @details Implements standard-compliant allocator using:
    * - ::operator new for allocation
    * - ::operator delete for de-allocation
    * - Placement new for construction
    * - Explicit destructor calls
    * @extends allocatorBase
    */
    template<typename TYPE>
    class allocator : public allocatorBase<TYPE, allocator> {
    public:
        using typename allocatorBase<TYPE, allocator>::propagate_on_container_copy_assignment;
        using typename allocatorBase<TYPE, allocator>::propagate_on_container_move_assignment;

        /**
        * @brief Allocates memory using global operator new
        * @param size Number of elements to allocate
        * @return Pointer to allocated memory
        */
        TYPE* allocate(u_integer size) override;

        /**
        * @brief Deallocates memory using global operator delete
        * @param ptr Pointer to memory to free
        * @param size Number of elements (unused in this implementation)
        */
        void deallocate(TYPE* ptr, u_integer size) override;
    };
}

template<typename TYPE, template <typename> typename DERIVED>
original::allocatorBase<TYPE, DERIVED>::~allocatorBase() = default;

template<typename TYPE, template <typename> typename DERIVED>
template<typename O_TYPE, typename... Args>
void original::allocatorBase<TYPE, DERIVED>::construct(O_TYPE *o_ptr, Args &&... args) {
    new (o_ptr) O_TYPE{std::forward<Args>(args)...};
}

template<typename TYPE, template <typename> typename DERIVED>
template<typename O_TYPE>
void original::allocatorBase<TYPE, DERIVED>::destroy(O_TYPE *o_ptr) {
    o_ptr->~O_TYPE();
}

template<typename TYPE>
TYPE* original::allocator<TYPE>::allocate(const u_integer size) {
    return static_cast<TYPE*>(operator new(size * sizeof(TYPE)));
}

template<typename TYPE>
void original::allocator<TYPE>::deallocate(TYPE *ptr, u_integer) {
    ::operator delete(ptr);
}

#endif //ALLOCATOR_H
