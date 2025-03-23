#ifndef REFCNTPTR_H
#define REFCNTPTR_H

#include "autoPtr.h"
#include "deleter.h"

/**
* @file refCntPtr.h
* @brief Reference-counted smart pointer hierarchy
* @details Implements shared ownership semantics through strong/weak reference models.
* Base class provides common reference counting infrastructure, while derived classes
* implement specific ownership policies. Supports cyclic reference breaking through
* weak reference design.
*/


namespace original{
    /**
    * @class refCntPtr
    * @tparam TYPE Managed object type
    * @tparam DERIVED CRTP pattern parameter
    * @tparam DELETER Custom deletion policy type
    * @brief Base class for reference-counted pointers
    * @details Provides shared infrastructure for:
    * - Reference counting mechanics
    * - Object lifetime tracking
    * - Common operator implementations
    * @extends autoPtr
    */
    template<typename TYPE, typename DERIVED, typename DELETER>
    class refCntPtr : public autoPtr<TYPE, DERIVED, DELETER>{
    protected:
        /**
        * @brief Construct from raw pointer
        * @param p Pointer to manage
        * @note Initializes reference counting system
        */
        explicit refCntPtr(TYPE* p);
    public:

        /**
        * @brief Equality comparison operator
        * @tparam O_DERIVED Other derived class type (strongPtr/weakPtr)
        * @param other Reference-counted pointer to compare with
        * @return bool True if both pointers share the same reference counter
        */
        template<typename O_DERIVED>
        bool operator==(const refCntPtr<TYPE, O_DERIVED, DELETER>& other) const;

        /**
        * @brief Inequality comparison operator
        * @tparam O_DERIVED Other derived class type (strongPtr/weakPtr)
        * @param other Reference-counted pointer to compare with
        * @return bool True if pointers use different reference counters
        */
        template<typename O_DERIVED>
        bool operator!=(const refCntPtr<TYPE, O_DERIVED, DELETER>& other) const;

        /**
        * @brief Get class identifier
        * @return Class name of refCntPtr
        */
        [[nodiscard]] std::string className() const override;

        /**
        * @brief Formatted string with reference info
        * @param enter Add newline if true
        * @return Contains pointer value and ref counts
        */
        [[nodiscard]] std::string toString(bool enter) const override;

        /**
        * @brief Default destructor of refCntPtr
        */
        ~refCntPtr() override = default;
    };

    template<typename TYPE, typename DELETER = deleter<TYPE>>
    class strongPtr;

    template<typename TYPE, typename DELETER = deleter<TYPE>>
    class weakPtr;


    /**
    * @class strongPtr
    * @tparam TYPE Managed object type
    * @tparam DELETER Deletion policy type (default: deleter<TYPE>)
    * @brief Shared ownership smart pointer with strong references
    * @details Maintains object lifetime through reference counting:
    * - Increases strong count on copy
    * - Decreases strong count on destruction
    * - Destroys object when strong count reaches zero
    * - Supports copy/move semantics for shared ownership
    * @extends refCntPtr
    */
    template<typename TYPE, typename DELETER>
    class strongPtr final : public refCntPtr<TYPE, strongPtr<TYPE, DELETER>, DELETER>{
        friend class weakPtr<TYPE, DELETER>;

    public:
        /**
        * @brief Construct from raw pointer
        * @param p Pointer to manage
        * @warning Shares ownership of existing resource
        */
        explicit strongPtr(TYPE* p);

        /**
        * @brief In-place construction with arguments
        * @tparam Args Constructor argument types
        * @param args Arguments to forward to TYPE constructor
        */
        template<typename... Args>
        explicit strongPtr(Args&&... args);

        /**
        * @brief Copy constructor shares ownership
        * @param other Source strongPtr to copy from
        */
        strongPtr(const strongPtr& other);

        /**
        * @brief Copy assignment shares ownership
        * @param other Source strongPtr to copy from
        * @return Reference to this strongPtr
        */
        strongPtr& operator=(const strongPtr& other);

        /**
        * @brief Move constructor transfers ownership
        * @param other Source strongPtr to move from
        * @post other becomes empty
        */
        strongPtr(strongPtr&& other) noexcept;

        /**
        * @brief Move assignment transfers ownership
        * @param other Source strongPtr to move from
        * @return Reference to this strongPtr
        * @post other becomes empty
        */
        strongPtr& operator=(strongPtr&& other) noexcept;

        /**
        * @brief Get class identifier
        * @return Class name of strongPtr
        */
        [[nodiscard]] std::string className() const override;

        /**
        * @brief Destructor decreases strong references count
        */
        ~strongPtr() override;

        template <typename T, typename DEL>
        friend strongPtr<T, DEL> makeStrongPtr();

        template <typename T, typename DEL>
        friend strongPtr<T, DEL> makeStrongPtr(u_integer size);
    };

    /**
    * @class weakPtr
    * @tparam TYPE Managed object type
    * @tparam DELETER Deletion policy type (default: deleter<TYPE>)
    * @brief Non-owning reference to shared resource
    * @details Provides safe access to resources managed by strongPtr:
    * - Does not affect object lifetime
    * - Must convert to strongPtr via lock() before access
    * - Automatically expires when all strong references removed
    * @extends refCntPtr
    */
    template<typename TYPE, typename DELETER>
    class weakPtr final : public refCntPtr<TYPE, weakPtr<TYPE, DELETER>, DELETER>{
        friend class strongPtr<TYPE, DELETER>;

        /**
        * @brief Initialize empty weak reference
        */
        explicit weakPtr();
    public:

        /**
        * @brief Construct from strongPtr observer
        * @param other Source strongPtr to observe
        * @details Creates weak reference to same resource:
        * - Shares reference counter with strongPtr
        * - Increments weak reference count
        * - Does NOT affect strong reference count
        */
        explicit weakPtr(const strongPtr<TYPE, DELETER>& other);

        /**
        * @brief Assign observation from strongPtr
        * @param other Source strongPtr to observe
        * @return Reference to this weakPtr
        * @details Behavior:
        * 1. Releases current observation (decrements weak count)
        * 2. Adopts new reference counter from other
        * 3. Increments new weak reference count
        * @note Handles self-assignment safely
        */
        weakPtr& operator=(const strongPtr<TYPE, DELETER>& other);

        /**
        * @brief Copy constructor duplicates observation
        * @param other Source weakPtr to copy
        * @details Shares the same observation target:
        * - Increments weak reference count
        * - No change to strong reference count
        */
        weakPtr(const weakPtr& other);

        /**
        * @brief Copy assignment duplicates observation
        * @param other Source weakPtr to copy
        * @return Reference to this weakPtr
        * @details Behavior:
        * 1. Releases current observation
        * 2. Shares other's reference counter
        * 3. Increments new weak reference count
        * @note Safely handles self-assignment
        */
        weakPtr& operator=(const weakPtr& other);

        /**
        * @brief Move constructor transfers observation
        * @param other Source weakPtr to move from
        * @post other becomes empty observer
        */
        weakPtr(weakPtr&& other) noexcept;

        /**
        * @brief Move assignment transfers observation
        * @param other Source weakPtr to move from
        * @return Reference to this weakPtr
        * @post other becomes empty observer
        */
        weakPtr& operator=(weakPtr&& other) noexcept;

        /**
        * @brief Attempt to acquire ownership
        * @return strongPtr<TYPE, DELETER> Valid strongPtr if resource exists
        * @details Creates temporary strong reference:
        * - Returns empty strongPtr if object destroyed
        * - Increments strong count if successful
        * - Thread-safe atomic reference check
        * @throws nothing (nothrow guarantee)
        */
        strongPtr<TYPE, DELETER> lock() const;

        /**
        * @brief Const dereference via temporary strong reference
        * @return const TYPE& Reference to observed object
        */
        const TYPE& operator*() const override;

        /**
        * @brief Const member access via temporary strong reference
        * @return Pointer to observed object
        */
        const TYPE* operator->() const override;

        /**
        * @brief Const array element access via temporary strong reference
        * @param index Array position
        * @return Reference to element
        */
        const TYPE& operator[](u_integer index) const override;

        /**
        * @brief Mutable dereference via temporary strong reference
        * @return Reference to observed object
        */
        TYPE& operator*() override;

        /**
        * @brief Mutable member access via temporary strong reference
        * @return Pointer to observed object
        */
        TYPE* operator->() override;

        /**
        * @brief Mutable array element access via temporary strong reference
        * @param index Array position
        * @return Reference to element
        */
        TYPE& operator[](u_integer index) override;

        /**
        * @brief Get class type identifier
        * @return std::string "weakPtr"
        */
        [[nodiscard]] std::string className() const override;

        /**
        * @brief Destructor releases weak reference
        * @details Decrements weak reference count:
        * - Does NOT trigger object destruction
        * - Final weak reference cleans up counter
        */
        ~weakPtr() override;
    };

    /**
    * @brief Factory for default-constructed strongPtr
    * @tparam T Object type
    * @tparam DEL Deletion policy (default: deleter<T>)
    * @return New shared ownership instance
    */
    template <typename T, typename DEL = deleter<T>>
    strongPtr<T, DEL> makeStrongPtr();

    /**
    * @brief Factory for array-allocated strongPtr
    * @tparam T Array element type
    * @tparam DEL Array deletion policy (default: deleter<T[]>)
    * @param size Number of elements
    * @return Shared array ownership
    */
    template <typename T, typename DEL = deleter<T[]>>
    strongPtr<T, DEL> makeStrongPtr(u_integer size);


    // ----------------- Definitions of refCntPtr.h -----------------


    template<typename TYPE, typename DERIVED, typename DELETER>
    refCntPtr<TYPE, DERIVED, DELETER>::refCntPtr(TYPE *p)
        : autoPtr<TYPE, DERIVED, DELETER>::autoPtr(p) {}

    template<typename TYPE, typename DERIVED, typename DELETER>
    template<typename O_DERIVED>
    bool refCntPtr<TYPE, DERIVED, DELETER>::operator==(const refCntPtr<TYPE, O_DERIVED, DELETER>& other) const {
        return this->get() == other.get();
    }

    template<typename TYPE, typename DERIVED, typename DELETER>
    template<typename O_DERIVED>
    bool refCntPtr<TYPE, DERIVED, DELETER>::operator!=(const refCntPtr<TYPE, O_DERIVED, DELETER>& other) const {
        return this->get() != other.get();
    }

    template<typename TYPE, typename DERIVED, typename DELETER>
    std::string refCntPtr<TYPE, DERIVED, DELETER>::className() const {
        return "refCntPtr";
    }

    template<typename TYPE, typename DERIVED, typename DELETER>
    std::string refCntPtr<TYPE, DERIVED, DELETER>::toString(const bool enter) const {
        std::stringstream ss;
        ss << this->className() << "(";
        ss << printable::formatString(this->get()) << ", ";
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
    template<typename ... Args>
    strongPtr<TYPE, DELETER>::strongPtr(Args &&...args) : strongPtr(static_cast<TYPE*>(nullptr)) {
        this->setPtr(new TYPE(std::forward<Args>(args)...));
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(const strongPtr& other) : strongPtr(static_cast<TYPE*>(nullptr)){
        this->operator=(other);
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>& strongPtr<TYPE, DELETER>::operator=(const strongPtr& other) {
        if (this == &other || *this == other)
            return *this;

        this->removeStrongRef();
        this->clean();
        this->ref_count = other.ref_count;
        this->addStrongRef();
        return *this;
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>::strongPtr(strongPtr&& other) noexcept : strongPtr(static_cast<TYPE*>(nullptr)) {
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    strongPtr<TYPE, DELETER>& strongPtr<TYPE, DELETER>::operator=(strongPtr&& other) noexcept {
        if (this == &other || *this == other)
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

    template<typename T, typename DEL>
    strongPtr<T, DEL> makeStrongPtr(const u_integer size) {
        return strongPtr<T, DEL>(new T[size]);
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr()
        : refCntPtr<TYPE, weakPtr, DELETER>(static_cast<TYPE*>(nullptr)) {
        this->addWeakRef();
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr(const strongPtr<TYPE, DELETER>& other)
        : weakPtr() {
        this->operator=(other);
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>& weakPtr<TYPE, DELETER>::operator=(const strongPtr<TYPE, DELETER>& other) {
        if (*this == other)
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
        if (this == &other || *this == other)
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
        if (this == &other || *this == other)
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
        strongPtr<TYPE, DELETER> strong_ptr{static_cast<TYPE*>(nullptr)};
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
    const TYPE& weakPtr<TYPE, DELETER>::operator[](u_integer index) const {
        return this->lock().operator[](index);
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
    TYPE & weakPtr<TYPE, DELETER>::operator[](u_integer index) {
        return this->lock().operator[](index);
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
