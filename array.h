#ifndef ARRAY_H
#define ARRAY_H
#pragma once


#include <initializer_list>
#include <sstream>
#include "error.h"

namespace original{

    template<typename TYPE>
    class array{
        size_t size_;
        TYPE* body;

        bool indexOutOfBound(int index);

        public:
        explicit array(size_t size);
        array(std::initializer_list<TYPE> lst);
        array(const array& other);
        array& operator=(const array& other);
        array(array&& other) _GLIBCXX_NOEXCEPT;

        ~array();

        _GLIBCXX_NODISCARD size_t size() const;
        TYPE get(int index);
        void set(int index, TYPE e);
        std::string toString(bool enter);
    };

}

    template <typename TYPE>
    bool original::array<TYPE>::indexOutOfBound(int index){
        return index < 0 || index >= this->size_;
    }

    template <typename TYPE>
    original::array<TYPE>::array(size_t size){
        this->size_ = size;
        this->body = this->body = new TYPE[this->size_];
    }

    template <typename TYPE>
    original::array<TYPE>::array(std::initializer_list<TYPE> lst){
        this->size_ = lst.size();
        this->body = new TYPE[this->size_];
        size_t i = 0;
        for (auto e : lst){
            this->body[i] = e;
            i += 1;
        }
    }

template <typename TYPE>
    original::array<TYPE>::array(const array& other) {
        this->size_ = other.size_;
        this->body = new TYPE[this->size_];
        for (size_t i = 0; i < this->size_; i++) {
            this->body[i] = other.body[i];
        }
    }

    template <typename TYPE>
    original::array<TYPE>& original::array<TYPE>::operator=(const array& other){
        if (this == &other) return *this;

        delete[] this->body;

        this->size_ = other.size_;
        this->body = new TYPE[this->size_];
        for (size_t i = 0; i < this->size_; i++) {
            this->body[i] = other.body[i];
        }
        return *this;
    }

    template <typename TYPE>
    original::array<TYPE>::array(array&& other) _GLIBCXX_NOEXCEPT{
        this->size_ = other.size_;
        this->body = other.body;
        other.size_ = 0;
        other.body = nullptr;
    }

    template <typename TYPE>
    original::array<TYPE>::~array() {
        delete[] this->body;
    }

    template <typename TYPE>
    size_t original::array<TYPE>::size() const{
        return this->size_;
    }

    template <typename TYPE>
    TYPE original::array<TYPE>::get(int index){
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        return this->body[index];
    }

    template <typename TYPE>
    void original::array<TYPE>::set(int index, TYPE e){
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        this->body[index] = e;
    }

    template <typename TYPE>
    std::string original::array<TYPE>::toString(const bool enter = false){
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < this->size_; ++i) {
            ss << this->body[i];
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

#endif //ARRAY_H
