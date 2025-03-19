#ifndef OWNERPTR_H
#define OWNERPTR_H

#include "autoPtr.h"

namespace original {
    template <typename TYPE, typename DELETER = deleter<TYPE>>
    class ownerPtr final : public autoPtr<TYPE, ownerPtr<TYPE, DELETER>, DELETER>{
    public:
        explicit ownerPtr(TYPE* p = nullptr);

        ownerPtr(const ownerPtr& other) = delete;

        ownerPtr& operator=(const ownerPtr& other) = delete;

        ownerPtr(ownerPtr&& other) noexcept;

        ownerPtr& operator=(ownerPtr&& other) noexcept;

        const TYPE& operator*() const;

        const TYPE* operator->() const;

        TYPE& operator*();

        TYPE* operator->();

        TYPE* unlock();

        [[nodiscard]] std::string className() const override;

        ~ownerPtr() override;

        template <typename T, typename DEL>
        friend ownerPtr<T, DEL> makeOwnerPtr();
    };

    template <typename T, typename DEL = deleter<T>>
    ownerPtr<T, DEL> makeOwnerPtr();


    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>::ownerPtr(TYPE *p)
        : autoPtr<TYPE, ownerPtr<TYPE>, DELETER>(p) {
        this->addStrongRef();
    }

    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>::ownerPtr(ownerPtr &&other) noexcept : ownerPtr(nullptr){
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>& ownerPtr<TYPE, DELETER>::operator=(ownerPtr&& other) noexcept {
        if (this == &other || this->ref_count == other.ref_count)
            return *this;

        this->ref_count = std::move(other.ref_count);
        return *this;
    }

    template<typename TYPE, typename DELETER>
    const TYPE& ownerPtr<TYPE, DELETER>::operator*() const {
        return *this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    const TYPE* ownerPtr<TYPE, DELETER>::operator->() const {
        return this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    TYPE& ownerPtr<TYPE, DELETER>::operator*() {
        return *this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    TYPE* ownerPtr<TYPE, DELETER>::operator->() {
        return this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    TYPE* ownerPtr<TYPE, DELETER>::unlock() {
        this->removeStrongRef();
        TYPE* p = this->getPtr();
        this->setPtr(nullptr);
        return p;
    }

    template<typename TYPE, typename DELETER>
    std::string ownerPtr<TYPE, DELETER>::className() const {
        return "ownerPtr";
    }

    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>::~ownerPtr() {
        this->removeStrongRef();
    }

    template<typename T, typename DEL>
    ownerPtr<T, DEL> makeOwnerPtr() {
        return ownerPtr<T, DEL>(new T{});
    }
}

#endif //OWNERPTR_H
