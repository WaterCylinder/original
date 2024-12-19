#ifndef BLOCKSLIST_H
#define BLOCKSLIST_H

#include <couple.h>
#include <iterationStream.h>
#include "serial.h"

namespace original {
    // todo
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


        vector<blocksListNode**> map;
        size_t size_;
        static constexpr size_t BLOCK_MAX_SIZE = 16;
        static constexpr size_t BLOCK_EMPTY = 0;
        static constexpr size_t BLOCK_FULL = BLOCK_MAX_SIZE;
        size_t first;
        size_t last;

        static blocksListNode** blockArrayInit();
        [[nodiscard]] static size_t indexReset(size_t index, bool increase);
        [[nodiscard]] static size_t blockCurSize(size_t index);
        [[nodiscard]] couple<size_t, size_t> toInnerIdx(size_t index) const;
        void moveElements(size_t index, size_t len, int offset);
        void grow(bool first);
        void squeeze(bool first);
    public:
        explicit blocksList();
        TYPE get(long long index) const override;
        [[nodiscard]] std::string className() const override;
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

    template <typename TYPE>
    auto original::blocksList<TYPE>::blockArrayInit() -> blocksListNode**
    {
        auto** arr = new blocksListNode*[BLOCK_MAX_SIZE];
        for (size_t i = 0; i < BLOCK_MAX_SIZE; i += 1)
        {
            arr[i] = nullptr;
        }
        return arr;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::indexReset(const size_t index, const bool increase) -> size_t
    {
        if (index == BLOCK_FULL && increase)
        {
            return BLOCK_EMPTY;
        }
        if (index == BLOCK_EMPTY && !increase)
        {
            return BLOCK_FULL;
        }
        return index;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blockCurSize(const size_t index) -> size_t
    {
        return index - BLOCK_EMPTY;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::toInnerIdx(size_t index) const -> couple<size_t, size_t>

    {
        auto res = couple<size_t, size_t>();
        if (index <= blocksList::blockCurSize(this->first) - 1)
        {
            res.second() = this->first + index + 1;
        }else
        {
            res.first() = (index + 1 - blocksList::blockCurSize(this->first)) / BLOCK_MAX_SIZE + 1;
            res.second() = (index + 1 - blocksList::blockCurSize(this->first)) % BLOCK_MAX_SIZE;
        }
        return res;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::moveElements(const size_t index, const size_t len, const int offset) -> void
    {
        if (offset > 0)
        {
            for (size_t i = 0; i < len; i += 1)
            {
                auto src_idx = this->toInnerIdx(index + offset + len - 1 - i);
                auto tar_idx = this->toInnerIdx(index + len - 1 - i);
                this->map.get(tar_idx.first())[tar_idx.second()] = this->map.get(src_idx.first())[src_idx.second()];
                this->map.get(src_idx.first())[src_idx.second()] = nullptr;
            }
        }else
        {
            for (size_t i = 0; i < len; i += 1)
            {
                auto src_idx = this->toInnerIdx(index + offset + i);
                auto tar_idx = this->toInnerIdx(index + i);
                this->map.get(tar_idx.first())[tar_idx.second()] = this->map.get(src_idx.first())[src_idx.second()];
                this->map.get(src_idx.first())[src_idx.second()] = nullptr;
            }
        }
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::grow(const bool first) -> void
    {
        if (first)
        {
            this->map.pushBegin(blocksList::blockArrayInit());
        }else
        {
            this->map.pushEnd(blocksList::blockArrayInit());
        }
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::squeeze(const bool first) -> void
    {
        if (first)
        {
            this->map.popBegin();
        }else
        {
            this->map.popEnd();
        }
    }

    template <typename TYPE>
    original::blocksList<TYPE>::blocksList()
        : map(), size_(0), first(BLOCK_EMPTY), last(BLOCK_EMPTY) {}

    template <typename TYPE>
    auto original::blocksList<TYPE>::get(long long index) const -> TYPE
    {
        index = this->parseNegIndex(index);
        auto idx = this->toInnerIdx(index);
        return this->map.get(idx.first())[idx.second()];
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::className() const -> std::string
    {
        return "blocksList";
    }

#endif //BLOCKSLIST_H
