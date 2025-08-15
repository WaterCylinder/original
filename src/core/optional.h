#ifndef ORIGINAL_OPTIONAL_H
#define ORIGINAL_OPTIONAL_H
#include <utility>
#include "types.h"
#include "error.h"

/**
 * @file optional.h
 * @brief Type-safe optional value container
 * @details Provides an alternative<TYPE> class that can either contain a value of type TYPE
 * or be in an empty state (represented by original::none from types.h). This implementation
 * provides:
 * - Value semantics with proper construction/destruction
 * - Safe access operations with error checking
 * - In-place construction
 * - Move and copy operations
 *
 * Key features:
 * - Type-safe alternative to raw pointers for optional values
 * - No dynamic memory allocation
 * - Explicit empty state handling
 * - Integration with original::none from types.h
 */

namespace original {

    /**
     * @class alternative
     * @brief A type-safe container that may or may not contain a value
     * @tparam TYPE The type of value to store
     * @details This class provides a way to represent optional values without
     * using pointers. It can either contain a value of type TYPE or be empty.
     * The empty state is represented using original::none from types.h.
     *
     * Example usage:
     * @code
     * alternative<int> opt;          // Empty state
     * alternative<int> opt2(42);     // Contains value 42
     * if (opt2) {                    // Check if has value
     *     int val = *opt2;           // Access the value
     * }
     * @endcode
     */
    template<typename TYPE>
    class alternative {
        /**
         * @union storage
         * @brief Internal storage for either TYPE or none
         * @details Uses a union to store either the value type or none,
         * with proper construction/destruction handling.
         */
        union storage {
            TYPE type_;     ///< Storage for the contained value
            none none_;     ///< Storage for empty state (from types.h)

            /**
             * @brief Default constructor (initializes to none)
             */
            storage() noexcept;

            /// Default copy constructor
            storage(const storage& other) noexcept = default;

            /// Default copy assignment
            storage& operator=(const storage& other) noexcept = default;

            /// Destructor
            ~storage();
        };

        bool non_none_type_; ///< Flag indicating whether value is present
        storage val_;       ///< The actual storage

        /**
         * @brief Safely destroys the current contents
         * @note Properly calls destructor based on current state
         */
        void destroy() noexcept;

    public:
        /**
         * @brief Constructs an empty alternative
         * @post hasValue() == false
         */
        explicit alternative();

        /**
         * @brief Constructs an alternative containing a value
         * @tparam Args Argument types for TYPE construction
         * @param args Arguments to forward to TYPE constructor
         * @post hasValue() == true
         */
        template<typename... Args>
        explicit alternative(Args&&... args);

        /**
         * @brief Copy constructor
         * @param other Alternative to copy from
         * @post hasValue() == other.hasValue()
         */
        alternative(const alternative& other);

        /**
         * @brief Copy assignment
         * @param other Alternative to copy from
         * @return Reference to this alternative
         * @post hasValue() == other.hasValue()
         */
        alternative& operator=(const alternative& other);

        /**
         * @brief Move constructor
         * @param other Alternative to move from
         * @post other is left in valid but unspecified state
         */
        alternative(alternative&& other) noexcept;

        /**
         * @brief Move assignment
         * @param other Alternative to move from
         * @return Reference to this alternative
         * @post other is left in valid but unspecified state
         */
        alternative& operator=(alternative&& other) noexcept;

        /**
         * @brief Const value access
         * @return Const reference to contained value
         * @throws valueError if empty
         */
        const TYPE& operator*() const;

        /**
         * @brief Mutable value access
         * @return Reference to contained value
         * @throws valueError if empty
         */
        TYPE& operator*();

        /**
         * @brief Const member access
         * @return Const pointer to contained value
         * @throws valueError if empty
         */
        const TYPE* operator->() const;

        /**
         * @brief Mutable member access
         * @return Pointer to contained value
         * @throws valueError if empty
         */
        TYPE* operator->();

        /**
         * @brief Gets const pointer to value
         * @return Pointer to value or nullptr if empty
         */
        const TYPE* get() const;

        /**
         * @brief Gets mutable pointer to value
         * @return Pointer to value or nullptr if empty
         */
        TYPE* get();

        /**
         * @brief Resets to empty state
         * @post hasValue() == false
         */
        void reset() noexcept;

        /**
         * @brief Constructs value in-place
         * @tparam Args Argument types for TYPE construction
         * @param args Arguments to forward to TYPE constructor
         * @post hasValue() == true
         */
        template<typename... Args>
        void emplace(Args&&... args);

        /**
         * @brief Sets value by copy
         * @param t Value to copy
         * @post hasValue() == true
         */
        void set(const TYPE& t);

        /**
         * @brief Checks if contains a value
         * @return true if contains value, false if empty
         */
        explicit operator bool() const;

        /**
         * @brief Checks if contains a value
         * @return true if contains value, false if empty
         */
        [[nodiscard]] bool hasValue() const;

        /**
         * @brief Destructor
         * @note Properly destroys contained value if present
         */
        ~alternative();
    };
}

template<typename TYPE>
original::alternative<TYPE>::storage::storage() noexcept
{
    new(&this->none_) none{};
}

template<typename TYPE>
original::alternative<TYPE>::storage::~storage() {}

template<typename TYPE>
void original::alternative<TYPE>::destroy() noexcept {
    if (this->non_none_type_){
        this->val_.type_.~TYPE();
        this->non_none_type_ = false;
    } else {
        this->val_.none_.~none();
        this->non_none_type_ = true;
    }
}

template<typename TYPE>
original::alternative<TYPE>::alternative()
    : non_none_type_(false), val_() {}

template<typename TYPE>
template<typename... Args>
original::alternative<TYPE>::alternative(Args &&... args)
    : non_none_type_(true), val_() {
    new (&this->val_.type_) TYPE{ std::forward<Args>(args)... };
}

template<typename TYPE>
original::alternative<TYPE>::alternative(const alternative& other) {
    this->non_none_type_ = other.non_none_type_;
    if (other.non_none_type_) {
        new (&val_.type_) TYPE{ other.val_.type_ };
    } else {
        new (&val_.none_) none{};
    }
}

template<typename TYPE>
original::alternative<TYPE>&
original::alternative<TYPE>::operator=(const alternative& other) {
    if (this == &other)
        return *this;

    this->destroy();
    this->non_none_type_ = other.non_none_type_;
    if (other.non_none_type_) {
        new (&val_.type_) TYPE{ other.val_.type_ };
    } else {
        new (&val_.none_) none{};
    }
    return *this;
}

template<typename TYPE>
original::alternative<TYPE>::alternative(alternative&& other) noexcept {
    this->non_none_type_ = other.non_none_type_;
    if (this->non_none_type_){
        new (&val_.type_) TYPE{ std::move(other.val_.type_) };
    } else{
        new (&val_.none_) none{};
    }
}

template<typename TYPE>
original::alternative<TYPE>&
original::alternative<TYPE>::operator=(alternative&& other)  noexcept {
    if (this == &other)
        return *this;

    this->destroy();
    this->non_none_type_ = other.non_none_type_;
    if (this->non_none_type_){
        new (&val_.type_) TYPE{ std::move(other.val_.type_) };
    } else{
        new (&val_.none_) none{};
    }
    return *this;
}

template<typename TYPE>
const TYPE&
original::alternative<TYPE>::operator*() const {
    if (!this->non_none_type_)
        throw valueError("Dereferencing a original::none value");

    return this->val_.type_;
}

template<typename TYPE>
TYPE&
original::alternative<TYPE>::operator*() {
    if (!this->non_none_type_)
        throw valueError("Dereferencing a original::none value");

    return this->val_.type_;
}

template<typename TYPE>
const TYPE*
original::alternative<TYPE>::operator->() const {
    if (!this->non_none_type_)
        throw valueError("Accessing member of a original::none value");

    return &this->val_.type_;
}

template<typename TYPE>
TYPE*
original::alternative<TYPE>::operator->() {
    if (!this->non_none_type_)
        throw valueError("Accessing member of a original::none value");

    return &this->val_.type_;
}

template<typename TYPE>
const TYPE* original::alternative<TYPE>::get() const {
    return this->non_none_type_ ? &this->val_.type_ : nullptr;
}

template<typename TYPE>
TYPE* original::alternative<TYPE>::get() {
    return this->non_none_type_ ? &this->val_.type_ : nullptr;
}

template<typename TYPE>
void original::alternative<TYPE>::reset() noexcept {
    this->destroy();
}

template<typename TYPE>
template<typename... Args>
void original::alternative<TYPE>::emplace(Args &&... args) {
    this->destroy();
    this->non_none_type_ = true;
    new (&val_.type_) TYPE{ std::forward<Args>(args)... };
}

template<typename TYPE>
void original::alternative<TYPE>::set(const TYPE &t) {
    this->destroy();
    this->non_none_type_ = true;
    new (&val_.type_) TYPE{ t };
}

template<typename TYPE>
original::alternative<TYPE>::operator bool() const {
    return this->non_none_type_;
}

template <typename TYPE>
bool original::alternative<TYPE>::hasValue() const
{
    return this->operator bool();
}

template<typename TYPE>
original::alternative<TYPE>::~alternative() {
    this->destroy();
}

#endif //ORIGINAL_OPTIONAL_H
