#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "type_traits"
#include "utility"


namespace original {
    template<typename TYPE, template <typename> typename DERIVED>
    class allocatorBase{
    public:
        using propagate_on_container_copy_assignment = std::false_type;
        using propagate_on_container_move_assignment = std::false_type;

        template <typename O_TYPE>
        using rebind_alloc = DERIVED<O_TYPE>;

        constexpr allocatorBase() = default;

        virtual TYPE* allocate(u_integer size) = 0;

        virtual void deallocate(TYPE* ptr, u_integer size) = 0;

        template<typename O_TYPE, typename... Args>
        void construct(O_TYPE* o_ptr, Args&&... args);

        template<typename O_TYPE>
        static void destroy(O_TYPE* o_ptr);

        virtual ~allocatorBase() = 0;
    };

    template<typename TYPE>
    class allocator : public allocatorBase<TYPE, allocator> {
    public:
        using typename allocatorBase<TYPE, allocator>::propagate_on_container_copy_assignment;
        using typename allocatorBase<TYPE, allocator>::propagate_on_container_move_assignment;

        TYPE* allocate(u_integer size) override;

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
