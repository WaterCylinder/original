#ifndef OWNERPTR_H
#define OWNERPTR_H

#include <utility>
#include "autoPtr.h"
#include "deleter.h"

/**
* @file ownerPtr.h
* @brief Exclusive-ownership smart pointer implementation
* @details Provides move-only semantics for dynamic object ownership management.
* Enforces single ownership through deleted copy operations and supports custom deleters.
* Inherits reference counting infrastructure from autoPtr base class.
*/

namespace original {
    /**
    * @class ownerPtr
    * @tparam TYPE Managed object type
    * @tparam DELETER Deletion policy type (default: deleter<TYPE>)
    * @brief Unique ownership smart pointer with move semantics
    * @details Key features:
    * - Explicit ownership transfer through move operations
    * - Prohibited copy operations for ownership uniqueness
    * - RAII-compliant automatic resource management
    * - Factory functions for safe object creation
    * @extends autoPtr
    */
    template <typename TYPE, typename DELETER = deleter<TYPE>>
    class ownerPtr final : public autoPtr<TYPE, ownerPtr<TYPE, DELETER>, DELETER>{
    public:
        /**
        * @brief Construct from raw pointer
        * @param p Pointer to take ownership of
        * @warning Transfers ownership responsibility to ownerPtr
        */
        explicit ownerPtr(TYPE* p);

        /**
        * @brief In-place construction with arguments
        * @tparam Args Constructor argument types
        * @param args Arguments to forward to TYPE constructor
        * @note Allocates and owns new dynamic instance
        */
        template<typename... Args>
        explicit ownerPtr(Args&&... args);

        ownerPtr(const ownerPtr& other) = delete; ///< Copy construction prohibited
        ownerPtr& operator=(const ownerPtr& other) = delete; ///< Copy assignment prohibited

        /**
        * @brief Move constructor
        * @param other Source ownerPtr to transfer ownership from
        * @post other becomes empty
        */
        ownerPtr(ownerPtr&& other) noexcept;

        /**
        * @brief Move assignment operator
        * @param other Source ownerPtr to transfer ownership from
        * @return Reference to this ownerPtr
        * @post other becomes empty
        */
        ownerPtr& operator=(ownerPtr&& other) noexcept;

        /**
        * @brief Release ownership of managed object
        * @return Raw pointer to released resource
        * @post ownerPtr no longer manages the resource
        * @warning Caller assumes ownership responsibility
        */
        TYPE* unlock();

        /**
        * @brief Get class name identifier
        * @return Class name of ownerPtr
        */
        [[nodiscard]] std::string className() const override;

        /**
        * @brief Destructor releases owned resources
        */
        ~ownerPtr() override;

        // Factory friend functions
        template <typename T, typename DEL>
        friend ownerPtr<T, DEL> makeOwnerPtr();

        template <typename T, typename DEL>
        friend ownerPtr<T, DEL> makeOwnerPtr(u_integer size);
    };

    /**
    * @brief Factory function for single object ownership
    * @tparam T Object type to create
    * @tparam DEL Deletion policy type (default: deleter<T>)
    * @return ownerPtr Owning pointer to new instance
    */
    template <typename T, typename DEL = deleter<T>>
    ownerPtr<T, DEL> makeOwnerPtr();

    /**
    * @brief Create ownerPtr with array allocation
    * @tparam T Array element type
    * @tparam DEL Array deletion policy type (default: deleter<T[]>)
    * @param size Number of elements to allocate
    * @return Newly created array ownerPtr
    * @note Uses array new for allocation
    */
    template <typename T, typename DEL = deleter<T[]>>
    ownerPtr<T, DEL> makeOwnerPtr(u_integer size);


    // ----------------- Definitions of ownerPtr.h -----------------


    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>::ownerPtr(TYPE *p)
        : autoPtr<TYPE, ownerPtr, DELETER>(p) {
        this->addStrongRef();
    }

    template<typename TYPE, typename DELETER>
    template<typename ... Args>
    ownerPtr<TYPE, DELETER>::ownerPtr(Args&& ...args) : ownerPtr(static_cast<TYPE*>(nullptr)) {
        this->setPtr(new TYPE(std::forward<Args>(args)...));
    }

    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>::ownerPtr(ownerPtr &&other) noexcept : ownerPtr(static_cast<TYPE*>(nullptr)){
        this->operator=(std::move(other));
    }

    template<typename TYPE, typename DELETER>
    ownerPtr<TYPE, DELETER>& ownerPtr<TYPE, DELETER>::operator=(ownerPtr&& other) noexcept {
        if (this == &other)
            return *this;

        this->removeStrongRef();
        this->clean();
        this->ref_count = other.ref_count;
        other.ref_count = autoPtr<TYPE, ownerPtr, DELETER>::newRefCount();
        other.addStrongRef();
        return *this;
    }

    template<typename TYPE, typename DELETER>
    TYPE* ownerPtr<TYPE, DELETER>::unlock() {
        TYPE* p = this->get();
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

    template<typename T, typename DEL>
    ownerPtr<T, DEL> makeOwnerPtr(const u_integer size) {
        return ownerPtr<T, DEL>(new T[size]);
    }
}

#endif //OWNERPTR_H
