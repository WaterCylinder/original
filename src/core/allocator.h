#ifndef ALLOCATOR_H
#define ALLOCATOR_H


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
    * @class allocators
    * @brief Utility class providing static memory allocation/de-allocation functions
    * @details Contains static methods for raw memory operations that serve as the foundation
    *          for other allocator implementations in this namespace.
    */
    class allocators final {
        public:
        /**
        * @brief Allocates raw memory using global operator new
        * @tparam TYPE Type of objects to allocate
        * @param size Number of elements to allocate
        * @return Pointer to allocated memory
        * @throw allocateError When memory allocation fails
        * @note Returns nullptr if size is 0
        * @see free
        */
        template <typename TYPE>
        static TYPE* malloc(u_integer size);

        /**
        * @brief Deallocates memory using global operator delete
        * @tparam TYPE Type of objects being freed (auto deduced)
        * @param ptr Pointer to memory to free
        * @see malloc
        */
        template <typename TYPE>
        static void free(TYPE* ptr);
    };

    /**
    * @class allocatorBase
    * @tparam TYPE The type of objects to allocate
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
    * @brief Default memory allocator using allocators utilities
    * @details Implements standard-compliant allocator using:
    * - allocators::malloc for allocation
    * - allocators::free for de-allocation
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

    /**
    * @class objPoolAllocator
    * @tparam TYPE Type of objects to allocate
    * @brief Object pool allocator for efficient fixed-size memory management
    * @details Implements a memory allocator using an object pool pattern with these characteristics:
    * - Organizes memory into power-of-two sized chunks (1, 2, 4, 8, 16, 32, ... bytes)
    * - Maintains separate free lists for each chunk size
    * - Allocates memory in blocks (chunks) to reduce fragmentation
    * - Falls back to allocators::malloc/allocators::free for allocations too large for the pool
    *
    * Memory Management Approach:
    * 1. The allocator maintains multiple free lists, one for each size class (power-of-two)
    * 2. When an allocation request comes in:
    *    - Finds the smallest size class that can satisfy the request
    *    - If no free chunks available, allocates a new block of chunks
    *    - For requests too large for the largest size class, uses global new
    * 3. When memory is deallocated:
    *    - Returns the chunk to its appropriate free list
    *    - Large allocations (outside pool) are freed immediately
    *
    * @note For large allocations, uses allocators::malloc/allocators::free
    * @extends allocatorBase
    * @see allocator For the default allocator implementation
    */
    template<typename TYPE>
    class objPoolAllocator final : public allocatorBase<TYPE, objPoolAllocator>
    {
        /**
        * @class freeChunk
        * @brief Internal structure representing a free memory chunk
        * @details Used to maintain the free list as a linked list
        */
        class freeChunk
        {
        public:
            freeChunk* next = nullptr; ///< Pointer to next free chunk
        };

        /**
        * @class allocatedChunks
        * @brief Tracks blocks of allocated memory for later de-allocation
        */
        class allocatedChunks {
        public:
            void* chunks = nullptr;       ///< Pointer to allocated memory block
            allocatedChunks* next = nullptr; ///< Next block in the list
        };

        /**
        * @brief The fundamental chunk size used for memory management
        * @details Determines the minimum allocation unit size in the pool.
        * The actual size is calculated as the maximum between:
        * - sizeof(TYPE): To ensure each chunk can hold at least one object of the templated type
        * - sizeof(freeChunk*): To ensure proper free list maintenance (must be able to store a pointer)
        */
        static constexpr u_integer CHUNK_SIZE =  sizeof(TYPE) > sizeof(freeChunk*) ? sizeof(TYPE) : sizeof(freeChunk*);

        u_integer size_index_max;         ///< Maximum size class index (2^index)
        u_integer* chunk_count;          ///< Array of chunk counts per size class
        freeChunk** free_list_head;      ///< Array of free list heads (one per size class)
        allocatedChunks* allocated_list_head; ///< List of all allocated blocks
        u_integer* chunks_available;     ///< Array of available chunks per size class

        /**
        * @brief Calculates the appropriate size class index for a request
        * @param size Requested allocation size
        * @return Index of the smallest size class that can satisfy the request
        */
        [[nodiscard]] static constexpr u_integer getChunkIndex(u_integer size);

        /**
        * @brief Allocates a new block of chunks for a size class
        * @param num_element Number of chunks to allocate
        * @param index Size class index
        * @throw allocateError If memory allocation fails
        */
        void chunkAllocate(u_integer num_element, u_integer index);

        /**
        * @brief Releases all allocated memory using allocators::free
        * @note Called during destruction and move assignment
        */
        void reset() noexcept;

    public:
        using typename allocatorBase<TYPE, objPoolAllocator>::propagate_on_container_copy_assignment;
        using propagate_on_container_move_assignment = std::true_type; ///< Allows propagation on move

        /**
        * @brief Constructs a new object pool allocator
        * @param index_max Maximum size class index (default 8-1=7 → 128 bytes)
        * @param count Initial number of chunks per size class (default 4)
        */
        explicit objPoolAllocator(u_integer index_max = 8, u_integer count = 4);

        objPoolAllocator(const objPoolAllocator&) = delete; ///< Copy construction disabled
        objPoolAllocator& operator=(const objPoolAllocator&) = delete; ///< Copy assignment disabled

        /**
        * @brief Move constructor
        * @param other Allocator to move from
        */
        objPoolAllocator(objPoolAllocator&& other) noexcept;

        /**
        * @brief Move assignment operator
        * @param other Allocator to move from
        * @return Reference to this allocator
        */
        objPoolAllocator& operator=(objPoolAllocator&& other) noexcept;

        /**
        * @brief Allocates memory from the pool
        * @param size Number of elements to allocate
        * @return Pointer to allocated memory
        * @throw allocateError When memory allocation fails
        * @note For large allocations, falls back to global operator new
        */
        TYPE* allocate(u_integer size) override;

        /**
        * @brief Returns memory to the pool
        * @param ptr Pointer to memory to free
        * @param size Number of elements originally allocated
        * @note For large allocations, uses global operator delete
        */
        void deallocate(TYPE* ptr, u_integer size) override;

        /**
        * @brief Destructor - releases all allocated memory
        */
        ~objPoolAllocator() override;
    };
}

template<typename TYPE>
TYPE* original::allocators::malloc(const u_integer size) {
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
void original::allocators::free(TYPE* ptr) {
    ::operator delete(ptr);
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
    return allocators::malloc<TYPE>(size);
}

template<typename TYPE>
void original::allocator<TYPE>::deallocate(TYPE* ptr, u_integer) {
    allocators::free(ptr);
}

template<typename TYPE>
constexpr original::u_integer original::objPoolAllocator<TYPE>::getChunkIndex(const u_integer size) {
    u_integer index = 0;
    while (static_cast<u_integer>(1) << index < size) {
        index += 1;
    }
    return index;
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
        allocators::free(this->allocated_list_head->chunks);
        allocators::free(this->allocated_list_head);
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
original::objPoolAllocator<TYPE>::objPoolAllocator(const u_integer index_max, const u_integer count)
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

    if (index >= this->size_index_max) {
        return allocators::malloc<TYPE>(size);
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

    if (index >= this->size_index_max) {
        allocators::free(ptr);
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
