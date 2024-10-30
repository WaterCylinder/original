#ifndef CHAIN_H
#define CHAIN_H
#pragma once

#include "config.h"
#include <sstream>
#include "array.h"
#include "iterator.h"
#include "serial.h"
#include "iterable.h"


namespace original {
    template <typename TYPE>
    class chain final : public serial<TYPE>, public iterable<TYPE>, public printable{
        class chainNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class chain;
            private:
                TYPE data;
                chainNode* prev;
                chainNode* next;
            protected:
                explicit chainNode(TYPE data, chainNode* prev = nullptr, chainNode* next = nullptr);
                chainNode(const chainNode& other);
                chainNode& operator=(const chainNode& other);
                chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT;
                TYPE getVal() const override;
                chainNode* getPPrev() override;
                chainNode* getPNext() override;
                void setPPrev(chainNode* new_prev);
                void setPNext(chainNode* new_next);
                static void connect(chainNode* prev, chainNode* next);
        };

        size_t size_;
        chainNode* begin;
        chainNode* end;

    public:
        explicit chain();
        chain(std::initializer_list<TYPE> list);
        explicit chain(array<TYPE> arr);
        _GLIBCXX_NODISCARD size_t size() const override;
        TYPE get(int index) override;
        TYPE operator[](int index) override;
        void set(int index, TYPE e) override;
        iterator<TYPE>* begins() override;
        iterator<TYPE>* ends() override;
        _GLIBCXX_NODISCARD std::string toString(bool enter) override;
        ~chain() override;
    };
}

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(TYPE data, chainNode* prev, chainNode* next)
    : wrapper<TYPE>(data), data(data), prev(prev), next(next) {}

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(const chainNode& other)
    : wrapper<TYPE>(other->data), data(other->data), prev(other->prev), next(other->next) {}

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::operator=(const chainNode& other) -> chainNode& {
        if (this == &other) return *this;

        this->prev = other->prev;
        this->next = other->next;
        return *this;
    }

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT
    : wrapper<TYPE>(other->data), prev(other->prev), next(other->next)
    {
        other->prev = nullptr;
        other->next = nullptr;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getVal() const -> TYPE
    {
        return this->data;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getPPrev() -> chainNode* {
        return this->prev;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getPNext() -> chainNode* {
        return this->next;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::setPPrev(chainNode* new_prev) -> void {
        this->prev = new_prev;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::setPNext(chainNode* new_next) -> void {
        this->next = new_next;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::connect(chainNode* prev, chainNode* next) -> void
    {
        prev->setPNext(next);
        next->setPPrev(prev);
    }

    template <typename TYPE>
    original::chain<TYPE>::chain()
        : size_(0), begin(), end() {}

    template <typename TYPE>
    original::chain<TYPE>::chain(std::initializer_list<TYPE> list)
        : size_(0), begin(), end() {
        for (auto e : list) {
            auto* cur_node = new chainNode(e);
            if (this->size() == 0)
            {
                this->begin = cur_node;
                this->end = cur_node;
            }else
            {
                chainNode::connect(this->end, cur_node);
                this->end = cur_node;
            }
            size_ += 1;
        }
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(array<TYPE> arr)
        : size_(0), begin(), end() {
        for (size_t i = 0; i < arr.size(); i++) {
            auto* cur_node = new chainNode(arr.get(i));
            if (this->size() == 0)
            {
                this->begin = cur_node;
                this->end = cur_node;
            }else
            {
                chainNode::connect(this->end, cur_node);
                this->end = cur_node;
            }
            size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::chain<TYPE>::size() const -> size_t
    {
        return this->size_;
    }

    template<typename TYPE>
    original::iterator<TYPE> * original::chain<TYPE>::begins() {
        return new iterator(begin);
    }

    template<typename TYPE>
    original::iterator<TYPE> * original::chain<TYPE>::ends() {
        return new iterator(end);
    }

    template <typename TYPE>
    TYPE original::chain<TYPE>::get(int index)
    {
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        auto* cur = this->begin;
        for(size_t i = 0; i < this->negIndex(index); i++)
        {
            cur = cur->getPNext();
        }
        return cur->getVal();
    }

    template <typename TYPE>
    TYPE original::chain<TYPE>::operator[](int index)
    {
        return this->get(index);
    }

    template <typename TYPE>
    void original::chain<TYPE>::set(int index, TYPE e)
    {
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        auto* new_node = new chainNode(e);
        auto* cur = this->begin;
        for(size_t i = 0; i < this->negIndex(index); i++)
        {
            cur = cur->getPNext();
        }
         auto* prev = cur->getPPrev();
         auto* next = cur->getPNext();
         original::chain<TYPE>::chainNode::connect(prev, new_node);
         original::chain<TYPE>::chainNode::connect(new_node, next);
         delete cur;
    }

template <typename TYPE>
    auto original::chain<TYPE>::toString(const bool enter) -> std::string
    {
        std::stringstream ss;
        ss << "chain" << "(";
        for (auto* node = this->begin; node != nullptr; node = node->getPNext())
        {
            ss << node->toString(false);
            if (node != this->end){
                ss << "," << " ";
            }
        }
        ss << ")";
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }

template <typename TYPE>
    original::chain<TYPE>::~chain() {
        auto* current = begin;
        while (current) {
            auto* next = current->getPNext();
            delete current;
            current = next;
        }
    }

#endif //CHAIN_H