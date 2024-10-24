#ifndef ARRAY_H
#define ARRAY_H
#include <initializer_list>
#include <sstream>
#include "error.h"

namespace original{

    template<typename TYPE>
    class array{
        size_t size_;
        TYPE* body_;

        bool indexOutOfBound(int index);

        public:
        explicit array(int size);
        array(std::initializer_list<TYPE> lst);
        array(const array& other);
        array& operator=(const array& other);
        array(array&& other) noexcept;

        ~array();

        _GLIBCXX_NODISCARD size_t size() const;
        TYPE get(int index);
        void set(int index, TYPE e);
        std::string toString(bool enter = false);
    };

}

    template <typename TYPE>
    bool original::array<TYPE>::indexOutOfBound(int index){
        return index < 0 || index >= this->size_;
    }

    template <typename TYPE>
    original::array<TYPE>::array(int size){
        this->size_ = size;
        this->body_ = this->body_ = new TYPE[this->size_];
    }

    template <typename TYPE>
    original::array<TYPE>::array(std::initializer_list<TYPE> lst){
        this->size_ = lst.size();
        this->body_ = new TYPE[this->size_];
        size_t i = 0;
        for (auto e : lst){
            this->body_[i] = e;
            i += 1;
        }
    }

    template <typename TYPE>
    original::array<TYPE>::array(const array& other) {
        this->size_ = other.size_;
        this->body_ = new TYPE[this->size_];
        for (size_t i = 0; i < this->size_; i++) {
            this->body_[i] = other.body_[i];
        }
    }

    template <typename TYPE>
    original::array<TYPE>& original::array<TYPE>::operator=(const array& other){
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
    original::array<TYPE>::array(array&& other) _GLIBCXX_NOEXCEPT{
        this->size_ = other.size_;
        this->body_ = other.body_;
        other.size_ = 0;
        other.body_ = nullptr;
    }

    template <typename TYPE>
    original::array<TYPE>::~array() {
        delete[] this->body_;
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
        return this->body_[index];
    }

    template <typename TYPE>
    void original::array<TYPE>::set(int index, TYPE e){
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        this->body_[index] = e;
    }

    template <typename TYPE>
    std::string original::array<TYPE>::toString(const bool enter){
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

#endif //ARRAY_H
