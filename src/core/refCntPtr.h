#ifndef REFCNTPTR_H
#define REFCNTPTR_H

#include "autoPtr.h"
#include "deleter.h"


namespace original{
    template<typename TYPE, typename DERIVED, typename DELETER>
    class refCntPtr : public autoPtr<TYPE, DERIVED, DELETER>{
    protected:
        explicit refCntPtr(TYPE* p = nullptr);
    public:
        [[nodiscard]] std::string className() const override;

        [[nodiscard]] std::string toString(bool enter) const override;

        ~refCntPtr() override = default;
    };

    template<typename TYPE, typename DELETER = deleter<TYPE>>
    class strongPtr;

    template<typename TYPE, typename DELETER = deleter<TYPE>>
    class weakPtr;


    template<typename TYPE, typename DELETER>
    class strongPtr final : public refCntPtr<TYPE, strongPtr<TYPE, DELETER>, DELETER>{
        friend class weakPtr<TYPE, DELETER>;

    public:
        explicit strongPtr(TYPE* p = nullptr);

        strongPtr(const strongPtr& other);

        strongPtr& operator=(const strongPtr& other);

        strongPtr(strongPtr&& other) noexcept;

        strongPtr& operator=(strongPtr&& other) noexcept;

        [[nodiscard]] std::string className() const override;

        ~strongPtr() override;

        template <typename T, typename DEL>
        friend strongPtr<T, DEL> makeStrongPtr();
    };

    template<typename TYPE, typename DELETER>
    class weakPtr final : public refCntPtr<TYPE, weakPtr<TYPE, DELETER>, DELETER>{
        friend class strongPtr<TYPE, DELETER>;

        explicit weakPtr();
    public:
        explicit weakPtr(const strongPtr<TYPE, DELETER>& other);

        weakPtr& operator=(const strongPtr<TYPE, DELETER>& other);

        weakPtr(const weakPtr& other);

        weakPtr& operator=(const weakPtr& other);

        weakPtr(weakPtr&& other) noexcept;

        weakPtr& operator=(weakPtr&& other) noexcept;

        strongPtr<TYPE, DELETER> lock() const;

        const TYPE& operator*() const override;

        const TYPE* operator->() const override;

        TYPE& operator*() override;

        TYPE* operator->() override;

        [[nodiscard]] std::string className() const override;

        ~weakPtr() override;
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
    std::string refCntPtr<TYPE, DERIVED, DELETER>::toString(const bool enter) const {
        std::stringstream ss;
        ss << this->className() << "(";
        ss << printable::formatString(this->getPtr()) << ", ";
        ss << "strong ref: " << this->strongRefs() << ", " << "weak ref: " << this->weakRefs();
        ss << ")";
        if (enter)
            ss << "\n";
        return ss.str();
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(TYPE *p)
        : refCntPtr<TYPE, strongPtr, DELETER>(p) {
        this->addStrongRef();
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(const strongPtr& other) : strongPtr(){
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
    strongPtr<TYPE, DELETER>::strongPtr(strongPtr&& other) noexcept : strongPtr() {
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>& strongPtr<TYPE, DELETER>::operator=(strongPtr&& other) noexcept {
        if (this == &other || this->ref_count == other.ref_count)
            return *this;

        this->removeStrongRef();
        this->clean();
        this->ref_count = other.ref_count;
        other.ref_count = autoPtr<TYPE, strongPtr, DELETER>::newRefCount();
        other.addStrongRef();
        return *this;
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
    strongPtr<T, DEL> makeStrongPtr() {
        return strongPtr<T, DEL>(new T{});
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr()
        : refCntPtr<TYPE, weakPtr, DELETER>(nullptr) {
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

        this->removeWeakRef();
        this->clean();
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
    weakPtr<TYPE, DELETER>::weakPtr(weakPtr&& other) noexcept : weakPtr() {
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>& weakPtr<TYPE, DELETER>::operator=(weakPtr&& other) noexcept {
        if (this == &other || this->ref_count == other.ref_count)
            return *this;

        this->removeWeakRef();
        this->clean();
        this->ref_count = other.ref_count;
        other.ref_count = autoPtr<TYPE, weakPtr, DELETER>::newRefCount();
        other.addWeakRef();
        return *this;
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER> weakPtr<TYPE, DELETER>::lock() const {
        strongPtr<TYPE, DELETER> strong_ptr;
        if (!this->expired()){
            strong_ptr.removeStrongRef();
            strong_ptr.clean();
            strong_ptr.ref_count = this->ref_count;
            strong_ptr.addStrongRef();
        }
        return strong_ptr;
    }

    template<typename TYPE, typename DELETER>
    const TYPE& weakPtr<TYPE, DELETER>::operator*() const {
        return this->lock().operator*();
    }

    template<typename TYPE, typename DELETER>
    const TYPE* weakPtr<TYPE, DELETER>::operator->() const {
        return this->lock().operator->();
    }

    template<typename TYPE, typename DELETER>
    TYPE& weakPtr<TYPE, DELETER>::operator*() {
        return this->lock().operator*();
    }

    template<typename TYPE, typename DELETER>
    TYPE* weakPtr<TYPE, DELETER>::operator->() {
        return this->lock().operator->();
    }

    template<typename TYPE, typename DELETER>
    std::string weakPtr<TYPE, DELETER>::className() const {
        return "weakPtr";
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::~weakPtr() {
        this->removeWeakRef();
    }
}

#endif //REFCNTPTR_H
