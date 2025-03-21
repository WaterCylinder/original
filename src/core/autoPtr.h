#ifndef AUTOPTR_H
#define AUTOPTR_H

#include "config.h"
#include "printable.h"
#include "comparable.h"
#include "error.h"


namespace original{
    template<typename TYPE, typename DELETER>
    class refCount;

    template<typename TYPE, typename DERIVED, typename DELETER>
    class autoPtr : public printable, public comparable<autoPtr<TYPE, DERIVED, DELETER>>{
    protected:
        refCount<TYPE, DELETER>* ref_count;

        explicit autoPtr(TYPE* p);

        TYPE* getPtr() const;

        void setPtr(TYPE* p);

        u_integer strongRefs() const;

        u_integer weakRefs() const;

        void addStrongRef();

        void addWeakRef();

        void removeStrongRef();

        void removeWeakRef();

        void destroyRefCnt() noexcept;

        void clean() noexcept;

        static refCount<TYPE, DELETER>* newRefCount(TYPE* p = nullptr);
    public:
        bool exist() const;

        bool expired() const;

        explicit operator bool() const;

        virtual const TYPE& operator*() const;

        virtual const TYPE* operator->() const;

        virtual TYPE& operator*();

        virtual TYPE* operator->();

        integer compareTo(const autoPtr& other) const override;

        std::string className() const override;

        std::string toString(bool enter) const override;

        ~autoPtr() override;
    };

    template<typename TYPE, typename DELETER>
    class refCount {
        template <typename, typename, typename>
        friend class autoPtr;

        TYPE* ptr;
        u_integer strong_refs;
        u_integer weak_refs;
        DELETER deleter;

        explicit refCount(TYPE* p = nullptr);
        void destroyPtr() noexcept;
        ~refCount();
    };

}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::autoPtr(TYPE* p)
    : ref_count(newRefCount(p)) {}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE* original::autoPtr<TYPE, DERIVED, DELETER>::getPtr() const {
    if (!this->exist()){
        throw nullPointerError();
    }
    return this->ref_count->ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::setPtr(TYPE* p) {
    if (!this->exist()){
        throw nullPointerError();
    }
    this->ref_count->ptr = p;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::u_integer original::autoPtr<TYPE, DERIVED, DELETER>::strongRefs() const {
    return this->ref_count->strong_refs;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::u_integer original::autoPtr<TYPE, DERIVED, DELETER>::weakRefs() const {
    return this->ref_count->weak_refs;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::addStrongRef() {
    this->ref_count->strong_refs += 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::addWeakRef() {
    this->ref_count->weak_refs += 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::removeStrongRef() {
    this->ref_count->strong_refs -= 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::removeWeakRef() {
    this->ref_count->weak_refs -= 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::destroyRefCnt() noexcept {
    delete this->ref_count;
    this->ref_count = nullptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::clean() noexcept {
    if (this->expired()){
        this->ref_count->destroyPtr();
    }
    if (!this->exist()){
        this->destroyRefCnt();
    }
}

template <typename TYPE, typename DERIVED, typename DELETER>
original::refCount<TYPE, DELETER>* original::autoPtr<TYPE, DERIVED, DELETER>::newRefCount(TYPE* p)
{
    return new refCount<TYPE, DELETER>(p);
}

template<typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::exist() const {
    return this->ref_count && (this->strongRefs() > 0 || this->weakRefs() > 0);
}

template<typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::expired() const {
    return this->ref_count && this->strongRefs() == 0;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::operator bool() const {
    return this->exist() && this->getPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator*() const {
    if (!this->getPtr())
        throw nullPointerError();
    return *this->getPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE*
original::autoPtr<TYPE, DERIVED, DELETER>::operator->() const {
    if (!this->getPtr())
        throw nullPointerError();
    return this->getPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE &original::autoPtr<TYPE, DERIVED, DELETER>::operator*() {
    if (!this->getPtr())
        throw nullPointerError();
    return *this->getPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE*
original::autoPtr<TYPE, DERIVED, DELETER>::operator->() {
    if (!this->getPtr())
        throw nullPointerError();
    return this->getPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::integer original::autoPtr<TYPE, DERIVED, DELETER>::compareTo(const autoPtr& other) const {
    return this->ref_count - other.ref_count;
}

template<typename TYPE, typename DERIVED, typename DELETER>
std::string original::autoPtr<TYPE, DERIVED, DELETER>::className() const {
    return "autoPtr";
}

template<typename TYPE, typename DERIVED, typename DELETER>
std::string original::autoPtr<TYPE, DERIVED, DELETER>::toString(const bool enter) const {
    std::stringstream ss;
    ss << this->className() << "(";
    ss << formatString(this->getPtr());
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::~autoPtr() {
    this->clean();
}

template<typename TYPE, typename DELETER>
original::refCount<TYPE, DELETER>::refCount(TYPE *p)
    : ptr(p), strong_refs(0), weak_refs(0) {}

template<typename TYPE, typename DELETER>
void original::refCount<TYPE, DELETER>::destroyPtr() noexcept {
    this->deleter(this->ptr);
    this->ptr = nullptr;
}

template<typename TYPE, typename DELETER>
original::refCount<TYPE, DELETER>::~refCount() {
    this->destroyPtr();
}

#endif //AUTOPTR_H
