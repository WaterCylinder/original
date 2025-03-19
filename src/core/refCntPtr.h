#ifndef REFCNTPTR_H
#define REFCNTPTR_H

#include "autoPtr.h"

namespace original{
    template<typename TYPE, typename DERIVED, typename DELETER = deleter<TYPE>>
    class refCntPtr : public autoPtr<TYPE, DERIVED, DELETER>{
    protected:
        explicit refCntPtr(TYPE* p);
    public:
        [[nodiscard]] std::string className() const override;

        [[nodiscard]] std::string toString(bool enter) const override;

        ~refCntPtr() override = default;
    };

    template<typename TYPE, typename DELETER = deleter<TYPE>>
    class strongPtr;

    template<typename TYPE, typename DELETER = deleter<TYPE>>
    class weakPtr;

    class refCntPointers;


    template<typename TYPE, typename DELETER>
    class strongPtr final : public refCntPtr<TYPE, strongPtr<TYPE, DELETER>, DELETER>{
        friend class refCntPointers;

    public:
        explicit strongPtr(TYPE* p = nullptr);

        strongPtr(const strongPtr& other);

        strongPtr& operator=(const strongPtr& other);

        strongPtr(strongPtr&& other) noexcept;

        strongPtr& operator=(strongPtr&& other) noexcept;

        const TYPE& operator*() const;

        const TYPE* operator->() const;

        TYPE& operator*();

        TYPE* operator->();

        [[nodiscard]] std::string className() const override;

        ~strongPtr() override;

        template <typename T, typename DEL>
        friend strongPtr<T, DEL> makeStrongPtr();
    };

    template<typename TYPE, typename DELETER>
    class weakPtr final : public refCntPtr<TYPE, weakPtr<TYPE, DELETER>, DELETER>{
        friend class refCntPointers;

        explicit weakPtr();
    public:
        explicit weakPtr(const strongPtr<TYPE, DELETER>& other);

        weakPtr& operator=(const strongPtr<TYPE, DELETER>& other);

        weakPtr(const weakPtr& other);

        weakPtr& operator=(const weakPtr& other);

        ~weakPtr() override;
    };

    class refCntPointers{
        template <typename T, typename DEL = deleter<T>>
        static strongPtr<T, DEL> lock(const weakPtr<T, DEL>& weak_ptr);
    };

    template <typename T, typename DEL = deleter<T>>
    strongPtr<T, DEL> makeStrongPtr();


    // ----------------- Definitions of refCntPtr.h -----------------


    template<typename TYPE, typename DERIVED, typename DELETER>
    refCntPtr<TYPE, DERIVED, DELETER>::refCntPtr(TYPE *p)
        : autoPtr<TYPE, DERIVED, DELETER>::autoPtr(p) {}

    template<typename TYPE, typename DERIVED, typename DELETER>
    std::string refCntPtr<TYPE, DERIVED, DELETER>::className() const {
        return "refCntPtr";
    }

    template<typename TYPE, typename DERIVED, typename DELETER>
    std::string refCntPtr<TYPE, DERIVED, DELETER>::toString(bool enter) const {
        std::stringstream ss;
        ss << this->className() << "(";
        ss << this->ptr << ", ";
        ss << "strong ref: " << this->strongRefs() << ", " << "weak ref: " << this->weakRefs();
        ss << ")";
        if (enter)
            ss << "\n";
        return ss.str();
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(TYPE *p)
        : refCntPtr<TYPE, strongPtr<TYPE, DELETER>, DELETER>(p) {
        this->addStrongRef();
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(const strongPtr& other) : strongPtr(nullptr){
        this->operator=(other);
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>& strongPtr<TYPE, DELETER>::operator=(const strongPtr& other) {
        if (this == &other || this->ref_count == other.ref_count)
            return *this;

        this->removeStrongRef();
        this->clean();
        this->ref_count = other.ref_count;
        this->addStrongRef();
        return *this;
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(strongPtr&& other) noexcept : strongPtr(nullptr) {
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>& strongPtr<TYPE, DELETER>::operator=(strongPtr&& other) noexcept {
        if (this == &other || this->ref_count == other.ref_count)
            return *this;

        this->ref_count = std::move(other.ref_count);
        return *this;
    }

    template<typename TYPE, typename DELETER>
    const TYPE& strongPtr<TYPE, DELETER>::operator*() const {
        return *this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    const TYPE* strongPtr<TYPE, DELETER>::operator->() const {
        return this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    TYPE& strongPtr<TYPE, DELETER>::operator*() {
        return *this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    TYPE* strongPtr<TYPE, DELETER>::operator->() {
        return this->getPtr();
    }

    template<typename TYPE, typename DELETER>
    std::string strongPtr<TYPE, DELETER>::className() const {
        return "strongPtr";
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::~strongPtr() {
        this->removeStrongRef();
    }

    template<typename T, typename DEL>
    strongPtr<T, DEL> refCntPointers::lock(const weakPtr<T, DEL>& weak_ptr) {
        if (!weak_ptr.expired()){
            strongPtr<T, DEL> strong_ptr;
            strong_ptr.ref_count = weak_ptr.ref_count;
            strong_ptr.ref_count->addStrongRef();
            return strong_ptr;
        }
        return strongPtr<T, DEL>();
    }

    template<typename T, typename DEL>
    strongPtr<T, DEL> makeStrongPtr() {
        return std::move(strongPtr<T, DEL>(new T{}));
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr()
        : refCntPtr<TYPE, weakPtr<TYPE, DELETER>, DELETER>(nullptr) {
        this->addWeakRef();
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr(const strongPtr<TYPE, DELETER>& other)
        : weakPtr() {
        this->operator=(other);
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>& weakPtr<TYPE, DELETER>::operator=(const strongPtr<TYPE, DELETER>& other) {
        if (this->ref_count == other.ref_count)
            return *this;

        this->ref_count = other.ref_count;
        this->addWeakRef();

        return *this;
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr(const weakPtr& other)
        : weakPtr() {
        this->operator=(other);
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>& weakPtr<TYPE, DELETER>::operator=(const weakPtr& other) {
        if (this == &other || this->ref_count == other.ref_count)
            return *this;

        this->removeWeakRef();
        this->clean();
        this->ref_count = other.ref_count;
        this->addWeakRef();
        return *this;
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::~weakPtr() {
        this->removeWeakRef();
    }
}

#endif //REFCNTPTR_H
