#ifndef CHAIN_H
#define CHAIN_H
#pragma once


#include <sstream>

#include "array.h"


namespace original
{
    template <typename TYPE>
    class chain{

        class chainNode{
                const TYPE data;
                chainNode* prev;
                chainNode* next;
            protected:
                chainNode(TYPE data, chainNode* prev, chainNode* next);
                chainNode(const chainNode& other);
                chainNode& operator=(const chainNode& other);
                chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT;

                TYPE getVal();
                chainNode* getPPrev();
                chainNode* getPNext();
                void setPPrev(chainNode* new_prev);
                void setPNext(chainNode* new_next);
                _GLIBCXX_NODISCARD std::string toString() const;
        };

        size_t size;
        chainNode* begin;
        chainNode end;

    public:
        explicit chain();
        chain(std::initializer_list<TYPE> list);
        explicit chain(array<TYPE> arr);
        //todo
    };

}

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(const TYPE data, chainNode* prev = nullptr, chainNode* next = nullptr){
        this.data = data;
        this.prev = prev;
        this.next = next;
    }

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(const chainNode& other){
        this.data = other.data;
        this.prev = other.prev;
        this.next = other.next;
    }

    template <typename TYPE>
    typename original::chain<TYPE>::chainNode&
        original::chain<TYPE>::chainNode::operator=(const chainNode& other){
        if (this == other) return *this;

        this.data = other.data;
        this.prev = other.prev;
        this.next = other.next;
        return *this;
    }

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT{
        this.data = other.data;
        this.prev = other.prev;
        this.next = other.next;

        other.prev = nullptr;
        other.next = nullptr;
    }

    template <typename TYPE>
    TYPE original::chain<TYPE>::chainNode::getVal(){
        return this.data;
    }

    template <typename TYPE>
    original::chain<TYPE>::template chainNode<TYPE>* original::chain<TYPE>::chainNode::getPPrev(){
        return this.prev;
    }

    template <typename TYPE>
    original::chain<TYPE>::template chainNode<TYPE>* original::chain<TYPE>::chainNode::getPNext(){
        return this.next;
    }

    template <typename TYPE>
    void original::chain<TYPE>::chainNode::setPPrev(chainNode* new_prev){
        this.prev = new_prev;
    }

    template <typename TYPE>
    void original::chain<TYPE>::chainNode::setPNext(chainNode* new_next){
        this.next = new_next;
    }

    template <typename TYPE>
    std::string original::chain<TYPE>::chainNode::toString() const{
        std::stringstream ss;
        ss << this.data << ",";
        return ss.str();
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(){
        chainNode* dump = chainNode<TYPE>();
        this->begin = dump;
        this->end = dump;
        this->size = 0;
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(std::initializer_list<TYPE> list){
        chainNode* dump = chainNode<TYPE>();
        this->begin = dump;
        this->end = dump;
        this->size = 0;
        for (auto e: list){
            chainNode* cur_node = chainNode<TYPE>(e, this->end);
            this->end.setPNext(cur_node);
            this->end = cur_node;
            this->size += 1;
        }
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(array<TYPE> arr){
        chainNode* dump = chainNode<TYPE>();
        this->begin = dump;
        this->end = dump;
        this->size = 0;
        for (size_t i = 0; i < arr.size(); i++){
            chainNode* cur_node = chainNode<TYPE>(arr.get(i), this->end);
            this->end.setPNext(cur_node);
            this->end = cur_node;
            this->size += 1;
        }
    }

#endif //CHAIN_H
