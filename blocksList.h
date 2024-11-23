#ifndef BLOCKSLIST_H
#define BLOCKSLIST_H

#include <iterationStream.h>

namespace original {
    template<typename TYPE>
    class blocksList final : public serial<TYPE>, public iterationStream<TYPE>{
        class blocksListNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class blocksList;
            private:
                TYPE data_;
                blocksListNode* prev;
                blocksListNode* next;
            protected:
                explicit blocksListNode(TYPE data = TYPE{}, blocksListNode* prev = nullptr, blocksListNode* next = nullptr);
                blocksListNode(const blocksListNode& other);
                blocksListNode& operator=(const blocksListNode& other);
                TYPE& getVal() override;
                const TYPE& getVal() const override;
                void setVal(TYPE data) override;
                blocksListNode* getPPrev() const override;
                blocksListNode* getPNext() const override;
                void setPPrev(blocksListNode* new_prev);
                void setPNext(blocksListNode* new_next);
                ~blocksListNode() override;
                static void connect(blocksListNode* prev, blocksListNode* next);
        };

        // todo
    };
}// namespace original

    template<typename TYPE>
    original::blocksList<TYPE>::blocksListNode::blocksListNode(TYPE data, blocksListNode* prev, blocksListNode* next)
        : data_(data), prev(prev), next(next) {}

    template<typename TYPE>
    original::blocksList<TYPE>::blocksListNode::blocksListNode(const blocksListNode& other)
        : data_(other.data_), prev(other.prev), next(other.next) {}

    template<typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::operator=(const blocksListNode& other) -> blocksListNode& {
        if (this != &other) {
            data_ = other.data_;
            prev = other.prev;
            next = other.next;
        }
        return *this;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::getVal() -> TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::getVal() const -> const TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::setVal(TYPE data) -> void
    {
        this->data_ = data;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::getPPrev() const -> blocksListNode* {
        return this->prev;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::getPNext() const -> blocksListNode* {
        return this->next;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::setPPrev(blocksListNode* new_prev) -> void
    {
        this->prev = new_prev;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::setPNext(blocksListNode* new_next) -> void
    {
        this->next = new_next;
    }

    template <typename TYPE>
    original::blocksList<TYPE>::blocksListNode::~blocksListNode()= default;

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListNode::connect(blocksListNode* prev, blocksListNode* next) -> void
    {
        if (prev != nullptr) prev->setPNext(next);
        if (next != nullptr) next->setPPrev(prev);
    }

#endif //BLOCKSLIST_H
