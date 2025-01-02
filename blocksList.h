#ifndef BLOCKSLIST_H
#define BLOCKSLIST_H

#include <couple.h>
#include <iterationStream.h>
#include "serial.h"

namespace original {
    // todo
    template<typename TYPE>
    class blocksList final : public serial<TYPE>, public iterationStream<TYPE>{
        vector<TYPE*> map{};
        uint32_t size_;
        static constexpr uint32_t BLOCK_MAX_SIZE = 16;
        static constexpr uint32_t POS_INIT = (BLOCK_MAX_SIZE - 1) / 2 + 1;
        uint32_t first;
        uint32_t last;

        static TYPE* blockArrayInit();
        [[nodiscard]] static uint32_t innerIdxToAbsIdx(uint32_t block, uint32_t pos);
        [[nodiscard]] uint32_t firstAbsIdx() const;
        [[nodiscard]] uint32_t lastAbsIdx() const;
        [[nodiscard]] int64_t absIdxToOuterIdx(uint32_t absIdx) const;
        [[nodiscard]] uint32_t outerIdxToAbsIdx(int64_t outerIdx) const;
        [[nodiscard]] static couple<uint32_t, uint32_t> absIdxToInnerIdx(uint32_t absIdx);
        void moveElements(uint32_t start_idx, uint32_t len, int64_t offset);
        void addBlock(bool first);
        void removeBlock(bool first);
    public:
        class Iterator final : public baseIterator<TYPE>
        {
            mutable int64_t cur_pos;
            mutable int64_t cur_block;
            mutable TYPE* block_;
            const blocksList* container_;

            explicit Iterator(int64_t pos, int64_t block, TYPE* block_ptr, const blocksList* container);
            bool equalPtr(const iterator<TYPE> *other) const override;
        public:
            friend blocksList;
            Iterator(const Iterator& other);
            Iterator& operator=(const Iterator& other);
            Iterator* clone() const override;
            [[nodiscard]] bool hasNext() const override;
            [[nodiscard]] bool hasPrev() const override;
            void next() const override;
            void prev() const override;
            void operator+=(int64_t steps) const override;
            void operator-=(int64_t steps) const override;
            int64_t operator-(const iterator<TYPE>& other) const override;
            Iterator* getPrev() const override;
            Iterator* getNext() const override;
            TYPE& get() override;
            TYPE get() const override;
            void set(const TYPE &data) override;
            [[nodiscard]] bool isValid() const override;
            bool atPrev(const iterator<TYPE> *other) const override;
            bool atNext(const iterator<TYPE> *other) const override;
            [[nodiscard]] std::string className() const override;
        };

        friend class Iterator;
        explicit blocksList();
        TYPE get(int64_t index) const override;
        [[nodiscard]] uint32_t size() const override;
        Iterator* begins() const override;
        Iterator* ends() const override;
        TYPE& operator[](int64_t index) override;
        void set(int64_t index, const TYPE &e) override;
        uint32_t indexOf(const TYPE &e) const override;
        [[nodiscard]] std::string className() const override;
    };
}// namespace original

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockArrayInit() -> TYPE* {
        auto arr = new TYPE[BLOCK_MAX_SIZE];
        for (uint32_t i = 0; i < BLOCK_MAX_SIZE; i++) {
            arr[i] = TYPE{};
        }
        return arr;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::innerIdxToAbsIdx(const uint32_t block, const uint32_t pos) -> uint32_t
    {
        return block * BLOCK_MAX_SIZE + pos;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::firstAbsIdx() const -> uint32_t
    {
        return innerIdxToAbsIdx(0, this->first);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::lastAbsIdx() const -> uint32_t
    {
        return innerIdxToAbsIdx(this->map.size() - 1, this->last);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::absIdxToOuterIdx(const uint32_t absIdx) const -> int64_t
    {
        return absIdx - this->firstAbsIdx();
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::outerIdxToAbsIdx(const int64_t outerIdx) const -> uint32_t
    {
        return this->firstAbsIdx() + outerIdx;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::absIdxToInnerIdx(const uint32_t absIdx) -> couple<uint32_t, uint32_t>
    {
        return {absIdx / BLOCK_MAX_SIZE, absIdx % BLOCK_MAX_SIZE};
    }

    template <typename TYPE>
    void original::blocksList<TYPE>::moveElements(uint32_t start_idx, uint32_t len, int64_t offset)
    {
        if (offset > 0)
        {
            for (uint32_t i = 0; i < len; i++)
            {
                auto idx = absIdxToInnerIdx(start_idx + len - 1 - i);
                auto idx_offset = absIdxToInnerIdx(start_idx + len - 1 - i + offset);
                this->map.get(idx_offset.first())[idx_offset.second()] = this->map.get(idx.first())[idx.second()];
            }
        }else
        {
            for (uint32_t i = 0; i < len; i++)
            {
                auto idx = absIdxToInnerIdx(start_idx + i);
                auto idx_offset = absIdxToInnerIdx(start_idx + i + offset);
                this->map.get(idx_offset.first())[idx_offset.second()] = this->map.get(idx.first())[idx.second()];
            }
        }
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::addBlock(bool first) -> void
    {
        auto* new_block = blockArrayInit();
        first ? this->map.pushBegin(new_block) : this->map.pushEnd(new_block);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::removeBlock(bool first) -> void
    {
        auto* removed_block = first ? this->map.popBegin() : this->map.popEnd();
        delete[] removed_block;
    }

    template <typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(const int64_t pos, const int64_t block, TYPE* block_ptr, const blocksList* container)
        : cur_pos(pos), cur_block(block), block_(block_ptr), container_(container) {}

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::equalPtr(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr
               && this->cur_pos = other_it->cur_pos
               && this->cur_block = other_it->cur_block
               && this->block_ == other_it->block_
               && this->container_ == other_it->container_;
    }

    template <typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(const Iterator& other) : Iterator(0, 0, nullptr, nullptr)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::operator=(const Iterator& other) -> Iterator&
    {
        if (this == &other)
            return *this;

        this->cur_pos = other->cur_pos;
        this->cur_block = other->cur_block;
        this->block_ == other->block_;
        this->container_ == other->container_;
        return *this;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::clone() const -> Iterator*
    {
        return new Iterator(*this);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::hasNext() const -> bool
    {
        return blocksList::innerIdxToAbsIdx(this->cur_block, this->cur_pos) < this->container_->lastAbsIdx();
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::hasPrev() const -> bool
    {
        return blocksList::innerIdxToAbsIdx(this->cur_block, this->cur_pos) > 0;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::next() const -> void
    {
        this->operator+=(1);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::prev() const -> void
    {
        this->operator-=(1);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::operator+=(int64_t steps) const -> void
    {
        if (!this->isValid()) throw outOfBoundError();
        auto new_idx = absIdxToInnerIdx(innerIdxToAbsIdx(this->cur_block, this->cur_pos) + steps);
        this->cur_block = new_idx.first();
        this->cur_pos = new_idx.second();
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::operator-=(int64_t steps) const -> void
    {
        if (!this->isValid()) throw outOfBoundError();
        auto new_idx = absIdxToInnerIdx(innerIdxToAbsIdx(this->cur_block, this->cur_pos) - steps);
        this->cur_block = new_idx.first();
        this->cur_pos = new_idx.second();
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::operator-(const iterator<TYPE>& other) const -> int64_t
    {
        auto* other_it = dynamic_cast<const Iterator*>(&other);
        if (other_it == nullptr)
            return this > &other ?
                std::numeric_limits<int64_t>::max() :
                std::numeric_limits<int64_t>::min();
        if (this->container_ != other_it->container_)
            return this->container_ > other_it->container_ ?
                std::numeric_limits<int64_t>::max() :
                std::numeric_limits<int64_t>::min();

        return innerIdxToAbsIdx(this->cur_block, this->cur_pos) - innerIdxToAbsIdx(other_it->cur_block, other_it->cur_pos);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::getPrev() const -> Iterator*
    {
        if (!this->isValid()) throw outOfBoundError();
        auto* it = this->clone();
        it->prev();
        return it;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::getNext() const -> Iterator*
    {
        if (!this->isValid()) throw outOfBoundError();
        auto* it = this->clone();
        it->next();
        return it;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::get() -> TYPE&
    {
        if (!this->isValid()) throw outOfBoundError();
        return this->block_[this->cur_pos];
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::get() const -> TYPE
    {
        if (!this->isValid()) throw outOfBoundError();
        return this->block_[this->cur_pos];
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::set(const TYPE& data) -> void
    {
        if (!this->isValid()) throw outOfBoundError();
        this->block_[this->cur_pos] = data;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::isValid() const -> bool
    {
        return  blocksList::absIdxToOuterIdx(innerIdxToAbsIdx(this->cur_block, this->cur_pos)) >= 0 &&
                blocksList::absIdxToOuterIdx(innerIdxToAbsIdx(this->cur_block, this->cur_pos)) < this->container_->size();
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::atPrev(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        if (other_it == nullptr)
            return false;
        return this->operator-(*other_it) == -1;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::atNext(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        if (other_it == nullptr)
            return false;
        return this->operator-(*other_it) == 1;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::className() const -> std::string {
        return "blocksList::Iterator";
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blocksList()
        : map(vector({blockArrayInit()})), size_(0), first(POS_INIT + 1), last(POS_INIT) {}

    template<typename TYPE>
    auto original::blocksList<TYPE>::get(int64_t index) const -> TYPE {
        if (this->indexOutOfBound(this->parseNegIndex(index))) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto inner_idx = absIdxToInnerIdx(this->outerIdxToAbsIdx(index));
        return this->map.get(inner_idx.first())[inner_idx.second()];
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::size() const -> uint32_t {
        return this->size_;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->map.data(), this);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->map.data() + this->map.size() - 1, this);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::operator[](const int64_t index) -> TYPE& {
        return this->get(index);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::set(int64_t index, const TYPE &e) -> void {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto inner_idx = absIdxToInnerIdx(this->outerIdxToAbsIdx(index));
        this->map.get(inner_idx.first())[inner_idx.second()] = e;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::indexOf(const TYPE &e) const -> uint32_t {
        for (uint32_t i = 0; i < this->size(); ++i) {
            if (this->get(i) == e) return i;
        }
        return this->size();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::className() const -> std::string {
        return "blocksList";
    }

#endif //BLOCKSLIST_H
