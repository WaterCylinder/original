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
        static constexpr size_t BLOCK_EMPTY = 0;
        static constexpr size_t BLOCK_FULL = BLOCK_MAX_SIZE;
        size_t first;
        size_t last;

        static TYPE* blockArrayInit();
        [[nodiscard]] static size_t indexReset(size_t index, bool increase);
        [[nodiscard]] static size_t blockCurSize(size_t index);
        [[nodiscard]] couple<size_t, size_t> toInnerIdx(size_t index) const;
        void moveElements(size_t index, size_t len, int offset);
        void grow(bool first);
        void squeeze(bool first);
    public:
        class Iterator final : public randomAccessIterator<TYPE>
        {
            explicit Iterator(TYPE* ptr, const blocksList* container, long long pos);
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
        TYPE get(long long index) const override;
        [[nodiscard]] size_t size() const override;
        iterator<TYPE>* begins() const override;
        iterator<TYPE>* ends() const override;
        TYPE& operator[](long long index) override;
        void set(long long index, const TYPE &e) override;
        size_t indexOf(const TYPE &e) const override;
        [[nodiscard]] std::string className() const override;
    };
}// namespace original

    template<typename TYPE>
    auto original::blocksList<TYPE>::blockArrayInit() -> TYPE * {
        auto arr = new TYPE[BLOCK_MAX_SIZE];
        for (size_t i = 0; i < BLOCK_MAX_SIZE; i++) {
            arr[i] = TYPE{};
        }
        return arr;
    }

    template<typename TYPE>
    original::blocksList<TYPE>::Iterator::Iterator(TYPE *ptr, const blocksList *container, long long pos)
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
    auto original::blocksList<TYPE>::className() const -> std::string {
        return "blocksList";
    }

#endif //BLOCKSLIST_H
