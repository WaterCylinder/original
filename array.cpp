#include "array.h"
#include "error.h"
#include <cstdlib>
#include "sstream"


namespace original{

    template <typename TYPE>
    bool array<TYPE>::indexOutOfBound(int index){
        return index < 0 || index >= this->size_;
    }

    template <typename TYPE>
    array<TYPE>::array(int size){
        this->size_ = size;
        this->body_ = this->body_ = new TYPE[this->size_];
    }

    template <typename TYPE>
    array<TYPE>::array(std::initializer_list<TYPE> lst){
        this->size_ = lst.size();
        this->body_ = new TYPE[this->size_];
        size_t i = 0;
        for (auto e : lst){
            this->body_[i] = e;
            i += 1;
        }
    }

    template <typename TYPE>
    array<TYPE>::array(const array& other) {
        this->size_ = other.size_;
        this->body_ = new TYPE[this->size_];
        for (size_t i = 0; i < this->size_; i++) {
            this->body_[i] = other.body_[i];
        }
    }

    template <typename TYPE>
    array<TYPE>& array<TYPE>::operator=(const array& other){
        if (this == &other) return *this;

        delete[] this->body_;

        this->size_ = other.size_;
        this->body_ = new TYPE[this->size_];
        for (size_t i = 0; i < this->size_; i++) {
            this->body_[i] = other.body_[i];
        }
        return *this;
    }

    template <typename TYPE>
    array<TYPE>::array(array&& other) noexcept{
        this->size_ = other.size_;
        this->body_ = other.body_;
        other.size_ = 0;
        other.body_ = nullptr;
    }

    template <typename TYPE>
    array<TYPE>::~array() {
        delete[] this->body_;
    }

    template <typename TYPE>
    size_t array<TYPE>::size() const
    {
        return this->size_;
    }

    template <typename TYPE>
    TYPE array<TYPE>::get(int index){
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        return this->body_[index];
    }

    template <typename TYPE>
    void array<TYPE>::set(int index, TYPE e){
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        this->body_[index] = e;
    }

    template <typename TYPE>
    std::string array<TYPE>::toString(const bool enter){
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < this->size_; ++i) {
            ss << this->body_[i];
            if (i < this->size_ - 1) {
                ss << ", ";
            }
        }
        ss << "]";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }
}
