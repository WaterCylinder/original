#ifndef BLOCKSLIST_H
#define BLOCKSLIST_H

#include <couple.h>
#include <iterationStream.h>
#include "serial.h"

namespace original {
    // todo
    template<typename TYPE>
    class blocksList final : public serial<TYPE>, public iterationStream<TYPE>{
        static constexpr uint32_t BLOCK_MAX_SIZE = 16;
        static constexpr uint32_t POS_INIT = (BLOCK_MAX_SIZE - 1) / 2 + 1;

        vector<TYPE*> map{};
        uint32_t size_;
        uint32_t first;
        uint32_t last;
        uint32_t first_block = 0;
        uint32_t last_block = 0;

        static TYPE* blockArrayInit();
        [[nodiscard]] static uint32_t innerIdxToAbsIdx(uint32_t block, uint32_t pos);
        [[nodiscard]] uint32_t firstAbsIdx() const;
        [[nodiscard]] uint32_t lastAbsIdx() const;
        [[nodiscard]] int64_t absIdxToOuterIdx(uint32_t absIdx) const;
        [[nodiscard]] uint32_t outerIdxToAbsIdx(int64_t outerIdx) const;
        [[nodiscard]] static couple<uint32_t, uint32_t> absIdxToInnerIdx(uint32_t absIdx);
        TYPE& getElem(uint32_t block, uint32_t pos) const;
        void setElem(uint32_t block, uint32_t pos, const TYPE& e);
        [[nodiscard]] bool growNeeded(uint32_t increment, bool is_first) const;
        void moveElements(uint32_t start_idx, uint32_t len, int64_t offset);
        void addBlock(bool is_first);
        void adjust(uint32_t increment, bool is_first);
    public:
        class Iterator final : public baseIterator<TYPE>
        {
            mutable int64_t cur_pos;
            mutable int64_t cur_block;
            mutable TYPE** data_;
            const blocksList* container_;

            explicit Iterator(int64_t pos, int64_t block, TYPE** data_ptr, const blocksList* container);
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

        friend Iterator;
        explicit blocksList();
        blocksList(const std::initializer_list<TYPE>& lst);
        explicit blocksList(const array<TYPE>& arr);
        blocksList(const blocksList& other);
        blocksList& operator=(const blocksList& other);
        bool operator==(const blocksList& other) const;
        TYPE get(int64_t index) const override;
        [[nodiscard]] uint32_t size() const override;
        Iterator* begins() const override;
        Iterator* ends() const override;
        TYPE& operator[](int64_t index) override;
        void set(int64_t index, const TYPE &e) override;
        uint32_t indexOf(const TYPE &e) const override;
        void push(int64_t index, const TYPE &e) override;
        TYPE pop(int64_t index) override;
        void pushBegin(const TYPE &e) override;
        TYPE popBegin() override;
        void pushEnd(const TYPE &e) override;
        TYPE popEnd() override;
        [[nodiscard]] std::string className() const override;
        ~blocksList() override;
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
        return innerIdxToAbsIdx(this->first_block, this->first);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::lastAbsIdx() const -> uint32_t
    {
        return innerIdxToAbsIdx(this->last_block, this->last);
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

    template<typename TYPE>
    TYPE& original::blocksList<TYPE>::getElem(uint32_t block, uint32_t pos) const {
        return this->map.get(block)[pos];
    }

    template<typename TYPE>
    void original::blocksList<TYPE>::setElem(uint32_t block, uint32_t pos, const TYPE &e) {
        this->map.get(block)[pos] = e;
    }

    template<typename TYPE>
    bool original::blocksList<TYPE>::growNeeded(uint32_t increment, bool is_first) const {
        return is_first ? firstAbsIdx() < increment
        : lastAbsIdx() + increment > innerIdxToAbsIdx(this->map.size() - 1, BLOCK_MAX_SIZE - 1);
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
                this->setElem(idx_offset.first(), idx_offset.second(),
                              this->getElem(idx.first(), idx.second()));
            }
        }else
        {
            for (uint32_t i = 0; i < len; i++)
            {
                auto idx = absIdxToInnerIdx(start_idx + i);
                auto idx_offset = absIdxToInnerIdx(start_idx + i + offset);
                this->setElem(idx_offset.first(), idx_offset.second(),
                              this->getElem(idx.first(), idx.second()));
            }
        }
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::addBlock(bool is_first) -> void
    {
        auto* new_block = blockArrayInit();
        is_first ? this->map.pushBegin(new_block) : this->map.pushEnd(new_block);
    }

    template<typename TYPE>
    void original::blocksList<TYPE>::adjust(uint32_t increment, bool is_first) {
        if (this->growNeeded(increment, is_first)){
            uint32_t new_blocks_cnt = increment / BLOCK_MAX_SIZE + 1;
            for (uint32_t i = 0; i < new_blocks_cnt; ++i) {
                this->addBlock(is_first);
            }
            if (is_first){
                this->first_block += new_blocks_cnt;
                this->last_block += new_blocks_cnt;
            }
        }
    }

    template <typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(const int64_t pos, const int64_t block, TYPE** data_ptr, const blocksList* container)
        : cur_pos(pos), cur_block(block), data_(data_ptr), container_(container) {}

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::equalPtr(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr
               && this->cur_pos == other_it->cur_pos
               && this->cur_block == other_it->cur_block
               && this->data_ == other_it->data_
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
        this->data_ = other->data_;
        this->container_ = other->container_;
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
        return blocksList::innerIdxToAbsIdx(this->cur_block, this->cur_pos) > this->container_->firstAbsIdx();
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
        return this->data_[this->cur_block][this->cur_pos];
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::get() const -> TYPE
    {
        if (!this->isValid()) throw outOfBoundError();
        return this->data_[this->cur_block][this->cur_pos];
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::set(const TYPE& data) -> void
    {
        if (!this->isValid()) throw outOfBoundError();
        this->data_[this->cur_block][this->cur_pos] = data;
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
    original::blocksList<TYPE>::blocksList(const std::initializer_list<TYPE>& lst) : blocksList() {
        this->adjust(lst.size(), false);
        for (const auto& e : lst) {
            auto new_idx = absIdxToInnerIdx(this->lastAbsIdx() + 1);
            this->last_block = new_idx.first();
            this->last = new_idx.second();
            this->setElem(this->last_block, this->last, e);
        }
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blocksList(const array<TYPE>& arr) : blocksList() {
        this->adjust(arr.size(), false);
        for (const auto& e : arr) {
            auto new_idx = absIdxToInnerIdx(this->lastAbsIdx() + 1);
            this->last_block = new_idx.first();
            this->last = new_idx.second();
            this->setElem(this->last_block, this->last, e);
        }
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blocksList(const blocksList& other) : blocksList() {
        this->operator=(other);
    }

    template<typename TYPE>
    original::blocksList<TYPE>& original::blocksList<TYPE>::operator=(const blocksList& other) {
        if (this == &other) return *this;

        this->map = other.map;
        this->first = other.first;
        this->last = other.last;
        this->size_ = other.size_;
        return *this;
    }

    template<typename TYPE>
    bool original::blocksList<TYPE>::operator==(const blocksList& other) const {
        if (this == &other) return true;
        if (this->size() != other.size()) return false;
        for (uint32_t i = 0; i < this->size(); ++i) {
            auto this_idx = absIdxToInnerIdx(this->outerIdxToAbsIdx(i));
            auto other_idx = absIdxToInnerIdx(other.outerIdxToAbsIdx(i));
            if (this->getElem(this_idx.first(), this_idx.second()) !=
                other.getElem(other_idx.first(), other_idx.second()))
                return false;
        }
        return true;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::get(int64_t index) const -> TYPE {
        if (this->indexOutOfBound(this->parseNegIndex(index))) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto inner_idx = absIdxToInnerIdx(this->outerIdxToAbsIdx(index));
        return this->getElem(inner_idx.first(), inner_idx.second());
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::size() const -> uint32_t {
        return this->size_;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->first, this->first_block, &this->map.data(), this);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->last, this->last_block, &this->map.data(), this);
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
        this->setElem(inner_idx.first(), inner_idx.second(), e);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::indexOf(const TYPE &e) const -> uint32_t {
        for (uint32_t i = 0; i < this->size(); ++i) {
            auto idx = absIdxToInnerIdx(this->outerIdxToAbsIdx(i));
            if (this->getElem(idx.first(), idx.second()) == e) return i;
        }
        return this->size();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::className() const -> std::string {
        return "blocksList";
    }

    template<typename TYPE>
    original::blocksList<TYPE>::~blocksList() {
        for (auto* block : this->map) {
            delete[] block;
        }
    }

#endif //BLOCKSLIST_H
