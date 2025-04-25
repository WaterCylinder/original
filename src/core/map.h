#ifndef MAP_H
#define MAP_H

#include "allocator.h"
#include "container.h"
#include "couple.h"



namespace original {
    template <typename K_TYPE,
              typename V_TYPE,
              typename ALLOC = allocator<couple<const K_TYPE, V_TYPE>>>
    class map : public container<couple<const K_TYPE, V_TYPE>, ALLOC>{
    protected:
        explicit map(ALLOC alloc = ALLOC{});
    public:
        virtual bool add(const K_TYPE& k, const V_TYPE& v) = 0;

        virtual bool remove(const K_TYPE& k) = 0;

        [[nodiscard]] virtual bool containsKey(const K_TYPE& k) const = 0;

        virtual V_TYPE get(const K_TYPE& k) const = 0;

        virtual bool update(const K_TYPE &key, const V_TYPE &value) = 0;

        virtual const V_TYPE& operator[](const K_TYPE& k) const = 0;

        virtual V_TYPE& operator[](const K_TYPE& k) = 0;

        ~map() override;
    };

    template<typename K_TYPE, typename V_TYPE, typename ALLOC>
    map<K_TYPE, V_TYPE, ALLOC>::map(ALLOC alloc)
            : container<couple<const K_TYPE, V_TYPE>, ALLOC>(std::move(alloc)) {}

    template<typename K_TYPE, typename V_TYPE, typename ALLOC>
    map<K_TYPE, V_TYPE, ALLOC>::~map() = default;
}

#endif //MAP_H
