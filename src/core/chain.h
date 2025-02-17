#ifndef CHAIN_H
#define CHAIN_H
#pragma once

#include "doubleDirectionIterator.h"
#include "array.h"
#include "baseList.h"
#include "iterationStream.h"

/**
 * @file chain.h
 * @brief Non-cyclic doubly linked list implementation
 * @details Provides a bidirectional linked list container with efficient front/back operations.
 * Features include:
 * - Constant time insertion/removal at both ends
 * - Linear time random access
 * - STL-style bidirectional iterators
 * - Safe element ownership through RAII
 */

namespace original {
    /**
     * @class chain
     * @tparam TYPE Type of elements stored in the list
     * @brief Non-cyclic doubly linked list container
     * @extends baseList
     * @extends iterationStream
     * @details Implements a classic doubly linked list with:
     * - Sentinel nodes for boundary management
     * - Bidirectional traversal capabilities
     * - Index-based element access (O(n) complexity)
     * - Deep copy semantics
     */
    template <typename TYPE>
    class chain final : public baseList<TYPE>, public iterationStream<TYPE, chain<TYPE>>{
        /**
         * @class chainNode
         * @brief Internal node structure for chain elements
         * @extends wrapper
         * @details Represents a single list node containing:
         * - Data payload
         * - Previous/next node pointers
         * - Value access/modification methods
         */
        class chainNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class chain;
            private:
                TYPE data_;         ///< Element storage
                chainNode* prev;    ///< Pointer to previous node
                chainNode* next;    ///< Pointer to next node
            protected:
                explicit chainNode(const TYPE& data = TYPE{}, chainNode* prev = nullptr, chainNode* next = nullptr);
                chainNode(const chainNode& other);
                chainNode& operator=(const chainNode& other);
                TYPE& getVal() override;
                const TYPE& getVal() const override;
                void setVal(TYPE data) override;
                chainNode* getPPrev() const override;
                chainNode* getPNext() const override;
                void setPPrev(chainNode* new_prev);
                void setPNext(chainNode* new_next);

                /**
                * @brief Connects two nodes
                * @param prev Previous node to connect
                * @param next Next node to connect
                */
                static void connect(chainNode* prev, chainNode* next);
        };

        uint32_t size_;         ///< Current element count
        chainNode* begin_;      ///< Pointer to first element node
        chainNode* end_;        ///< Pointer to end sentinel node

        chainNode* findNode(int64_t index) const;
        void chainInit();
        void firstAdd(chainNode* node);
        chainNode* lastDelete();
        void chainDestruction();
    public:
        /**
         * @class Iterator
         * @brief Bidirectional iterator implementation for chain
         * @extends doubleDirectionIterator<TYPE>
         * @details Provides:
         * - Forward/backward traversal
         * - Clone capability
         * - Position comparison checks
         */
        class Iterator final : public doubleDirectionIterator<TYPE>
        {
            explicit Iterator(chainNode* ptr);
        public:
            friend chain;
            Iterator(const Iterator& other);
            Iterator& operator=(const Iterator& other);
            Iterator* clone() const override;
            bool atPrev(const iterator<TYPE> *other) const override;
            bool atNext(const iterator<TYPE> *other) const override;
            [[nodiscard]] std::string className() const override;
        };

        explicit chain();
        chain(const chain& other);
        chain(const std::initializer_list<TYPE>& list);
        explicit chain(const array<TYPE>& arr);
        chain& operator=(const chain& other);
        chain(chain&& other) noexcept;
        chain& operator=(chain&& other) noexcept;
        void operator+=(chain& other);
        [[nodiscard]] uint32_t size() const override;
        TYPE get(int64_t index) const override;
        TYPE& operator[](int64_t index) override;
        void set(int64_t index, const TYPE &e) override;
        uint32_t indexOf(const TYPE &e) const override;
        void pushBegin(const TYPE &e) override;
        void push(int64_t index, const TYPE &e) override;
        void pushEnd(const TYPE &e) override;
        TYPE popBegin() override;
        TYPE pop(int64_t index) override;
        TYPE popEnd() override;
        Iterator* begins() const override;
        Iterator* ends() const override;
        [[nodiscard]] std::string className() const override;
        ~chain() override;
    };
}

    template <typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(const TYPE& data, chainNode* prev, chainNode* next)
    : data_(data), prev(prev), next(next) {}

    template<typename TYPE>
    original::chain<TYPE>::chainNode::chainNode(const chainNode& other)
            : data_(other.data_), prev(other.prev), next(other.next) {}

    template<typename TYPE>
    auto original::chain<TYPE>::chainNode::operator=(const chainNode& other) -> chainNode& {
        if (this != &other) {
            data_ = other.data_;
            prev = other.prev;
            next = other.next;
        }
        return *this;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getVal() -> TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getVal() const -> const TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::setVal(TYPE data) -> void
    {
        this->data_ = data;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getPPrev() const -> chainNode* {
        return this->prev;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainNode::getPNext() const -> chainNode* {
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
        if (prev != nullptr) prev->setPNext(next);
        if (next != nullptr) next->setPPrev(prev);
    }

    template<typename TYPE>
    auto original::chain<TYPE>::findNode(int64_t index) const -> chainNode* {
        const bool reverse_visit = index <= this->size() / 2 ? 0 : 1;
        chainNode* cur;
        if (!reverse_visit){
            cur = this->begin_;
            for(uint32_t i = 0; i < index; i++)
            {
                cur = cur->getPNext();
            }
        } else{
            cur = this->end_;
            for(uint32_t i = this->size() - 1; i > index; i -= 1)
            {
                cur = cur->getPPrev();
            }
        }
        return cur;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainInit() -> void
    {
        auto* pivot = new chainNode();
        this->size_ = 0;
        this->begin_ = pivot->getPNext();
        this->end_ = pivot;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::firstAdd(chainNode* node) -> void
    {
        chainNode::connect(this->end_, node);
        this->begin_ = node;
        this->end_ = node;
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::lastDelete() -> chainNode*
    {
        auto* last = this->end_;
        delete last->getPPrev();
        this->chainInit();
        return last;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::chainDestruction() -> void
    {
        auto* current = this->end_;
        while (current) {
            auto* prev = current->getPPrev();
            delete current;
            current = prev;
        }
    }

    template<typename TYPE>
    original::chain<TYPE>::Iterator::Iterator(chainNode* ptr)
        : doubleDirectionIterator<TYPE>::doubleDirectionIterator(ptr) {}

    template<typename TYPE>
    original::chain<TYPE>::Iterator::Iterator(const Iterator& other)
        : doubleDirectionIterator<TYPE>::doubleDirectionIterator(nullptr) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::chain<TYPE>::Iterator::operator=(const Iterator& other) -> Iterator& {
        if (this == &other) return *this;
        doubleDirectionIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::chain<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::chain<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr && this->_ptr->getPNext() == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::chain<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr && other_it->_ptr->getPNext() == this->_ptr;
    }

    template<typename TYPE>
    auto original::chain<TYPE>::Iterator::className() const -> std::string {
        return "chain::Iterator";
    }

    template <typename TYPE>
    original::chain<TYPE>::chain() : size_(0)
    {
        chainInit();
    }

    template<typename TYPE>
    original::chain<TYPE>::chain(const chain& other) : chain(){
        this->operator=(other);
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(const std::initializer_list<TYPE>& list)
        : chain() {
        for (const auto& e : list) {
            auto* cur_node = new chainNode(e);
            if (this->size() == 0)
            {
                this->firstAdd(cur_node);
            }else
            {
                chainNode::connect(this->end_, cur_node);
                this->end_ = cur_node;
                size_ += 1;
            }
        }
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(const array<TYPE>& arr)
        : chain() {
        for (uint32_t i = 0; i < arr.size(); i++) {
            auto* cur_node = new chainNode(arr.get(i));
            if (this->size() == 0)
            {
                this->firstAdd(cur_node);
            }else
            {
                chainNode::connect(this->end_, cur_node);
                this->end_ = cur_node;
                size_ += 1;
            }
        }
    }

    template<typename TYPE>
    original::chain<TYPE>& original::chain<TYPE>::operator=(const chain& other){
        if (this == &other) return *this;
        this->chainDestruction();
        this->size_ = other.size_;
        if (this->size() != 0){
            auto* other_ = other.begin_->getPPrev();
            auto* pivot = new chainNode(other_->getVal());
            other_ = other_->getPNext();
            chainNode::connect(pivot, new chainNode(other_->getVal()));
            this->begin_ = pivot->getPNext();
            auto* this_ = this->begin_;
            while (other_ != other.end_){
                other_ = other_->getPNext();
                chainNode::connect(this_, new chainNode(other_->getVal()));
                this_ = this_->getPNext();
            }
            this->end_ = this_;
        } else{
            this->chainInit();
        }
        return *this;
    }

    template <typename TYPE>
    original::chain<TYPE>::chain(chain&& other) noexcept : chain()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE>
    auto original::chain<TYPE>::operator=(chain&& other) noexcept -> chain&
    {
        if (this == &other)
            return *this;

        this->chainDestruction();
        this->begin_ = other.begin_;
        this->end_ = other.end_;
        this->size_ = other.size_;
        other.chainInit();
        return *this;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::operator+=(chain& other) -> void
    {
        if (other.empty()) return;

        this->size_ += other.size_;
        delete other.begin_->getPPrev();
        chainNode::connect(this->end_, other.begin_);
        this->end_ = other.end_;
        other.chainInit();
    }

    template <typename TYPE>
    auto original::chain<TYPE>::size() const -> uint32_t
    {
        return this->size_;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::className() const -> std::string
    {
        return "chain";
    }

    template <typename TYPE>
    auto original::chain<TYPE>::get(int64_t index) const -> TYPE
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        chainNode* cur = this->findNode(this->parseNegIndex(index));
        return cur->getVal();
    }

    template <typename TYPE>
    auto original::chain<TYPE>::operator[](const int64_t index) -> TYPE&
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        chainNode* cur = this->findNode(this->parseNegIndex(index));
        return cur->getVal();
    }

    template <typename TYPE>
    auto original::chain<TYPE>::set(int64_t index, const TYPE &e) -> void
    {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        cur->setVal(e);
    }

    template <typename TYPE>
    auto original::chain<TYPE>::indexOf(const TYPE &e) const -> uint32_t {
        uint32_t i = 0;
        for (chainNode* current = this->begin_; current != nullptr; current = current->getPNext()) {
            if (current->getVal() == e) {
                return i;
            }
            i += 1;
        }
        return this->size();
    }

    template <typename TYPE>
    auto original::chain<TYPE>::pushBegin(const TYPE &e) -> void
    {
        auto* new_node = new chainNode(e);
        if (this->size() == 0){
            this->firstAdd(new_node);
        } else{
            auto* pivot = this->begin_->getPPrev();
            chainNode::connect(new_node, this->begin_);
            chainNode::connect(pivot, new_node);
            this->begin_ = new_node;
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::chain<TYPE>::push(int64_t index, const TYPE &e) -> void
    {
        index = this->parseNegIndex(index);
        if (index == 0){
            this->pushBegin(e);
        } else if (index == this->size()){
            this->pushEnd(e);
        } else{
            if (this->indexOutOfBound(index)){
                throw outOfBoundError();
            }
            auto* new_node = new chainNode(e);
            chainNode* cur = this->findNode(index);
            auto* prev = cur->getPPrev();
            chainNode::connect(prev, new_node);
            chainNode::connect(new_node, cur);
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::chain<TYPE>::pushEnd(const TYPE &e) -> void
    {
        auto* new_node = new chainNode(e);
        if (this->size() == 0){
            this->firstAdd(new_node);
        } else{
            chainNode::connect(this->end_, new_node);
            this->end_ = new_node;
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::chain<TYPE>::popBegin() -> TYPE
    {
        TYPE res;
        if (this->size() == 0){
            throw noElementError();
        }
        if (this->size() == 1){
            auto* del = this->lastDelete();
            res = del->getVal();
            delete del;
            this->chainInit();
        } else{
            res = this->begin_->getVal();
            auto* new_begin = this->begin_->getPNext();
            auto* pivot = this->begin_->getPPrev();
            delete this->begin_;
            this->begin_ = new_begin;
            chainNode::connect(pivot, this->begin_);
            this->size_ -= 1;
        }
        return res;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::pop(int64_t index) -> TYPE
    {
        index = this->parseNegIndex(index);
        if (index == 0){
            return this->popBegin();
        }
        if (index == this->size() - 1){
            return this->popEnd();
        }
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        TYPE res;
        chainNode* cur = this->findNode(index);
        res = cur->getVal();
        auto* prev = cur->getPPrev();
        auto* next = cur->getPNext();
        chainNode::connect(prev, next);
        delete cur;
        this->size_ -= 1;
        return res;
    }

    template <typename TYPE>
    auto original::chain<TYPE>::popEnd() -> TYPE
    {
        TYPE res;
        if (this->size() == 0){
            throw noElementError();
        }
        if (this->size() == 1){
            auto* del = this->lastDelete();
            res = del->getVal();
            delete del;
            this->chainInit();
        } else{
            res = this->end_->getVal();
            auto* new_end = this->end_->getPPrev();
            delete this->end_;
            this->end_ = new_end;
            chainNode::connect(this->end_, nullptr);
            this->size_ -= 1;
        }
        return res;
    }

    template<typename TYPE>
    auto original::chain<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->begin_);
    }

    template<typename TYPE>
    auto original::chain<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->end_);
    }

    template <typename TYPE>
    original::chain<TYPE>::~chain() {
        this->chainDestruction();
    }

#endif //CHAIN_H