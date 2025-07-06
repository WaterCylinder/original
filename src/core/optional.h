#ifndef ORIGINAL_OPTIONAL_H
#define ORIGINAL_OPTIONAL_H
#include <utility>
#include "types.h"
#include "error.h"


namespace original {

    template<typename TYPE>
    class alternative {
        union storage {
            TYPE type_;
            none none_;

            storage() noexcept;

            storage(const storage& other) noexcept = default;

            storage& operator=(const storage& other) noexcept = default;

            ~storage();
        };

        bool non_none_type_;
        storage val_;

        void destroy() noexcept;
    public:
        explicit alternative();

        template<typename... Args>
        explicit alternative(Args&&... args);

        alternative(const alternative& other);

        alternative& operator=(const alternative& other);

        alternative(alternative&& other) noexcept;

        alternative& operator=(alternative&& other) noexcept;

        const TYPE& operator*() const;

        TYPE& operator*();

        const TYPE* operator->() const;

        TYPE* operator->();

        const TYPE* get() const;

        TYPE* get();

        void reset() noexcept;

        template<typename... Args>
        void emplace(Args&&... args);

        void set(const TYPE& t);

        explicit operator bool() const;

        bool hasValue() const;

        ~alternative();
    };
}

template<typename TYPE>
original::alternative<TYPE>::storage::storage() noexcept
{
    new(&this->none_) none{};
}

template<typename TYPE>
original::alternative<TYPE>::storage::~storage() = default;

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
    this->val_.type_ = t;
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
