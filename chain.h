#ifndef CHAIN_H
#define CHAIN_H
#pragma once

#include <sstream>
#include "array.h"
#include "iterable.h"

namespace original {
        template <typename TYPE>
        class chain {

            class chainNode {
                TYPE data;
                chainNode* prev;
                chainNode* next;

            protected:
                explicit chainNode(TYPE data, chainNode* prev = nullptr, chainNode* next = nullptr);
                chainNode(const chainNode& other);
                chainNode& operator=(const chainNode& other);
                chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT;

                TYPE getVal() const;
                chainNode* getPPrev();
                chainNode* getPNext();
                void setPPrev(chainNode* new_prev);
                void setPNext(chainNode* new_next);
                static void connect(chainNode* prev, chainNode* next);
                _GLIBCXX_NODISCARD std::string toString(bool enter = false) const;
            };

            class iterator final : public iterable<chainNode> {
                chainNode* ptr_;

            public:
                explicit iterator(chainNode* ptr);

                chainNode* getNext() override;
                chainNode* getPrev() override;
                bool hasNext() override;
                bool hasPrev() override;

                iterator& operator++() override;
                iterator& operator--() override;
            };

            size_t size_;
            chainNode* begin;
            chainNode* end;

        public:
            explicit chain();
            chain(std::initializer_list<TYPE> list);
            explicit chain(array<TYPE> arr);
            _GLIBCXX_NODISCARD size_t size() const;
            bool indexOutOfBound(int index);
            TYPE get(int index);
            TYPE operator[](int index);
            void set(int index, TYPE e);
            _GLIBCXX_NODISCARD std::string toString(bool enter = false) const;
            ~chain();
        };
    }

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(TYPE data, chainNode* prev, chainNode* next)
        : data(data), prev(prev), next(next) {}

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(const chainNode& other)
        : data(other.data), prev(other.prev), next(other.next) {}

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::operator=(const chainNode& other) -> chainNode& {
        if (this == &other) return *this;

        this->prev = other->prev;
        this->next = other->next;
        return *this;
    }

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT
        : data(other.data), prev(other.prev), next(other.next) {
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
    auto original::chain<TYPE>::chainNode::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->data;
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }

    template <typename TYPE>
    original::chain<TYPE>::iterator::iterator(chainNode* ptr) : iterable<chainNode>(ptr), ptr_(ptr) {}

    template <typename TYPE>
    auto original::chain<TYPE>::iterator::getNext() -> chainNode* {
        return this.ptr_->getPNext();
    }

    template <typename TYPE>
    auto original::chain<TYPE>::iterator::getPrev() -> chainNode* {
        return this.ptr_->getPPrev();
    }

    template <typename TYPE>
    auto original::chain<TYPE>::iterator::hasNext() -> bool {
        if (this.ptr_ == nullptr)
        {
            return false;
        }
        return this->getNext() != nullptr;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::iterator::hasPrev() -> bool {
        if (this.ptr_ == nullptr)
        {
            return false;
        }
        return this->getPrev() != nullptr;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::iterator::operator++() -> iterator& {
        this.ptr_ = this.ptr_->getPNext();
        return *this;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::iterator::operator--() -> iterator& {
        this.ptr_ = this.ptr_->getPPrev();
        return *this;
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

    template <typename TYPE>
    auto original::chain<TYPE>::indexOutOfBound(int index) -> bool
    {
        if (index < 0)
        {
            return index < - this->size();
        }
        return index >= this->size_;
    }

    template <typename TYPE>
    TYPE original::chain<TYPE>::get(int index)
    {
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        const size_t idx = index >= 0 ? index : this->size() + index;
        iterator it = this->begin;
        for(size_t i = 0; i < idx; i++)
        {
            it = it.getNext();
        }
        return *it;
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
        chainNode new_node = new chainNode(e);
        const size_t idx = index >= 0 ? index : this->size() + index;
        iterator it = this->begin;
        for(size_t i = 0; i < idx; i++)
        {
            it = it.getNext();
        }
    //todo
        chainNode* prev = *it.getPrev();
        chainNode* next = *it.getNext();
        chainNode::connect(prev, new_node);
        chainNode::connect(new_node, next);
        delete *it;
    }

template <typename TYPE>
    auto original::chain<TYPE>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << "chain" << "(";
        for (iterator it = new chain::iterator<>(this->begin); it.hasNext(); it = it.getNext())
        {
            ss << *it.toString() << ",";
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