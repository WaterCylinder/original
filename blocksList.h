#ifndef BLOCKSLIST_H
#define BLOCKSLIST_H

#include <couple.h>
#include <iterationStream.h>
#include "serial.h"

namespace original {
    // todo
    template<typename TYPE>
    class blocksList final : public serial<TYPE>, public iterationStream<TYPE>{
        vector<TYPE*> map;
        size_t size_;
        static constexpr size_t BLOCK_MAX_SIZE = 16;
        static constexpr size_t POS_INIT = (BLOCK_MAX_SIZE - 1) / 2 + 1;
        size_t first;
        size_t last;

        static TYPE* blockArrayInit();
        [[nodiscard]] couple<size_t, size_t> toInnerIdx(int64_t index) const;
        void moveElements(size_t index, size_t len, int offset);
        void grow(bool first);
        void shrink(bool first);
    public:
        class Iterator final : public randomAccessIterator<TYPE>
        {
            explicit Iterator(TYPE* ptr, const blocksList* container, int64_t pos);
        public:
            friend blocksList;
            Iterator(const Iterator& other);
            Iterator& operator=(const Iterator& other);
            Iterator* clone() const override;
            bool atPrev(const iterator<TYPE> *other) const override;
            bool atNext(const iterator<TYPE> *other) const override;
            [[nodiscard]] std::string className() const override;
        };

        explicit blocksList();
        TYPE get(int64_t index) const override;
        [[nodiscard]] size_t size() const override;
        iterator<TYPE>* begins() const override;
        iterator<TYPE>* ends() const override;
        TYPE& operator[](int64_t index) override;
        void set(int64_t index, const TYPE &e) override;
        size_t indexOf(const TYPE &e) const override;
        [[nodiscard]] std::string className() const override;
    };
}// namespace original

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockArrayInit() -> TYPE* {
        auto arr = new TYPE[BLOCK_MAX_SIZE];
        for (size_t i = 0; i < BLOCK_MAX_SIZE; i++) {
            arr[i] = TYPE{};
        }
        return arr;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::toInnerIdx(int64_t index) const -> couple<size_t, size_t> {
        const size_t first_size = BLOCK_MAX_SIZE - this->first;
        if (this->map.size() == 1 || index + 1 <= first_size) return {0, this->first + index};

        const size_t remains = index + 1 - first_size;
        return {1 + remains / (BLOCK_MAX_SIZE + 1), remains % (BLOCK_MAX_SIZE + 1) - 1};
    }

    template<typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(TYPE *ptr, const blocksList *container, int64_t pos)
        : randomAccessIterator<TYPE>::Iterator(ptr, container, pos) {}

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
    auto original::blocksList<TYPE>::size() const -> size_t {
        return this->size_;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::set(int64_t index, const TYPE &e) -> void {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        auto inner_idx = this->toInnerIdx(this->parseNegIndex(index));
        this->map.get(inner_idx.first())[inner_idx.second()] = e;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::className() const -> std::string {
        return "blocksList";
    }

#endif //BLOCKSLIST_H
