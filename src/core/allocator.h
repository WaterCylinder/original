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
        u_integer size_index_max;
        u_integer* chunk_count;
        freeChunk** free_list_head;
        allocatedChunks* allocated_list_head;
        u_integer* chunks_available;

        [[nodiscard]] static constexpr u_integer getChunkIndex(u_integer size);

        void deallocate(TYPE* ptr);

        void chunkAllocate(u_integer num_element, u_integer index);

        void reset() noexcept;

        public:
            using typename allocatorBase<TYPE, objPoolAllocator>::propagate_on_container_copy_assignment;
            using propagate_on_container_move_assignment = std::true_type;

        explicit objPoolAllocator(u_integer index_max = 8, u_integer count = 4);

        objPoolAllocator(const objPoolAllocator&) = delete;

        objPoolAllocator& operator=(const objPoolAllocator&) = delete;

        objPoolAllocator(objPoolAllocator&& other) noexcept;

        objPoolAllocator& operator=(objPoolAllocator&& other) noexcept;

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

template<typename TYPE>
constexpr original::u_integer original::objPoolAllocator<TYPE>::getChunkIndex(const u_integer size) {
    u_integer index = 0;
    while (static_cast<u_integer>(1) << index < size) {
        index += 1;
    }
    return index;
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
void original::objPoolAllocator<TYPE>::chunkAllocate(const u_integer num_element, u_integer index)
{
    const u_integer block_size = (1 << index) * CHUNK_SIZE;
    char* new_free_chunk = nullptr;
    allocatedChunks* new_allocated_chunk = nullptr;

    try
    {
        new_free_chunk = static_cast<char*>(operator new(num_element * block_size));
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
        auto cur_ptr = reinterpret_cast<freeChunk*>(new_free_chunk + i * block_size);
        cur_ptr->next = this->free_list_head[index];
        this->free_list_head[index] = cur_ptr;
    }
    this->chunks_available[index] += num_element;
    this->chunk_count[index] = this->chunk_count[index] + (this->chunk_count[index] >> 1);
}

template<typename TYPE>
void original::objPoolAllocator<TYPE>::reset() noexcept {
    while (this->allocated_list_head) {
        auto next_chunk = this->allocated_list_head->next;
        ::operator delete(this->allocated_list_head->chunks);
        ::operator delete(this->allocated_list_head);
        this->allocated_list_head = next_chunk;
    }

    if (this->chunk_count) {
        delete[] this->chunk_count;
        this->chunk_count = nullptr;
    }
    if (this->free_list_head) {
        delete[] this->free_list_head;
        this->free_list_head = nullptr;
    }
    if (this->chunks_available) {
        delete[] this->chunks_available;
        this->chunks_available = nullptr;
    }
}

template <typename TYPE>
original::objPoolAllocator<TYPE>::objPoolAllocator(const u_integer index_max,const u_integer count)
    : size_index_max(index_max), chunk_count(new u_integer[this->size_index_max]),
      free_list_head(new freeChunk*[this->size_index_max]), allocated_list_head(nullptr),
      chunks_available(new u_integer[this->size_index_max]) {
    for (u_integer i = 0; i < this->size_index_max; i++) {
        this->chunk_count[i] = count;
        this->free_list_head[i] = nullptr;
        this->chunks_available[i] = 0;
    }
}

template<typename TYPE>
void original::objPoolAllocator<TYPE>::deallocate(TYPE* ptr) {
    auto p = reinterpret_cast<freeChunk*>(ptr);
    p->next = this->free_list_head;
    this->free_list_head = p;
    this->chunks_available += 1;
}

template<typename TYPE>
original::objPoolAllocator<TYPE>::objPoolAllocator(objPoolAllocator&& other)  noexcept : objPoolAllocator() {
    this->operator=(std::move(other));
}

template<typename TYPE>
original::objPoolAllocator<TYPE>& original::objPoolAllocator<TYPE>::operator=(objPoolAllocator&& other) noexcept {
    if (this == &other)
        return *this;

    this->reset();

    this->size_index_max = other.size_index_max;
    this->chunk_count = other.chunk_count;
    this->free_list_head = other.free_list_head;
    this->allocated_list_head = other.allocated_list_head;
    this->chunks_available = other.chunks_available;

    other.chunk_count = nullptr;
    other.free_list_head = nullptr;
    other.allocated_list_head = nullptr;
    other.chunks_available = nullptr;
    return *this;
}

template <typename TYPE>
TYPE* original::objPoolAllocator<TYPE>::allocate(const u_integer size)
{
    if (size == 0) {
        return nullptr;
    }

    u_integer index = getChunkIndex(size);

    if (index >= size_index_max) {
        try {
            return static_cast<TYPE*>(operator new(size * sizeof(TYPE)));
        } catch (const std::bad_alloc&) {
            throw allocateError();
        }
    }

    if (!this->free_list_head[index] || this->chunks_available[index] * (static_cast<u_integer>(1) << index) < size) {
        this->chunkAllocate(max<u_integer>(1, this->chunk_count[index]), index);
    }

    auto cur_ptr = this->free_list_head[index];
    this->free_list_head[index] = this->free_list_head[index]->next;
    this->chunks_available[index] -= 1;
    return reinterpret_cast<TYPE*>(cur_ptr);
}

template <typename TYPE>
void original::objPoolAllocator<TYPE>::deallocate(TYPE* ptr, const u_integer size)
{
    if (size == 0) {
        return;
    }

    u_integer index = getChunkIndex(size);

    if (index >= size_index_max) {
        ::operator delete(ptr);
        return;
    }

    auto p = reinterpret_cast<freeChunk*>(ptr);
    p->next = this->free_list_head[index];
    this->free_list_head[index] = p;
    this->chunks_available[index] += 1;
}

template<typename TYPE>
original::objPoolAllocator<TYPE>::~objPoolAllocator() {
    this->reset();
}

#endif //ALLOCATOR_H
