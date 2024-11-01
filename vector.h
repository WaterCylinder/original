#ifndef VECTOR_H
#define VECTOR_H

#include "serial.h"
#include "iterable.h"
#include "array.h"

namespace original{
    template <typename TYPE>
    class vector final : public serial<TYPE>, public iterable<TYPE>, public printable{
        class vectorNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class vector;
            private:
                TYPE data_;
            protected:
                explicit vectorNode(TYPE data);
                TYPE& getVal() override;
                void setVal(TYPE data) override;
                vectorNode* getPPrev() override;
                vectorNode* getPNext() override;
                void setPPrev(vectorNode* new_prev);
                void setPNext(vectorNode* new_next);
                static void connect(vectorNode* prev, vectorNode* next);
        };

        size_t size_;
        size_t inner_size;
        size_t inner_begin;
        vectorNode** body;
//    public:
//        explicit vector();
//        vector(std::initializer_list<TYPE> list);
//        explicit vector(array<TYPE> arr);
//        _GLIBCXX_NODISCARD size_t size() const override;
//        TYPE get(int index) override;
//        TYPE operator[](int index) override;
//        void set(int index, TYPE e) override;
//        void pushBegin(TYPE e) override;
//        void push(int index, TYPE e) override;
//        void pushEnd(TYPE e) override;
//        TYPE popBegin() override;
//        TYPE pop(int index) override;
//        TYPE popEnd() override;
//        iterator<TYPE>* begins() override;
//        iterator<TYPE>* ends() override;
//        _GLIBCXX_NODISCARD std::string toString(bool enter) override;
//        ~vector() override;
    };
}

    template<typename TYPE>
    original::vector<TYPE>::vectorNode::vectorNode(TYPE data) : data_(data) {}

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getVal() -> TYPE&
    {
        return this->data_;
    }


    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::setVal(TYPE data) -> void
    {
        this->data_ = data;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getPPrev() -> vectorNode* {
        return this - 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getPNext() -> vectorNode* {
        return this + 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::setPPrev(vectorNode* new_prev) -> void {
        this->prev = new_prev;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::setPNext(vectorNode* new_next) -> void {
        this->next = new_next;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::connect(vectorNode* prev, vectorNode* next) -> void
    {
        prev->setPNext(next);
        next->setPPrev(prev);
    }

#endif //VECTOR_H
