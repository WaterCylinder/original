#ifndef ALLOCATOR_H
#define ALLOCATOR_H

namespace original {
    template<typename TYPE>
    class allocatorBase{
    public:

        constexpr allocatorBase() = default;

        virtual TYPE* allocate(u_integer size) = 0;

        virtual void deallocate(TYPE* ptr, u_integer size) = 0;

        template<typename O_TYPE, typename... Args>
        void construct(O_TYPE* o_ptr, Args&&... args);

        template<typename O_TYPE>
        void destroy(O_TYPE* o_ptr);

        virtual ~allocatorBase() = 0;
    };

    template<typename TYPE>
    class allocator : public allocatorBase<TYPE> {
        TYPE* allocate(u_integer size) override;

        void deallocate(TYPE* ptr, u_integer size) override;
    };
}

template<typename TYPE>
original::allocatorBase<TYPE>::~allocatorBase() = default;

template<typename TYPE>
template<typename O_TYPE, typename... Args>
void original::allocatorBase<TYPE>::construct(O_TYPE *o_ptr, Args &&... args) {
    new (o_ptr) O_TYPE{std::forward<Args>(args)...};
}

template<typename TYPE>
template<typename O_TYPE>
void original::allocatorBase<TYPE>::destroy(O_TYPE *o_ptr) {
    o_ptr->~O_TYPE();
}

template<typename TYPE>
TYPE* original::allocator<TYPE>::allocate(original::u_integer size) {
    return static_cast<TYPE*>(::operator new(size * sizeof(TYPE)));
}

template<typename TYPE>
void original::allocator<TYPE>::deallocate(TYPE *ptr, original::u_integer) {
    ::operator delete(ptr);
}

#endif //ALLOCATOR_H
