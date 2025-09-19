#ifndef AUTOPTR_H
#define AUTOPTR_H

#include "config.h"
#include "printable.h"
#include "comparable.h"
#include "hash.h"
#include "error.h"

/**
* @file autoPtr.h
* @brief Base class for reference-counted smart pointers
* @details Implements core functionality for automatic memory management
* using reference counting. Supports strong/weak references and custom deleters.
* Inherits printable and comparable interfaces for debugging and comparison.
*
* Key Features:
* - Automatic memory management via reference counting
* - Support for both strong and weak references
* - Customizable deletion policies via template parameter
* - Thread-safe reference counting operations
* - Integration with printable, comparable and hashable interfaces
*
* @note This is a base class meant to be inherited by concrete smart pointer implementations
* @see sharedPtr For a complete implementation using this base
* @see weakPtr For the weak reference counterpart
*/

namespace original {
    // Forward declaration for refCountBase
    class refCountBase;

    // Forward declaration for refCount
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
    * - Exception-safe operations
    *
    * @extends printable For string representation capabilities
    * @extends comparable For comparison operations
    * @extends hashable For hashing support
    */
    template<typename TYPE, typename DERIVED, typename DELETER>
    class autoPtr : public printable,
                    public comparable<autoPtr<TYPE, DERIVED, DELETER>>,
                    public hashable<autoPtr<TYPE, DERIVED, DELETER>> {
        template<typename, typename, typename> friend class autoPtr;
    protected:
        refCountBase* ref_count; ///< Reference counter object
        TYPE* alias_ptr;         ///< Aliased pointer for type casting scenarios

        /**
        * @brief Construct from raw pointer
        * @param p Raw pointer to manage
        * @warning Ownership is transferred to autoPtr
        */
        explicit autoPtr(TYPE* p);

        /**
        * @brief Increment strong reference count
        * @internal Reference set method
        */
        void addStrongRef() const;

        /**
        * @brief Increment weak reference count
        * @internal Reference set method
        */
        void addWeakRef() const;

        /**
        * @brief Decrement strong reference count
        * @internal Reference set method
        */
        void removeStrongRef() const;

        /**
        * @brief Decrement weak reference count
        * @internal Reference set method
        */
        void removeWeakRef() const;

        /**
        * @brief Release ownership of the managed pointer
        * @return Raw pointer to the managed object
        * @note The caller becomes responsible for deleting the returned pointer
        * @post Reference counter retains weak references but no strong references
        */
        TYPE* releasePtr() noexcept;

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
        * @return Equivalent to a non null managed pointer
        */
        explicit operator bool() const;

        /**
        * @brief Logical NOT operator
        * @return Opposite of operator bool()
        */
        bool operator!() const;

        /**
        * @brief Get managed pointer const version
        * @return Raw pointer to managed object
        * @throws nullPointerError if no active references
        */
        const TYPE* get() const;

        /**
        * @brief Get managed pointer
        * @return Raw pointer to managed object
        * @throws nullPointerError if no active references
        */
        TYPE* get();

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
        * @brief Swaps the reference counters between two autoPtr instances
        * @param other Another autoPtr instance to swap with
        *
        * @post After swapping:
        * - Current object acquires other's reference counter
        * - Other object acquires current object's reference counter
        * - Managed object ownership transfers implicitly
        */
        void swap(autoPtr& other) noexcept;

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
        * @brief Compute hash value for the pointer
        * @return Hash of managed pointer address
        * @note Uses hash<TYPE> specialization
        */
        [[nodiscard]] u_integer toHash() const noexcept override;

        /**
        * @brief Equality comparison
        * @param other autoPtr to compare with
        * @return True if both manage same pointer
        * @note Compares pointer addresses, not values
        */
        bool equals(const autoPtr& other) const noexcept override;

        /**
        * @brief Destructor triggers reference cleanup
        */
        ~autoPtr() override;

        template<typename T, typename DER, typename DEL>
        friend bool operator==(const autoPtr<T, DER, DEL>& ptr, const std::nullptr_t& null);

        template<typename T, typename DER, typename DEL>
        friend bool operator!=(const autoPtr<T, DER, DEL>& ptr, const std::nullptr_t& null);

        template<typename T, typename DER, typename DEL>
        friend bool operator==(const std::nullptr_t& null, const autoPtr<T, DER, DEL>& ptr);

        template<typename T, typename DER, typename DEL>
        friend bool operator!=(const std::nullptr_t& null, const autoPtr<T, DER, DEL>& ptr);
    };

    /**
    * @brief Equality comparison with nullptr
    * @tparam T Managed object type
    * @tparam DER CRTP derived class type
    * @tparam DEL Deleter policy type
    * @param ptr autoPtr to compare
    * @param null nullptr_t to compare against
    * @return true if the autoPtr is empty (no managed object)
    * @note Equivalent to checking !operator bool() of ptr
    */
    template<typename T, typename DER, typename DEL>
    bool operator==(const autoPtr<T, DER, DEL>& ptr, const std::nullptr_t& null);

    /**
    * @brief Inequality comparison with nullptr
    * @tparam T Managed object type
    * @tparam DER CRTP derived class type
    * @tparam DEL Deleter policy type
    * @param ptr autoPtr to compare
    * @param null nullptr_t to compare against
    * @return true if the autoPtr is not empty (has a managed object)
    * @note Equivalent to checking operator bool() of ptr
    */
    template<typename T, typename DER, typename DEL>
    bool operator!=(const autoPtr<T, DER, DEL>& ptr, const std::nullptr_t& null);

    /**
    * @brief Equality comparison with nullptr (reversed operands)
    * @tparam T Managed object type
    * @tparam DER CRTP derived class type
    * @tparam DEL Deleter policy type
    * @param ptr autoPtr to compare
    * @param null nullptr_t to compare against
    * @return true if the autoPtr is empty (no managed object)
    * @note Equivalent to checking !operator bool() of ptr
    */
    template<typename T, typename DER, typename DEL>
    bool operator==(const std::nullptr_t& null, const autoPtr<T, DER, DEL>& ptr);

    /**
    * @brief Inequality comparison with nullptr (reversed operands)
    * @tparam T Managed object type
    * @tparam DER CRTP derived class type
    * @tparam DEL Deleter policy type
    * @param ptr autoPtr to compare
    * @param null nullptr_t to compare against
    * @return true if the autoPtr is not empty (has a managed object)
    * @note Equivalent to checking operator bool() of ptr
    */
    template<typename T, typename DER, typename DEL>
    bool operator!=(const std::nullptr_t& null, const autoPtr<T, DER, DEL>& ptr);

    /**
    * @class refCountBase
    * @brief Base class for reference counting metadata
    * @details Stores reference counts and provides interface for pointer management
    */
    class refCountBase {
        template <typename, typename, typename>
        friend class autoPtr;

        protected:
        mutable u_integer strong_refs; ///< Strong reference counter
        mutable u_integer weak_refs;   ///< Weak reference counter

        /**
        * @brief Construct refCountBase object
        */
        refCountBase();

        /**
        * @brief Get managed pointer (const version)
        * @return Const pointer to managed object
        */
        virtual const void* getPtr() const noexcept = 0;

        /**
        * @brief Get managed pointer
        * @return Pointer to managed object
        */
        virtual void* getPtr() noexcept = 0;

        /**
        * @brief Release ownership of the managed pointer
        * @return Raw pointer to the managed object
        * @note The caller becomes responsible for deleting the returned pointer
        */
        virtual void* releasePtr() noexcept = 0;

        /**
        * @brief Destroy managed pointer using deleter
        */
        virtual void destroyPtr() noexcept = 0;

        virtual ~refCountBase() = default;
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
    class refCount final : public refCountBase {
        template <typename, typename, typename>
        friend class autoPtr;

        TYPE* ptr;             ///< Managed raw pointer
        DELETER deleter;       ///< Deleter policy instance

        /**
        * @brief Construct refCount object
        * @param p Pointer to manage (nullptr allowed)
        */
        explicit refCount(TYPE* p = nullptr);

        const void* getPtr() const noexcept override;

        void* getPtr() noexcept override;

        void* releasePtr() noexcept override;

        /**
        * @brief Destroy managed pointer using deleter
        */
        void destroyPtr() noexcept override;

        /**
        * @brief Destructor ensures resource cleanup
        */
        ~refCount() override;
    };
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::autoPtr(TYPE* p)
    : ref_count(newRefCount(p)), alias_ptr(nullptr) {}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::addStrongRef() const
{
    this->ref_count->strong_refs += 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::addWeakRef() const
{
    this->ref_count->weak_refs += 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::removeStrongRef() const
{
    this->ref_count->strong_refs -= 1;
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::removeWeakRef() const
{
    this->ref_count->weak_refs -= 1;
}

template <typename TYPE, typename DERIVED, typename DELETER>
TYPE* original::autoPtr<TYPE, DERIVED, DELETER>::releasePtr() noexcept
{
    return static_cast<TYPE*>(this->ref_count->releasePtr());
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

template <typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::operator!() const {
    return !this->operator bool();
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE* original::autoPtr<TYPE, DERIVED, DELETER>::get() const {
    if (!this->exist()){
        throw nullPointerError();
    }
    if (this->alias_ptr) {
        return this->alias_ptr;
    }
    return static_cast<TYPE*>(this->ref_count->getPtr());
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE* original::autoPtr<TYPE, DERIVED, DELETER>::get() {
    if (!this->exist()){
        throw nullPointerError();
    }
    if (this->alias_ptr) {
        return this->alias_ptr;
    }
    return static_cast<TYPE*>(this->ref_count->getPtr());
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator*() const {
    const auto ptr = this->get();
    if (!ptr)
        throw nullPointerError();
    return *ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE*
original::autoPtr<TYPE, DERIVED, DELETER>::operator->() const {
    const auto ptr = this->get();
    if (!ptr)
        throw nullPointerError();
    return ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
const TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator[](u_integer index) const {
    const auto ptr = this->get();
    if (!ptr)
        throw nullPointerError();
    return ptr[index];
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE &original::autoPtr<TYPE, DERIVED, DELETER>::operator*() {
    auto ptr = this->get();
    if (!ptr)
        throw nullPointerError();
    return *ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE*
original::autoPtr<TYPE, DERIVED, DELETER>::operator->() {
    auto ptr = this->get();
    if (!ptr)
        throw nullPointerError();
    return ptr;
}

template<typename TYPE, typename DERIVED, typename DELETER>
TYPE& original::autoPtr<TYPE, DERIVED, DELETER>::operator[](u_integer index) {
    auto ptr = this->get();
    if (!ptr)
        throw nullPointerError();
    return ptr[index];
}

template<typename TYPE, typename DERIVED, typename DELETER>
void original::autoPtr<TYPE, DERIVED, DELETER>::swap(autoPtr& other) noexcept {
    auto other_ref_count = other.ref_count;
    other.ref_count = this->ref_count;
    this->ref_count = other_ref_count;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::integer original::autoPtr<TYPE, DERIVED, DELETER>::compareTo(const autoPtr& other) const {
    return this->get() - other.get();
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
original::u_integer original::autoPtr<TYPE, DERIVED, DELETER>::toHash() const noexcept {
    return hash<TYPE>::hashFunc(this->get());
}

template<typename TYPE, typename DERIVED, typename DELETER>
bool original::autoPtr<TYPE, DERIVED, DELETER>::equals(const autoPtr& other) const noexcept {
    return *this == other;
}

template<typename TYPE, typename DERIVED, typename DELETER>
original::autoPtr<TYPE, DERIVED, DELETER>::~autoPtr() {
    this->clean();
}

template<typename T, typename DER, typename DEL>
bool original::operator==(const autoPtr<T, DER, DEL>& ptr, const std::nullptr_t&) {
    return !ptr.operator bool();
}

template<typename T, typename DER, typename DEL>
bool original::operator!=(const autoPtr<T, DER, DEL>& ptr, const std::nullptr_t&) {
    return ptr.operator bool();
}

template<typename T, typename DER, typename DEL>
bool original::operator==(const std::nullptr_t&, const autoPtr<T, DER, DEL>& ptr) {
    return !ptr.operator bool();
}

template<typename T, typename DER, typename DEL>
bool original::operator!=(const std::nullptr_t&, const autoPtr<T, DER, DEL>& ptr) {
    return ptr.operator bool();
}

inline original::refCountBase::refCountBase() : strong_refs(0), weak_refs(0) {}

template<typename TYPE, typename DELETER>
original::refCount<TYPE, DELETER>::refCount(TYPE *p)
    : ptr(p) {}

template <typename TYPE, typename DELETER>
const void* original::refCount<TYPE, DELETER>::getPtr() const noexcept
{
    return this->ptr;
}

template <typename TYPE, typename DELETER>
void* original::refCount<TYPE, DELETER>::getPtr() noexcept
{
    if constexpr (std::is_const_v<TYPE>) {
        return const_cast<std::remove_const_t<TYPE>*>(this->ptr);
    } else {
        return this->ptr;
    }
}

template <typename TYPE, typename DELETER>
void* original::refCount<TYPE, DELETER>::releasePtr() noexcept
{
    if constexpr (std::is_const_v<TYPE>) {
        auto p = const_cast<std::remove_const_t<TYPE>*>(this->ptr);
        this->ptr = nullptr;
        return p;
    } else {
        auto p = this->ptr;
        this->ptr = nullptr;
        return p;
    }
}

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
