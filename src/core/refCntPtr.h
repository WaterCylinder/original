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
        explicit refCntPtr(TYPE* p = std::nullptr_t{});
    public:
        using autoPtr<TYPE, DERIVED, DELETER>::operator==;
        using autoPtr<TYPE, DERIVED, DELETER>::operator!=;

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
        explicit strongPtr(TYPE* p = std::nullptr_t{});

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
        * @brief Resets the smart pointer and releases the managed object
        * @details Performs the following sequence of operations:
        * 1. Decrements the current strong reference count
        * 2. Cleans up the managed pointer and reference counter
        * 3. Creates a new reference counter and initializes strong references
        * @post Equivalent to replacing the current strongPtr with a newly constructed empty pointer
        */
        void reset() noexcept;

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

        template <typename T, typename DEL, typename... Args>
        friend strongPtr<T, DEL> makeStrongPtr(Args&&... args);

        template <typename T, typename DEL, typename... Args>
        friend strongPtr<T, DEL> makeStrongPtrArray(u_integer size, Args&&... args);
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

    public:

        /**
        * @brief Initialize empty weak reference
        */
        explicit weakPtr();

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
    * @brief Creates a new strongPtr managing a shared object
    * @tparam T Type of object to create and manage
    * @tparam DEL Deleter policy type (default: deleter<T>)
    * @tparam Args Argument types for object construction
    * @param args Arguments to forward to T's constructor
    * @return strongPtr<T, DEL> sharing ownership of the new object
    * @note Provides exception-safe object creation with shared ownership
    * @details The object will be destroyed when all strong references are released
    *
    * @code
    * // Create a strongPtr managing a new MyClass constructed with args
    * auto ptr = makeStrongPtr<MyClass>(arg1, arg2);
    * @endcode
    */
    template <typename T, typename DEL = deleter<T>, typename... Args>
    strongPtr<T, DEL> makeStrongPtr(Args&&... args);

    /**
    * @brief Creates a new strongPtr managing a shared array
    * @tparam T Type of array elements to create
    * @tparam DEL Deleter policy type (default: deleter<T[]>)
    * @tparam Args Argument types for array element initialization
    * @param size Number of elements in the array
    * @param args Arguments to forward to each element's constructor
    * @return strongPtr<T, DEL> sharing ownership of the new array
    * @note Provides exception-safe array creation with shared ownership
    * @details The array will be destroyed when all strong references are released
    *
    * @code
    * // Create a strongPtr managing a new MyClass[10] array
    * auto arr = makeStrongPtrArray<MyClass>(10);
    * @endcode
    */
    template <typename T, typename DEL = deleter<T[]>, typename... Args>
    strongPtr<T, DEL> makeStrongPtrArray(u_integer size, Args&&... args);


    // ----------------- Definitions of refCntPtr.h -----------------


    template<typename TYPE, typename DERIVED, typename DELETER>
    refCntPtr<TYPE, DERIVED, DELETER>::refCntPtr(TYPE *p)
        : autoPtr<TYPE, DERIVED, DELETER>::autoPtr(p) {}

    template<typename TYPE, typename DERIVED, typename DELETER>
    template<typename O_DERIVED>
    bool refCntPtr<TYPE, DERIVED, DELETER>::operator==(const refCntPtr<TYPE, O_DERIVED, DELETER>& other) const {
        return this->ref_count == other.ref_count;
    }

    template<typename TYPE, typename DERIVED, typename DELETER>
    template<typename O_DERIVED>
    bool refCntPtr<TYPE, DERIVED, DELETER>::operator!=(const refCntPtr<TYPE, O_DERIVED, DELETER>& other) const {
        return this->ref_count != other.ref_count;
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
    strongPtr<TYPE, DELETER>::strongPtr(const strongPtr& other) : strongPtr(){
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
    strongPtr<TYPE, DELETER>::strongPtr(strongPtr&& other) noexcept : strongPtr() {
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    void strongPtr<TYPE, DELETER>::reset() noexcept {
        this->removeStrongRef();
        this->clean();
        this->ref_count = autoPtr<TYPE, strongPtr, DELETER>::newRefCount();
        this->addStrongRef();
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

    template<typename T, typename DEL, typename ... Args>
    strongPtr<T, DEL> makeStrongPtr(Args &&...args) {
        return strongPtr<T, DEL>(new T(std::forward<Args>(args)...));
    }

    template<typename T, typename DEL, typename ... Args>
    strongPtr<T, DEL> makeStrongPtrArray(const u_integer size, Args &&...args) {
        auto strong_ptr = strongPtr<T, DEL>(new T[size]);
        for (u_integer i = 0; i < size; i++)
        {
            strong_ptr[i] = T(std::forward<Args>(args)...);
        }
        return strong_ptr;
    }

    template<typename TYPE, typename DELETER>
    weakPtr<TYPE, DELETER>::weakPtr()
        : refCntPtr<TYPE, weakPtr, DELETER>() {
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
    TYPE& weakPtr<TYPE, DELETER>::operator[](u_integer index) {
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
