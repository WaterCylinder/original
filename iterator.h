#ifndef ITERATOR_H
#define ITERATOR_H

#include "printable.h"
#include "wrapper.h"

namespace original{
    template<typename TYPE>
    class iterator : public printable{
    protected:
        wrapper<TYPE>* ptr_;
    public:
        explicit iterator(wrapper<TYPE>* wrapper);
        iterator(iterator<TYPE>& other);
        TYPE operator*();
        iterator& operator=(const iterator<TYPE>* other);
        iterator& operator=(const TYPE& data);
        iterator& operator++();
        iterator operator++(int);
        bool operator==(const iterator<TYPE>& other);
        bool operator!=(const iterator<TYPE>& other);
        bool hasNext();
        bool hasPrev();
        bool atLeft(const iterator<TYPE>* other);
        bool atRight(const iterator<TYPE>* other);
        void next();
        void prev();
        TYPE& get();
        void set(TYPE data);
        bool equal(const iterator<TYPE>* other);
        bool isNull();
        std::string toString(bool enter) override;
        virtual ~iterator() = default;
    };
}

    template<typename TYPE>
    original::iterator<TYPE>::iterator(wrapper<TYPE>* wrapper) : ptr_(wrapper) {}

    template<typename TYPE>
    original::iterator<TYPE>::iterator(iterator<TYPE>& other)
        : ptr_(other.ptr_) {}

    template<typename TYPE>
    TYPE original::iterator<TYPE>::operator*() {
        return this->get();
    }

    template<typename TYPE>
    original::iterator<TYPE>& original::iterator<TYPE>::operator=(const iterator<TYPE>* other) {
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
    bool original::iterator<TYPE>::operator==(const iterator<TYPE>& other) {
        return this->equal(&other);
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::operator!=(const iterator<TYPE>& other) {
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
    bool original::iterator<TYPE>::atLeft(const iterator<TYPE> *other) {
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->getPNext() == other->ptr_;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::atRight(const iterator<TYPE> *other) {
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
    void original::iterator<TYPE>::set(TYPE data){
        if (this->isNull()){
            throw nullPointerError();
        }
        return ptr_->setVal(data);
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::equal(const iterator<TYPE> *other) {
        return this->ptr_ == other->ptr_;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::isNull(){
        return ptr_ == nullptr;
    }

    template<typename TYPE>
    std::string original::iterator<TYPE>::toString(bool enter){
        std::stringstream ss;
        ss << "iterator" << "(" << "#" << this->ptr_ << ")";
        if (enter){
            ss << "\n";
        }
        return ss.str();
    }

#endif //ITERATOR_H
