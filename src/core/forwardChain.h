#ifndef FORWARDCHAIN_H
#define FORWARDCHAIN_H

#include "singleDirectionIterator.h"
#include "array.h"
#include "baseList.h"

namespace original {
    template <typename TYPE>
    class forwardChain final : public baseList<TYPE>, public iterationStream<TYPE>{
        class forwardChainNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class forwardChain;
            private:
                TYPE data_;
                forwardChainNode* next;
            protected:
                explicit forwardChainNode(const TYPE& data = TYPE{}, forwardChainNode* next = nullptr);
                forwardChainNode(const forwardChainNode& other);
                forwardChainNode& operator=(const forwardChainNode& other);
                TYPE& getVal() override;
                const TYPE& getVal() const override;
                void setVal(TYPE data) override;
                forwardChainNode* getPPrev() const override;
                forwardChainNode* getPNext() const override;
                void setPNext(forwardChainNode* new_next);
                static void connect(forwardChainNode* prev, forwardChainNode* next);
        };

        uint32_t size_;
        forwardChainNode* begin_;

        forwardChainNode* beginNode() const;
        forwardChainNode* findNode(int64_t index) const;
        void chainInit();
        void firstAdd(forwardChainNode* node);
        forwardChainNode* lastDelete();
        void chainDestruction();
    public:
        class Iterator final : public singleDirectionIterator<TYPE>
        {
            explicit Iterator(forwardChainNode* ptr);
        public:
            friend forwardChain;
            Iterator(const Iterator& other);
            Iterator& operator=(const Iterator& other);
            Iterator* clone() const override;
            bool atPrev(const iterator<TYPE> *other) const override;
            bool atNext(const iterator<TYPE> *other) const override;
            [[nodiscard]] std::string className() const override;
        };

        explicit forwardChain();
        forwardChain(const forwardChain& other);
        forwardChain(std::initializer_list<TYPE> list);
        explicit forwardChain(const array<TYPE>& arr);
        forwardChain& operator=(const forwardChain& other);
        forwardChain(forwardChain&& other) noexcept;
        forwardChain& operator=(forwardChain&& other) noexcept;
        bool operator==(const forwardChain& other) const;
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
        ~forwardChain() override;
    };
}

    template <typename TYPE>
    original::forwardChain<TYPE>::forwardChainNode::forwardChainNode(const TYPE& data, forwardChainNode* next)
        : data_(data), next(next) {}

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChainNode::forwardChainNode(const forwardChainNode &other)
        : data_(other.data_), next(other.next) {}

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::operator=(
        const forwardChainNode &other) -> forwardChainNode & {
        if (this != &other) {
            data_ = other.data_;
            next = other.next;
        }
        return *this;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getVal() -> TYPE& {
        return this->data_;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getVal() const -> const TYPE& {
        return this->data_;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::setVal(TYPE data) -> void {
        this->data_ = data;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getPPrev() const -> forwardChainNode* {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getPNext() const -> forwardChainNode* {
        return this->next;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::setPNext(forwardChainNode *new_next) -> void {
        this->next = new_next;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::connect(
        forwardChainNode *prev, forwardChainNode *next) -> void {
        if (prev != nullptr) prev->setPNext(next);
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::beginNode() const -> forwardChainNode*
    {
        return this->begin_->getPNext();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::findNode(const int64_t index) const -> forwardChainNode* {
        if (this->size() == 0) return this->begin_;
        auto* cur = this->beginNode();
        for(uint32_t i = 0; i < index; i++)
        {
            cur = cur->getPNext();
        }
        return cur;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::chainInit() -> void
    {
        auto* pivot = new forwardChainNode();
        this->size_ = 0;
        this->begin_ = pivot;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::firstAdd(forwardChainNode* node) -> void
    {
        forwardChainNode::connect(this->findNode(0), node);
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::lastDelete() -> forwardChainNode*
    {
        auto* last = this->beginNode();
        delete this->begin_;
        this->chainInit();
        return last;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::chainDestruction() -> void
    {
        auto* cur = this->begin_;
        while (cur)
        {
            auto* next = cur->getPNext();
            delete cur;
            cur = next;
        }
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::Iterator::Iterator(forwardChainNode *ptr)
        : singleDirectionIterator<TYPE>(ptr) {}

    template<typename TYPE>
    original::forwardChain<TYPE>::Iterator::Iterator(const Iterator &other)
        : singleDirectionIterator<TYPE>(nullptr) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::operator=(const Iterator &other) -> Iterator & {
        if (this == &other) return *this;
        singleDirectionIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr && this->_ptr->getPNext() == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr && other_it->_ptr->getPNext() == this->_ptr;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::className() const -> std::string {
        return "forwardChain::Iterator";
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain() : size_(0)
    {
        this->chainInit();
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(const forwardChain &other) : forwardChain() {
        this->operator=(other);
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(std::initializer_list<TYPE> list) : forwardChain() {
        for (auto e: list) {
            auto* cur_node = new forwardChainNode(e);
            if (this->size() == 0)
            {
                this->firstAdd(cur_node);
            }else
            {
                auto* end = this->findNode(this->size() - 1);
                forwardChainNode::connect(end, cur_node);
                this->size_ += 1;
            }
        }
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(const array<TYPE>& arr) : forwardChain() {
        for (uint32_t i = 0; i < arr.size(); i++) {
            auto* cur_node = new forwardChainNode(arr.get(i));
            if (this->size() == 0)
            {
                this->firstAdd(cur_node);
            }else
            {
                auto* end = this->findNode(this->size() - 1);
                forwardChainNode::connect(end, cur_node);
                this->size_ += 1;
            }
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::operator=(const forwardChain &other) -> forwardChain& {
        if (this == &other) return *this;
        this->chainDestruction();
        this->size_ = other.size_;
        if (this->size() != 0){
            auto* other_ = other.begin_;
            this->begin_ = new forwardChainNode(other_->getVal());
            auto* this_ = this->begin_;
            while (other_->getPNext() != nullptr){
                other_ = other_->getPNext();
                forwardChainNode::connect(this_, new forwardChainNode(other_->getVal()));
                this_ = this_->getPNext();
            }
        } else{
            this->chainInit();
        }
        return *this;
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(forwardChain &&other) noexcept : forwardChain() {
        this->operator=(std::move(other));
    }

    template<typename TYPE>
    original::forwardChain<TYPE> & original::forwardChain<TYPE>::operator=(forwardChain &&other) noexcept {
        if (this == &other)
            return *this;

        this->chainDestruction();
        this->begin_ = other.begin_;
        this->size_ = other.size_;
        other.chainInit();
        return *this;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::operator==(const forwardChain &other) const -> bool {
        if (this == &other) return true;
        if (this->size() != other.size()) return false;
        auto* this_ = this->begin_;
        auto* other_ = other.begin_;
        for (uint32_t i = 0; i < this->size(); ++i) {
            if (this_->getVal() != other_->getVal()){
                return false;
            }
        }
        return true;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::size() const -> uint32_t {
        return this->size_;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::get(int64_t index) const -> TYPE {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        return cur->getVal();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::operator[](int64_t index) -> TYPE& {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        return cur->getVal();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::set(int64_t index, const TYPE &e) -> void {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        cur->setVal(e);
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::indexOf(const TYPE &e) const -> uint32_t {
        uint32_t i = 0;
        for (auto* current = this->begin_; current != nullptr; current = current->getPNext()) {
            if (current->getVal() == e) {
                return i;
            }
            i += 1;
        }
        return this->size();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::pushBegin(const TYPE &e) -> void {
        auto* new_node = new forwardChainNode(e);
        if (this->size() == 0){
            this->firstAdd(new_node);
        } else{
            auto* next = this->beginNode();
            forwardChainNode::connect(this->begin_, new_node);
            forwardChainNode::connect(new_node, next);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::push(int64_t index, const TYPE &e) -> void {
        index = this->parseNegIndex(index);
        if (index == 0){
            this->pushBegin(e);
        } else if (index == this->size()){
            this->pushEnd(e);
        } else{
            if (this->indexOutOfBound(index)){
                throw outOfBoundError();
            }
            auto* new_node = new forwardChainNode(e);
            auto* prev = this->findNode(index - 1);
            auto* cur = prev->getPNext();
            forwardChainNode::connect(prev, new_node);
            forwardChainNode::connect(new_node, cur);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::pushEnd(const TYPE &e) -> void {
        auto* new_node = new forwardChainNode(e);
        if (this->size() == 0){
            this->firstAdd(new_node);
        } else{
            auto* end = this->findNode(this->size() - 1);
            forwardChainNode::connect(end, new_node);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::popBegin() -> TYPE {
        TYPE res;
        if (this->size() == 0){
            throw noElementError();
        }

        res = this->beginNode()->getVal();
        if (this->size() == 1){
            delete this->lastDelete();
        } else{
            auto* del = this->beginNode();
            auto* new_begin = del->getPNext();
            delete del;
            forwardChainNode::connect(this->begin_, new_begin);
            this->size_ -= 1;
        }
        return res;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::pop(int64_t index) -> TYPE {
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
        auto* prev = this->findNode(index - 1);
        auto* cur = prev->getPNext();
        res = cur->getVal();
        auto* next = cur->getPNext();
        forwardChainNode::connect(prev, next);
        delete cur;
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::popEnd() -> TYPE {
        TYPE res;
        if (this->size() == 0){
            throw noElementError();
        }
        if (this->size() == 1){
            res = this->beginNode()->getVal();
            delete this->lastDelete();
        } else{
            auto* new_end = this->findNode(this->size() - 2);
            auto* end = new_end->getPNext();
            res = end->getVal();
            delete end;
            forwardChainNode::connect(new_end, nullptr);
            this->size_ -= 1;
        }
        return res;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->beginNode());
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->findNode(this->size() - 1));
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::className() const -> std::string {
        return "forwardChain";
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::~forwardChain() {
        this->chainDestruction();
    }

#endif //FORWARDCHAIN_H
