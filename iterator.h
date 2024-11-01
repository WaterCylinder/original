#ifndef ITERATOR_H
#define ITERATOR_H

#include "printable.h"
#include "wrapper.h"

namespace original{
    template<typename TYPE>
    class iterator final : public printable{
    protected:
        wrapper<TYPE>* ptr_;
    public:
        explicit iterator(wrapper<TYPE>* wrapper);
        iterator(const iterator& other);
        TYPE& operator*();
        const TYPE& operator*() const;
        iterator& operator=(const iterator* other);
        iterator& operator=(const TYPE& data);
        iterator& operator++();
        iterator operator++(int);
        bool operator==(const iterator& other);
        bool operator!=(const iterator& other);
        bool hasNext();
        bool hasPrev();
        bool atPrev(const iterator* other);
        bool atNext(const iterator* other);
        void next();
        void prev();
        TYPE& get();
        const TYPE& get() const;
        void set(TYPE data);
        bool equal(const iterator* other);
        bool isNull();
        std::string toString(bool enter) override;
        ~iterator() override = default;
    };
}

    template<typename TYPE>
    original::iterator<TYPE>::iterator(wrapper<TYPE>* wrapper) : ptr_(wrapper) {}

    template<typename TYPE>
    original::iterator<TYPE>::iterator(const iterator& other)
        : ptr_(other.ptr_) {}

    template<typename TYPE>
    TYPE& original::iterator<TYPE>::operator*() {
        return this->get();
    }

    template<typename TYPE>
    const TYPE& original::iterator<TYPE>::operator*() const{
        return this->get();
    }

    template<typename TYPE>
    original::iterator<TYPE>& original::iterator<TYPE>::operator=(const iterator* other) {
        if (this == &other) {
            return *this;
        }
        ptr_ = other->ptr_;
        return *this;
    }

    template<typename TYPE>
    original::iterator<TYPE>& original::iterator<TYPE>::operator=(const TYPE& data) {
        this->set(data);
        return *this;
    }

    template<typename TYPE>
    original::iterator<TYPE>& original::iterator<TYPE>::operator++(){
        this->next();
        return *this;
    }

    template<typename TYPE>
    original::iterator<TYPE> original::iterator<TYPE>::operator++(int){
        iterator next = *this;
        next();
        return next;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::operator==(const iterator& other) {
        return this->equal(&other);
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::operator!=(const iterator& other) {
        return !this->equal(&other);
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::hasNext() {
        if (this->isNull()){
            return false;
        }
        return ptr_->getPNext() != nullptr;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::hasPrev() {
        if (this->isNull()){
            return false;
        }
        return ptr_->getPPrev() != nullptr;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::atPrev(const iterator *other) {
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->getPNext() == other->ptr_;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::atNext(const iterator *other) {
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->getPPrev() == other->ptr_;
    }

    template<typename TYPE>
    void original::iterator<TYPE>::next() {
        if (this->isNull()){
            throw nullPointerError();
        }
        ptr_ = ptr_->getPNext();
    }

    template<typename TYPE>
    void original::iterator<TYPE>::prev() {
        if (this->isNull()){
            throw nullPointerError();
        }
        ptr_ = ptr_->getPPrev();
    }

    template<typename TYPE>
    TYPE& original::iterator<TYPE>::get(){
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->getVal();
    }

    template<typename TYPE>
    const TYPE& original::iterator<TYPE>::get() const{
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->getVal();
    }

    template<typename TYPE>
    void original::iterator<TYPE>::set(TYPE data){
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->setVal(data);
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::equal(const iterator *other) {
        return this->ptr_ == other->ptr_;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::isNull(){
        return ptr_ == nullptr;
    }

    template<typename TYPE>
    std::string original::iterator<TYPE>::toString(const bool enter){
        std::stringstream ss;
        ss << "iterator" << "(" << "#" << this->ptr_ << ")";
        if (enter){
            ss << "\n";
        }
        return ss.str();
    }

#endif //ITERATOR_H
