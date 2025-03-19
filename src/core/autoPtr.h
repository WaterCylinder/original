#ifndef AUTOPTR_H
#define AUTOPTR_H

#include "config.h"
#include "printable.h"
#include "comparable.h"
#include "deleter.h"


namespace original{
    template<typename TYPE, typename DERIVED, typename DELETER = deleter<TYPE>>
    class autoPtr : public printable, public comparable<DERIVED>{
    protected:
        class refCount {
            TYPE* ptr;
            u_integer strong_refs;
            u_integer weak_refs;
            DELETER deleter;

        public:
            friend class autoPtr;

            explicit refCount(TYPE* p);

            refCount(refCount&& other) noexcept;

            refCount& operator=(refCount&& other) noexcept;

            void destroyPtr() noexcept;

            ~refCount();
        };

        refCount* ref_count;

        explicit autoPtr(TYPE* p);

        TYPE* getPtr();

        void setPtr(TYPE* p);

        u_integer strongRefs() const;

        u_integer weakRefs() const;

        void addStrongRef();

        void addWeakRef();

        void removeStrongRef();

        void removeWeakRef();

        void clean() noexcept;
    public:
        void* operator new(size_t) = delete;

        void operator delete(void*) = delete;

        bool valid() const;

        bool expired() const;

        integer compareTo(const autoPtr& other) const override;

        std::string className() const override;

        std::string toString(bool enter) const override;

        ~autoPtr() override;
    };
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::refCount::refCount(TYPE* p)
        : ptr(p), strong_refs(0), weak_refs(0) {}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::refCount::refCount(refCount &&other) noexcept : refCount(nullptr) {
    this->operator=(std::move(other));
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::refCount&
original::autoPtr<TYPE, DERIVED, DELETER>::refCount::operator=(refCount&& other) noexcept {
    if (this == &other)
        return *this;

    this->ptr = other->ptr;
    other.ptr = nullptr;
    this->strong_refs = other.strong_refs;
    other.strong_refs = 0;
    this->weak_refs = other.weak_refs;
    other.weak_refs = 0;
    this->deleter = std::move(other.deleter);
    return *this;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::refCount::destroyPtr() noexcept {
    this->deleter(this->ptr);
    this->ptr = nullptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::refCount::~refCount() {
    this->destroyPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::autoPtr(TYPE* p)
    : ref_count(new refCount(p)) {}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE *original::autoPtr<TYPE, DERIVED, DELETER>::getPtr() {
    return this->ref_count->ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::setPtr(TYPE* p) {
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
void original::autoPtr<TYPE, DERIVED, DELETER>::clean() noexcept {
    if (!this->valid()){
        delete this->ref_count;
    }
    if (this->expired()){
        this->ref_count->destroyPtr();
    }
}

template<typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::expired() const {
    return this->strongRefs() == 0;
}

template<typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::valid() const {
    return this->strongRefs() > 0 && this->weakRefs() > 0;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::integer original::autoPtr<TYPE, DERIVED, DELETER>::compareTo(const autoPtr& other) const {
    return this->getPtr() - other->getPtr();
}

template<typename TYPE, typename DERIVED, typename DELETER>
std::string original::autoPtr<TYPE, DERIVED, DELETER>::className() const {
    return "autoPtr";
}

template<typename TYPE, typename DERIVED, typename DELETER>
std::string original::autoPtr<TYPE, DERIVED, DELETER>::toString(bool enter) const {
    std::stringstream ss;
    ss << this->className() << "(";
    ss << this->ptr;
    ss << ")";
    if (enter)
        ss << "\n";
    return ss.str();
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::~autoPtr() {
    this->clean();
}

#endif //AUTOPTR_H
