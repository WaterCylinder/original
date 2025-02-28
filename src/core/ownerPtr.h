#ifndef OWNERPTR_H
#define OWNERPTR_H

#include "autoPtr.h"

namespace original {
    // todo
    template <typename TYPE>
    class ownerPtr : public autoPtr<TYPE, ownerPtr<TYPE>>{
    public:
        explicit ownerPtr(TYPE* p = nullptr);

        ownerPtr(const ownerPtr& other) = delete;

        ownerPtr& operator=(const ownerPtr& other) = delete;

        ownerPtr(ownerPtr&& other) noexcept;

        ownerPtr& operator=(ownerPtr&& other) noexcept;
    };

    template<typename TYPE>
    original::ownerPtr<TYPE>::ownerPtr(TYPE *p) : autoPtr<TYPE, ownerPtr<TYPE>>(p, nullptr) {}

    template<typename TYPE>
    ownerPtr<TYPE>::ownerPtr(ownerPtr &&other) noexcept {

    }

    template<typename TYPE>
    ownerPtr<TYPE>& ownerPtr<TYPE>::operator=(ownerPtr &&other) noexcept {
        return *this;
    }

}

#endif //OWNERPTR_H
