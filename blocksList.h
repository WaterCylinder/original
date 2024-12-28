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

        class blockState final : public container<TYPE> {
                bool first_;
                bool last_;
                uint32_t first_ptr;
                uint32_t last_ptr;

                explicit blockState(bool first, bool last, uint32_t first_ptr, uint32_t last_ptr);
                [[nodiscard]] uint32_t size() const override;
                bool contains(const TYPE &e) const override;
                void add(const TYPE &e) override;
                void clear() override;
            public:
                friend class blocksList::Iterator;
                friend class blocksList::blockItr;
        };

        class blockItr final : public randomAccessIterator<TYPE> {
            explicit blockItr(TYPE* ptr, blockState* container, int64_t pos);
            blockItr(const blockItr& other);
            blockItr& operator=(const blockItr& other);
            blockItr* clone() const override;
        };

        static TYPE* blockArrayInit();
        [[nodiscard]] couple<uint32_t, uint32_t> toInnerIdx(int64_t index) const;
        void moveElements(uint32_t index, uint32_t len, int offset);
        void grow(bool first);
        void shrink(bool first);
    public:
        class Iterator final : public baseIterator<TYPE>
        {
            mutable randomAccessIterator<TYPE*>* base_;
            mutable blockItr* cur_;

            explicit Iterator(TYPE** ptr, const blocksList* container);
            bool equalPtr(const iterator<TYPE> *other) const override;
        public:
            friend blocksList;
            friend class blockItr;
            Iterator(const Iterator& other);
            Iterator& operator=(const Iterator& other);
            Iterator* clone() const override;
            [[nodiscard]] bool hasNext() const override;
            [[nodiscard]] bool hasPrev() const override;
            void next() const override;
            void prev() const override;
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

        friend class blockItr;
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

    template<typename TYPE>
    auto original::blocksList<TYPE>::toInnerIdx(int64_t index) const -> couple<uint32_t, uint32_t> {
        const uint32_t first_size = BLOCK_MAX_SIZE - this->first;
        if (this->map.size() == 1 || index + 1 <= first_size) return {0, this->first + index};

        const uint32_t remains = index + 1 - first_size;
        return {1 + remains / (BLOCK_MAX_SIZE + 1), remains % (BLOCK_MAX_SIZE + 1) - 1};
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blockState::blockState(
        const bool first, const bool last, const uint32_t first_ptr, const uint32_t last_ptr)
        : first_(first), last_(last), first_ptr(first_ptr), last_ptr(last_ptr) {}

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockState::size() const -> uint32_t {
        if (this->first_ && this->last_)
            return this->last_ptr + 1 - this->first_ptr;
        if (this->first_)
            return BLOCK_MAX_SIZE - this->first_ptr + 1;
        if (this->last_)
            return this->last_ptr + 1;
        return BLOCK_MAX_SIZE;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockState::contains(const TYPE&) const -> bool {
        return false;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockState::add(const TYPE &) -> void {}

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockState::clear() -> void {}

    template<typename TYPE>
    original::blocksList<TYPE>::blockItr::blockItr(TYPE *ptr, blockState *container, int64_t pos)
        : randomAccessIterator<TYPE>(ptr, container, pos) {}

    template<typename TYPE>
    original::blocksList<TYPE>::blockItr::blockItr(const blockItr &other)
        : randomAccessIterator<TYPE>(nullptr, nullptr, 0) {
        this.operator=(other);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockItr::operator=(const blockItr &other) -> blockItr& {
        if (this == &other) return *this;
        this->_ptr = other._ptr;
        this->_container = other._container;
        this->_pos = other._pos;
        return *this;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockItr::clone() const -> blockItr* {
        return new blockItr(*this);
    }

    template<typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(TYPE **ptr, const blocksList *container)
        : base_{ptr, container, 0}, cur_{*ptr, new blockState(
          true, blocksList::size() == 1, container->first, container->last)} {}

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::equalPtr(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const blocksList*>(other);
        return other_it != nullptr && this->base_ == other->base_ && this->cur_ == other->cur_;
    }

    template<typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(const Iterator &other)
        : randomAccessIterator<TYPE>(nullptr, nullptr, 0)
    {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::operator=(const Iterator &other) -> Iterator & {
        if (this == &other) {
            return *this;
        }
        randomAccessIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::hasNext() const -> bool {
        return this->base_.hasNext();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::hasPrev() const -> bool {
        return this->base_.hasPrev();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::next() const -> void {
        if (this->cur_->hasNext()) {
            this->cur_->next();
        } else {
            this->base_->next();
            delete this->cur_;
            this->cur_ = new blockItr(this->base_->get(), this->base_->_container, 0);
        }
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::prev() const -> void {
        if (this->cur_->hasPrev()) {
            this->cur_->prev();
        } else {
            this->base_->prev();
            delete this->cur_;
            this->cur_ = new blockItr(&this->base_->get()[BLOCK_MAX_SIZE - 1], this->base_->_container, BLOCK_MAX_SIZE - 1);
        }
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::getPrev() const -> Iterator* {
        auto* it = this->clone();
        it->next();
        return it;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::getNext() const -> Iterator* {
        auto* it = this->clone();
        it->prev();
        return it;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::get() -> TYPE& {
        if (!this->isValid()) throw outOfBoundError();
        return this->cur_->get();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::get() const -> TYPE
    {
        if (!this->isValid()) throw outOfBoundError();
        return this->cur_->get();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::set(const TYPE &data) -> void {
        if (!this->isValid()) throw outOfBoundError();
        this->cur_.set(data);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::isValid() const -> bool {
        return this->base_->isValid();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return this->_ptr + 1 == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it->_ptr + 1 == this->_ptr;
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
        auto inner_idx = this->toInnerIdx(this->parseNegIndex(index));
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
    auto original::blocksList<TYPE>::operator[](int64_t index) -> TYPE& {
        return this->get(index);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::set(int64_t index, const TYPE &e) -> void {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        auto inner_idx = this->toInnerIdx(this->parseNegIndex(index));
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
