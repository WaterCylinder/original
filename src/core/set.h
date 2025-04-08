#ifndef SET_H
#define SET_H


#include "allocator.h"
#include "container.h"


namespace original {
    template <typename K_TYPE, typename ALLOC = allocator<K_TYPE>>
    class set : public container<K_TYPE, ALLOC> {
    protected:
        explicit set(ALLOC alloc = ALLOC{});
    public:
        virtual bool add(const K_TYPE& e) = 0;

        virtual bool remove(const K_TYPE& e) = 0;

        ~set() override;
    };
}

template<typename K_TYPE, typename ALLOC>
original::set<K_TYPE, ALLOC>::set(ALLOC alloc)
    : container<K_TYPE, ALLOC>(std::move(alloc)) {}

template<typename K_TYPE, typename ALLOC>
original::set<K_TYPE, ALLOC>::~set() = default;

#endif //SET_H
