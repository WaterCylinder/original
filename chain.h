#ifndef CHAIN_H
#define CHAIN_H
#pragma once

#include <sstream>
#include "array.h"
#include "container.h"

namespace original {
    namespace Chain
    {
        template <typename TYPE>
        class chainNode final : wrapper<TYPE>{

                TYPE data;
                chainNode* prev;
                chainNode* next;

            public:
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
                _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
            };

        template <typename TYPE>
        class iterator final : iterable<wrapper<TYPE>> {
            chainNode<TYPE>* ptr_;

        public:
            explicit iterator(chainNode<TYPE>* ptr);

            iterator* getNext() override;
            iterator* getPrev() override;
            bool hasNext() override;
            bool hasPrev() override;

            iterator& operator++() override;
            iterator& operator--() override;
        };
    }

        template <typename TYPE>
        class chain final : container<TYPE>{
        public:


            size_t size_;
            Chain::chainNode<TYPE>* begin;
            Chain::chainNode<TYPE>* end;

            bool indexOutOfBound(int index);

            explicit chain();
            chain(std::initializer_list<TYPE> list);
            explicit chain(array<TYPE> arr);
            _GLIBCXX_NODISCARD size_t size() const;
            Chain::iterator<Chain::chainNode<TYPE>>* begins() override;
            Chain::iterator<Chain::chainNode<TYPE>>* ends() override;
            TYPE get(int index) const override;
            TYPE operator[](int index) override;
            void set(int index, TYPE e) override;
            _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
            ~chain() override;

        };
    }

    template <typename TYPE>
    original::Chain::chainNode<TYPE>::chainNode(TYPE data, chainNode* prev, chainNode* next)
    : data(data), prev(prev), next(next) {}

    template <typename TYPE>
    original::Chain::chainNode<TYPE>::chainNode(const chainNode& other)
    : data(other->data), prev(other->prev), next(other->next) {}

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::operator=(const chainNode& other) -> chainNode& {
        if (this == &other) return *this;

        this->prev = other->prev;
        this->next = other->next;
        return *this;
    }

    template <typename TYPE>
    original::Chain::chainNode<TYPE>::chainNode(chainNode&& other) _GLIBCXX_NOEXCEPT
    : data(other->data), prev(other->prev), next(other->next)
    {
        other->prev = nullptr;
        other->next = nullptr;
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::getVal() const -> TYPE
    {
        return this->data;
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::getPPrev() -> chainNode* {
        return this->prev;
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::getPNext() -> chainNode* {
        return this->next;
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::setPPrev(chainNode* new_prev) -> void {
        this->prev = new_prev;
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::setPNext(chainNode* new_next) -> void {
        this->next = new_next;
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::connect(chainNode* prev, chainNode* next) -> void
    {
        prev->setPNext(next);
        next->setPPrev(prev);
    }

    template <typename TYPE>
    auto original::Chain::chainNode<TYPE>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->data;
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }

    template <typename TYPE>
    original::Chain::iterator<TYPE>::iterator(chainNode<TYPE>* ptr) : iterable<wrapper<TYPE>>(), ptr_(ptr) {}

    template <typename TYPE>
    auto original::Chain::iterator<TYPE>::getNext() -> iterator*
    {
        return this->ptr_->getPNext();
    }

    template <typename TYPE>
    auto original::Chain::iterator<TYPE>::getPrev() -> iterator* {
        return this->ptr_->getPPrev();
    }

    template <typename TYPE>
    auto original::Chain::iterator<TYPE>::hasNext() -> bool {
        if (this->ptr_ == nullptr)
        {
            return false;
        }
        return this->getNext() != nullptr;
    }

    template <typename TYPE>
    auto original::Chain::iterator<TYPE>::hasPrev() -> bool {
        if (this->ptr_ == nullptr)
        {
            return false;
        }
        return this->getPrev() != nullptr;
    }

    template <typename TYPE>
    auto original::Chain::iterator<TYPE>::operator++() -> iterator& {
        this->ptr_ = this->ptr_->getPNext();
        return *this;
    }

    template <typename TYPE>
    auto original::Chain::iterator<TYPE>::operator--() -> iterator& {
        this->ptr_ = this->ptr_->getPPrev();
        return *this;
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
    original::chain<TYPE>::chain()
        : size_(0), begin(), end() {}

    template <typename TYPE>
    original::chain<TYPE>::chain(std::initializer_list<TYPE> list)
        : size_(0), begin(), end() {
        for (auto e : list) {
            auto* cur_node = new Chain::chainNode(e);
            if (this->size() == 0)
            {
                this->begin = cur_node;
                this->end = cur_node;
            }else
            {
                Chain::chainNode<TYPE>::connect(this->end, cur_node);
                this->end = cur_node;
            }
            size_ += 1;
        }
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(array<TYPE> arr)
        : size_(0), begin(), end() {
        for (size_t i = 0; i < arr.size(); i++) {
            auto* cur_node = new Chain::chainNode(arr.get(i));
            if (this->size() == 0)
            {
                this->begin = cur_node;
                this->end = cur_node;
            }else
            {
                Chain::chainNode<TYPE>::connect(this->end, cur_node);
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
    auto original::chain<TYPE>::begins() -> Chain::iterator<Chain::chainNode<TYPE>>*{
        return new Chain::iterator(this->begin);
    }

    template <typename TYPE>
    auto original::chain<TYPE>::ends() -> Chain::iterator<Chain::chainNode<TYPE>>*{
        return new Chain::iterator(this->end);
    }

template <typename TYPE>
    TYPE original::chain<TYPE>::get(int index) const
    {
        if (this->indexOutOfBound(index)){
            throw indexError();
        }
        const size_t idx = index >= 0 ? index : this->size() + index;
        auto* it = this->begins();
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
        Chain::chainNode<TYPE>* new_node = new Chain::chainNode(e);
        const size_t idx = index >= 0 ? index : this->size() + index;
        auto* it = this->begins();
        for(size_t i = 0; i < idx; i++)
        {
            it = it.getNext();
        }
    //todo
        Chain::chainNode<TYPE>* prev = *it.getPrev();
        Chain::chainNode<TYPE>* next = *it.getNext();
        Chain::chainNode<TYPE>::connect(prev, new_node);
        Chain::chainNode<TYPE>::connect(new_node, next);
        delete *it;
    }

template <typename TYPE>
    auto original::chain<TYPE>::toString(const bool enter) const -> std::string
    {
        std::stringstream ss;
        ss << "chain" << "(";
        for (auto* it = this->begins(); it->hasNext(); it = it->getNext())
        {
            ss << it->operator*()->toString() << ",";
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