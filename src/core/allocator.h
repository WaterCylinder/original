#ifndef ALLOCATOR_H
#define ALLOCATOR_H


#include <iostream>

#include "error.h"
#include "maths.h"
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

        /**
        * @brief Constructs a new allocatorBase instance
        * @details Performs compile-time validation of the allocated type:
        * - Ensures TYPE is not void
        * - Ensures TYPE is a complete type (size > 0)
        *
        * @code{.cpp}
        * allocatorBase<int, allocator> a;  // Valid
        * allocatorBase<void, allocator> b; // Compile-time error
        * @endcode
        */
        constexpr allocatorBase();

        /**
        * @brief Allocates raw memory
        * @param size Number of elements to allocate
        * @return Pointer to allocated memory
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
        * @throw allocateError When memory allocation fails
        * @note Returns nullptr if size is 0
        */
        TYPE* allocate(u_integer size) override;

        /**
        * @brief Deallocates memory using global operator delete
        * @param ptr Pointer to memory to free
        * @param size Number of elements (unused in this implementation)
        */
        void deallocate(TYPE* ptr, u_integer size) override;
    };

    template<typename TYPE>
    class objPoolAllocator final : public allocatorBase<TYPE, objPoolAllocator>
    {
        class freeChunk
        {
        public:
            freeChunk* next = nullptr;
        };

        class allocatedChunks {
        public:
            void* chunks = nullptr;
            allocatedChunks* next = nullptr;
        };

        static constexpr u_integer CHUNK_SIZE =  sizeof(TYPE) > sizeof(freeChunk*) ? sizeof(TYPE) : sizeof(freeChunk*);
        u_integer chunk_count;
        freeChunk* free_list_head;
        allocatedChunks* allocated_list_head;
        u_integer chunks_available;

        TYPE* allocate();

        void deallocate(TYPE* ptr);

        void chunkAllocate(u_integer num_element);

        public:
            using typename allocatorBase<TYPE, objPoolAllocator>::propagate_on_container_copy_assignment;
            using typename allocatorBase<TYPE, objPoolAllocator>::propagate_on_container_move_assignment;

        explicit objPoolAllocator(u_integer count = 4);

        TYPE* allocate(u_integer size) override;

        void deallocate(TYPE* ptr, u_integer size) override;

        ~objPoolAllocator() override;
    };
}

template <typename TYPE, template <typename> class DERIVED>
constexpr original::allocatorBase<TYPE, DERIVED>::allocatorBase()
{
    staticError<allocateError, sizeof(TYPE) == 0 || std::is_void_v<TYPE>>{};
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
    if (size == 0) {
        return nullptr;
    }

    try {
        return static_cast<TYPE*>(operator new(size * sizeof(TYPE)));
    } catch (const std::bad_alloc&) {
        throw allocateError();
    }
}

template<typename TYPE>
void original::allocator<TYPE>::deallocate(TYPE *ptr, u_integer) {
    ::operator delete(ptr);
}

template <typename TYPE>
void original::objPoolAllocator<TYPE>::chunkAllocate(const u_integer num_element)
{
    char* new_free_chunk = nullptr;
    allocatedChunks* new_allocated_chunk = nullptr;

    try
    {
        new_free_chunk = static_cast<char*>(operator new(num_element * CHUNK_SIZE));
        new_allocated_chunk = static_cast<allocatedChunks*>(operator new(sizeof(allocatedChunks)));
    }
    catch (const std::bad_alloc&)
    {
        throw allocateError();
    }

    new_allocated_chunk->chunks = new_free_chunk;
    new_allocated_chunk->next = this->allocated_list_head;
    this->allocated_list_head = new_allocated_chunk;

    for (u_integer i = 0; i < num_element; i++)
    {
        auto cur_ptr = reinterpret_cast<freeChunk*>(new_free_chunk + i * CHUNK_SIZE);
        cur_ptr->next = this->free_list_head;
        this->free_list_head = cur_ptr;
    }
    this->chunks_available += num_element;
    this->chunk_count = this->chunk_count + (this->chunk_count >> 1);
}

template <typename TYPE>
original::objPoolAllocator<TYPE>::objPoolAllocator(const u_integer count)
    : chunk_count(count), free_list_head(nullptr), allocated_list_head(nullptr), chunks_available(0) {}

template<typename TYPE>
TYPE* original::objPoolAllocator<TYPE>::allocate() {
    if (!this->free_list_head)
        this->chunkAllocate(max(static_cast<u_integer>(1), this->chunk_count));
    auto cur_ptr = this->free_list_head;
    this->free_list_head = this->free_list_head->next;
    cur_ptr->next = nullptr;
    this->chunks_available -= 1;
    return reinterpret_cast<TYPE*>(cur_ptr);
}

template<typename TYPE>
void original::objPoolAllocator<TYPE>::deallocate(TYPE* ptr) {
    auto p = reinterpret_cast<freeChunk*>(ptr);
    p->next = this->free_list_head;
    this->free_list_head = p;
    this->chunks_available += 1;
}

template <typename TYPE>
TYPE* original::objPoolAllocator<TYPE>::allocate(const u_integer size)
{
    if (size == 0) {
        return nullptr;
    }

    if (size == 1) {
        return this->allocate();
    }

    try {
        return static_cast<TYPE*>(operator new(size * sizeof(TYPE)));
    } catch (const std::bad_alloc&) {
        throw allocateError();
    }
}

template <typename TYPE>
void original::objPoolAllocator<TYPE>::deallocate(TYPE* ptr, const u_integer size)
{
    if (size == 0) {
        return;
    }
    if (size == 1) {
        this->deallocate(ptr);
        return;
    }

    ::operator delete(ptr);
}

template<typename TYPE>
original::objPoolAllocator<TYPE>::~objPoolAllocator() {
    while (this->allocated_list_head) {
        auto next_chunk = this->allocated_list_head->next;
        ::operator delete(this->allocated_list_head->chunks);
        ::operator delete(this->allocated_list_head);
        this->allocated_list_head = next_chunk;
    }
}

#endif //ALLOCATOR_H
