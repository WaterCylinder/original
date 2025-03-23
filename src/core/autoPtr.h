#ifndef AUTOPTR_H
#define AUTOPTR_H

#include "config.h"
#include "printable.h"
#include "comparable.h"
#include "error.h"

/**
* @file autoPtr.h
* @brief Base class for reference-counted smart pointers
* @details Implements core functionality for automatic memory management
* using reference counting. Supports strong/weak references and custom deleters.
* Inherits printable and comparable interfaces for debugging and comparison.
*/

namespace original {
    // Forward declaration for friend class
    template<typename TYPE, typename DELETER>
    class refCount;

    /**
    * @class autoPtr
    * @tparam TYPE Managed object type
    * @tparam DERIVED CRTP pattern parameter for inheritance
    * @tparam DELETER Custom deleter policy type
    * @brief Base smart pointer with reference counting
    * @details Provides core resource management capabilities through:
    * - Strong/weak reference tracking
    * - Automatic cleanup when references reach zero
    * - Custom deletion policies via template parameter
    * @extends printable
    * @extends comparable
    */
    template<typename TYPE, typename DERIVED, typename DELETER>
    class autoPtr : public printable, public comparable<autoPtr<TYPE, DERIVED, DELETER>>{
    protected:
        refCount<TYPE, DELETER>* ref_count; ///< Reference counter object

        /**
        * @brief Construct from raw pointer
        * @param p Raw pointer to manage
        * @warning Ownership is transferred to autoPtr
        */
        explicit autoPtr(TYPE* p);

        /**
        * @brief Replace managed pointer
        * @param p New pointer to manage
        * @throws nullPointerError if no active references
        */
        void setPtr(TYPE* p);

        /**
        * @brief Increment strong reference count
        * @internal Reference set method
        */
        void addStrongRef();

        /**
        * @brief Increment weak reference count
        * @internal Reference set method
        */
        void addWeakRef();

        /**
        * @brief Decrement strong reference count
        * @internal Reference set method
        */
        void removeStrongRef();

        /**
        * @brief Decrement weak reference count
        * @internal Reference set method
        */
        void removeWeakRef();

        /**
        * @brief Destroy reference counter
        * @note Only destroys counter when both ref counts reach zero
        */
        void destroyRefCnt() noexcept;

        /**
        * @brief Cleanup resources when expired
        * @internal Resource management method
        */
        void clean() noexcept;

        /**
        * @brief Create new reference counter
        * @param p Pointer to manage (nullptr allowed)
        * @return Newly created counter object
        */
        static refCount<TYPE, DELETER>* newRefCount(TYPE* p = nullptr);

    public:

        /**
        * @brief Get strong reference count
        * @return Current number of strong references
        */
        u_integer strongRefs() const;

        /**
        * @brief Get weak reference count
        * @return Current number of weak references
        */
        u_integer weakRefs() const;

        /**
        * @brief Check active ownership
        * @return True if it has active references
        */
        bool exist() const;

        /**
        * @brief Check resource validity
        * @return True if strong references == 0
        */
        bool expired() const;

        /**
        * @brief Boolean conversion operator
        * @return Equivalent to exist()
        */
        explicit operator bool() const;

        /**
        * @brief Get managed pointer
        * @return Raw pointer to managed object
        * @throws nullPointerError if no active references
        */
        TYPE* get() const;

        /**
        * @brief Const dereference operator
        * @return Reference to managed object
        * @throws nullPointerError if no managed object
        */
        virtual const TYPE& operator*() const;

        /**
        * @brief Const member access operator
        * @return Pointer to managed object
        * @throws nullPointerError if no managed object
        */
        virtual const TYPE* operator->() const;

        /**
        * @brief Const array access operator
        * @param index Array index
        * @return Reference to element
        * @throws nullPointerError if no managed array
        */
        virtual const TYPE& operator[](u_integer index) const;

        // Mutable accessors
        /**
        * @brief Mutable dereference operator
        * @return Reference to managed object
        * @throws nullPointerError if no managed object
        */
        virtual TYPE& operator*();

        /**
        * @brief Mutable member access operator
        * @return Pointer to managed object
        * @throws nullPointerError if no managed object
        */
        virtual TYPE* operator->();

        /**
        * @brief Mutable array access operator
        * @param index Array index
        * @return Reference to element
        * @throws nullPointerError if no managed array
        */
        virtual TYPE& operator[](u_integer index);

        /**
        * @brief Compare reference counters
        * @param other autoPtr to compare with
        * @return Difference between refcount addresses
        */
        integer compareTo(const autoPtr& other) const override;

        /**
        * @brief Get class name string
        * @return Name of autoPtr
        */
        std::string className() const override;

        /**
        * @brief String representation formatter
        * @param enter Add newline if true
        * @return Formatted resource info
        */
        std::string toString(bool enter) const override;

        /**
        * @brief Destructor triggers reference cleanup
        */
        ~autoPtr() override;
    };

    /**
    * @class refCount
    * @tparam TYPE Managed object type
    * @tparam DELETER Custom deleter policy type
    * @brief Reference counting metadata container
    * @details Stores:
    * - Managed pointer
    * - Strong/weak reference counts
    * - Deleter policy instance
    */
    template<typename TYPE, typename DELETER>
    class refCount {
        template <typename, typename, typename>
        friend class autoPtr;

        TYPE* ptr;             ///< Managed raw pointer
        u_integer strong_refs; ///< Strong reference counter
        u_integer weak_refs;   ///< Weak reference counter
        DELETER deleter;       ///< Deleter policy instance

        /**
        * @brief Construct refCount object
        * @param p Pointer to manage (nullptr allowed)
        */
        explicit refCount(TYPE* p = nullptr);

        /**
        * @brief Destroy managed pointer using deleter
        */
        void destroyPtr() noexcept;

        /**
        * @brief Destructor ensures resource cleanup
        */
        ~refCount();
    };
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::autoPtr(TYPE* p)
    : ref_count(newRefCount(p)) {}


template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::setPtr(TYPE* p) {
    if (!this->exist()){
        throw nullPointerError();
    }
    this->ref_count->ptr = p;
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
    if (!this->exist()){
        this->destroyRefCnt();
    }
    if (this->expired()){
        this->ref_count->destroyPtr();
    }
}

template <typename TYPE, typename DERIVED, typename DELETER>
original::refCount<TYPE, DELETER>* original::autoPtr<TYPE, DERIVED, DELETER>::newRefCount(TYPE* p)
{
    return new refCount<TYPE, DELETER>(p);
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
bool original::autoPtr<TYPE, DERIVED, DELETER>::exist() const {
    return this->ref_count && (this->strongRefs() > 0 || this->weakRefs() > 0);
}

template<typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::expired() const {
    return this->ref_count && this->strongRefs() == 0;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::operator bool() const {
    return this->exist() && this->get();
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE* original::autoPtr<TYPE, DERIVED, DELETER>::get() const {
    if (!this->exist()){
        throw nullPointerError();
    }
    return this->ref_count->ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator*() const {
    if (!this->get())
        throw nullPointerError();
    return *this->get();
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE*
original::autoPtr<TYPE, DERIVED, DELETER>::operator->() const {
    if (!this->get())
        throw nullPointerError();
    return this->get();
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator[](u_integer index) const {
    if (!this->get())
        throw nullPointerError();
    return this->get()[index];
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE &original::autoPtr<TYPE, DERIVED, DELETER>::operator*() {
    if (!this->get())
        throw nullPointerError();
    return *this->get();
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE*
original::autoPtr<TYPE, DERIVED, DELETER>::operator->() {
    if (!this->get())
        throw nullPointerError();
    return this->get();
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator[](u_integer index) {
    if (!this->get())
        throw nullPointerError();
    return this->get()[index];
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
    ss << formatString(this->get());
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
    TYPE* tmp = this->ptr;
    this->ptr = nullptr;
    this->deleter(tmp);
}

template<typename TYPE, typename DELETER>
original::refCount<TYPE, DELETER>::~refCount() {
    this->destroyPtr();
}

#endif //AUTOPTR_H
