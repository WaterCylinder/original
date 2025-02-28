#ifndef AUTOPTR_H
#define AUTOPTR_H

#include "printable.h"
#include "comparable.h"

namespace original{
    // todo
    template<typename TYPE, typename DERIVED>
    class autoPtr : public printable, public comparable<DERIVED>{
    protected:
        class refCount {
            uint32_t strong_refs;
            uint32_t weak_refs;

        public:
            friend class autoPtr;

            explicit refCount();

            [[nodiscard]] uint32_t strongRefs() const;

            [[nodiscard]] uint32_t weakRefs() const;

            void addStrongRef();

            void removeStrongRef();

            void addWeakRef();

            void removeWeakRef();
        };

        TYPE* ptr;
        refCount* ref_cnt;

        explicit autoPtr(TYPE* p, refCount* cnt);

        ~autoPtr() override = default;
    public:
        void* operator new(size_t) = delete;

        void operator delete(void*) = delete;

        const TYPE& operator*() const;

        const TYPE* operator->() const;

        bool isValid() const;

        int64_t compareTo(const autoPtr& p) const override;
    };
}

template<typename TYPE, typename DERIVED>
original::autoPtr<TYPE, DERIVED>::refCount::refCount()
    : strong_refs(1), weak_refs(0) {}

template<typename TYPE, typename DERIVED>
uint32_t original::autoPtr<TYPE, DERIVED>::refCount::strongRefs() const {
    return this->strong_refs;
}

template<typename TYPE, typename DERIVED>
uint32_t original::autoPtr<TYPE, DERIVED>::refCount::weakRefs() const {
    return this->weak_refs;
}

template<typename TYPE, typename DERIVED>
void original::autoPtr<TYPE, DERIVED>::refCount::addStrongRef() {
    this->strong_refs += 1;
}

template<typename TYPE, typename DERIVED>
void original::autoPtr<TYPE, DERIVED>::refCount::removeStrongRef() {
    this->strong_refs -= 1;
}

template<typename TYPE, typename DERIVED>
void original::autoPtr<TYPE, DERIVED>::refCount::addWeakRef() {
    this->weak_refs += 1;
}

template<typename TYPE, typename DERIVED>
void original::autoPtr<TYPE, DERIVED>::refCount::removeWeakRef() {
    this->weak_refs -= 1;
}

template<typename TYPE, typename DERIVED>
original::autoPtr<TYPE, DERIVED>::autoPtr(TYPE* p, refCount* cnt)
    : ptr(p), ref_cnt(cnt) {}

template<typename TYPE, typename DERIVED>
const TYPE& original::autoPtr<TYPE, DERIVED>::operator*() const {
    return *this->ptr;
}

template<typename TYPE, typename DERIVED>
const TYPE* original::autoPtr<TYPE, DERIVED>::operator->() const {
    return this->ptr;
}

template<typename TYPE, typename DERIVED>
bool original::autoPtr<TYPE, DERIVED>::isValid() const {
    return this->ref_cnt->strongRefs() > 0;
}

template<typename TYPE, typename DERIVED>
int64_t original::autoPtr<TYPE, DERIVED>::compareTo(const autoPtr& p) const {
    return this->ptr - p.ptr;
}

#endif //AUTOPTR_H
