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
        bool operator==(const iterator<TYPE>* other);
        bool operator!=(const iterator<TYPE>* other);
        bool hasNext();
        bool hasPrev();
        void next();
        void prev();
        TYPE get();
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
        return ptr_->getVal();
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
    bool original::iterator<TYPE>::operator==(const iterator<TYPE>* other) {
        return ptr_ == other->ptr_;
    }

    template<typename TYPE>
    bool original::iterator<TYPE>::operator!=(const iterator<TYPE>* other) {
        return ptr_ != other->ptr_;
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
    void original::iterator<TYPE>::next() {
        if (!this->isNull()) {
            ptr_ = ptr_->getPNext();
        }
    }

    template<typename TYPE>
    void original::iterator<TYPE>::prev() {
        if (!this->isNull()) {
            ptr_ = ptr_->getPPrev();
        }
    }

    template<typename TYPE>
    TYPE original::iterator<TYPE>::get(){
        if (this->isNull()){
            throw emptyPointerError();
        }
        return ptr_->getVal();
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
