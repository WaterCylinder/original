#ifndef BLOCKSLIST_H
#define BLOCKSLIST_H

#include "baseList.h"
#include "couple.h"
#include "vector.h"

/**
 * @file blocksList.h
 * @brief A block-based list implementation similar to std::deque.
 * @details This file provides the definition of the blocksList class, which is a list that stores elements
 *          in blocks. It efficiently handles operations such as insertion and removal at both ends, while
 *          maintaining a structure similar to std::deque. The class supports random access and iteration.
 */

namespace original {
    /**
     * @class blocksList
     * @tparam TYPE Type of elements stored in the blocksList
     * @brief A block-based list implementation.
     * @extends baseList
     * @extends iterationStream
     * @details The blocksList class is a container that stores elements in blocks, which makes it similar to
     *          std::deque. The class provides operations for insertion, deletion, and accessing elements
     *          from both ends. The internal structure consists of blocks of a fixed size, and elements are
     *          efficiently managed across these blocks. It also provides bidirectional iteration.
     */
    template <typename TYPE>
    class blocksList final : public baseList<TYPE>, public iterationStream<TYPE, blocksList<TYPE>> {
        static constexpr uint32_t BLOCK_MAX_SIZE = 16; ///< The maximum size of each block
        static constexpr uint32_t POS_INIT = (BLOCK_MAX_SIZE - 1) / 2 + 1; ///< Initial position in a block

        vector<TYPE*> map; ///< Vector to store blocks of elements
        uint32_t size_; ///< Current size of the list
        uint32_t first_; ///< Position of the first element
        uint32_t last_; ///< Position of the last element
        uint32_t first_block; ///< Block index of the first element
        uint32_t last_block; ///< Block index of the last element

        /**
         * @brief Initializes the blocksList with an initial block.
         */
        void blocksListInit();

        /**
         * @brief Destroys the blocksList by deleting all blocks.
         */
        void blocksListDestruct() const;

        /**
         * @brief Initializes a block array.
         * @return A pointer to the new block array.
         */
        static TYPE* blockArrayInit();

        /**
         * @brief Converts the block and position to an absolute index.
         * @param block The block index.
         * @param pos The position within the block.
         * @return The absolute index.
         */
        [[nodiscard]] static uint32_t innerIdxToAbsIdx(uint32_t block, uint32_t pos);

        /**
         * @brief Gets the absolute index of the first element.
         * @return The absolute index of the first element.
         */
        [[nodiscard]] uint32_t firstAbsIdx() const;

        /**
         * @brief Gets the absolute index of the last element.
         * @return The absolute index of the last element.
         */
        [[nodiscard]] uint32_t lastAbsIdx() const;

        /**
         * @brief Converts an absolute index to an outer index.
         * @param absIdx The absolute index.
         * @return The corresponding outer index.
         */
        [[nodiscard]] int64_t absIdxToOuterIdx(uint32_t absIdx) const;

        /**
         * @brief Converts an outer index to an absolute index.
         * @param outerIdx The outer index.
         * @return The corresponding absolute index.
         */
        [[nodiscard]] uint32_t outerIdxToAbsIdx(int64_t outerIdx) const;

        /**
         * @brief Converts an absolute index to a pair of block and position.
         * @param absIdx The absolute index.
         * @return A pair of block index and position within the block.
         */
        [[nodiscard]] static couple<uint32_t, uint32_t> absIdxToInnerIdx(uint32_t absIdx);

        /**
         * @brief Computes the new block and position after applying an offset.
         * @param block The current block index.
         * @param pos The current position within the block.
         * @param offset The offset to apply.
         * @return A pair of the new block index and position.
         */
        [[nodiscard]] static couple<uint32_t, uint32_t> innerIdxOffset(uint32_t block, uint32_t pos, int64_t offset);

        /**
         * @brief Converts an outer index to a block and position pair.
         * @param outerIdx The outer index.
         * @return A pair of block index and position within the block.
         */
        [[nodiscard]] couple<uint32_t, uint32_t> outerIdxToInnerIdx(int64_t outerIdx) const;

        /**
         * @brief Converts a block and position to an outer index.
         * @param block The block index.
         * @param pos The position within the block.
         * @return The corresponding outer index.
         */
        [[nodiscard]] int64_t innerIdxToOuterIdx(uint32_t block, uint32_t pos) const;

        /**
         * @brief Gets the element at a given block and position.
         * @param block The block index.
         * @param pos The position within the block.
         * @return A reference to the element at the specified position.
         */
        TYPE& getElem(uint32_t block, uint32_t pos) const;

        /**
         * @brief Sets the element at a given block and position.
         * @param block The block index.
         * @param pos The position within the block.
         * @param e The value to set at the specified position.
         */
        void setElem(uint32_t block, uint32_t pos, const TYPE& e);

        /**
         * @brief Determines whether the blocksList needs to grow.
         * @param increment The number of elements to add.
         * @param is_first Flag indicating whether it is the first element being added.
         * @return True if growth is needed, false otherwise.
         */
        [[nodiscard]] bool growNeeded(uint32_t increment, bool is_first) const;

        /**
         * @brief Moves elements from one position to another within the list.
         * @param start_block The block where the move starts.
         * @param start_pos The position where the move starts.
         * @param len The number of elements to move.
         * @param offset The offset to apply to the positions.
         */
        void moveElements(uint32_t start_block, uint32_t start_pos, uint32_t len, int64_t offset);

        /**
         * @brief Adds a new block to the blocksList.
         * @param is_first Flag indicating whether the block should be added at the beginning or the end.
         */
        void addBlock(bool is_first);

        /**
         * @brief Adjusts the blocksList to accommodate a specified number of elements.
         * @param increment The number of elements to accommodate.
         * @param is_first Flag indicating whether the adjustment is for the first or last block.
         */
        void adjust(uint32_t increment, bool is_first);

    public:
        /**
         * @class Iterator
         * @brief Iterator for blocksList, supports forward and backward iteration.
         * @extends baseIterator
         * @details This iterator allows iterating through the blocksList with random access.
         */
        class Iterator final : public baseIterator<TYPE> {
            mutable int64_t cur_pos; ///< Current position within the block
            mutable int64_t cur_block; ///< Current block index
            mutable TYPE** data_; ///< Pointer to the block data
            const blocksList* container_; ///< Pointer to the containing blocksList

            /**
             * @brief Constructs an iterator for the blocksList.
             * @param pos The position of the element.
             * @param block The block index of the element.
             * @param data_ptr The pointer to the block data.
             * @param container The blocksList container.
             */
            explicit Iterator(int64_t pos, int64_t block, TYPE** data_ptr, const blocksList* container);

            /**
             * @brief Checks if two iterators point to the same element.
             * @param other The other iterator to compare.
             * @return True if the iterators point to the same element, false otherwise.
             */
            bool equalPtr(const iterator<TYPE>* other) const override;

        public:
            friend blocksList;

            /**
             * @brief Copy constructor for the Iterator.
             * @param other The iterator to copy.
             */
            Iterator(const Iterator& other);

            /**
             * @brief Assignment operator for the Iterator.
             * @param other The iterator to assign.
             * @return A reference to this iterator.
             */
            Iterator& operator=(const Iterator& other);

            /**
             * @brief Clones the iterator.
             * @return A new iterator pointing to the same element.
             */
            Iterator* clone() const override;

            /**
             * @brief Checks if there is a next element.
             * @return True if there is a next element, false otherwise.
             */
            [[nodiscard]] bool hasNext() const override;

            /**
             * @brief Checks if there is a previous element.
             * @return True if there is a previous element, false otherwise.
             */
            [[nodiscard]] bool hasPrev() const override;

            /**
             * @brief Moves the iterator to the next element.
             */
            void next() const override;

            /**
             * @brief Moves the iterator to the previous element.
             */
            void prev() const override;

            /**
             * @brief Advances the iterator by the specified number of steps.
             * @param steps The number of steps to advance.
             */
            void operator+=(int64_t steps) const override;

            /**
             * @brief Moves the iterator backward by the specified number of steps.
             * @param steps The number of steps to move backward.
             */
            void operator-=(int64_t steps) const override;

            /**
             * @brief Computes the distance between two iterators.
             * @param other The other iterator to compare.
             * @return The distance between the two iterators.
             */
            int64_t operator-(const iterator<TYPE>& other) const override;

            /**
             * @brief Gets the previous iterator.
             * @return A new iterator pointing to the previous element.
             */
            Iterator* getPrev() const override;

            /**
             * @brief Gets the next iterator.
             * @return A new iterator pointing to the next element.
             */
            Iterator* getNext() const override;

            /**
             * @brief Gets the element pointed to by the iterator.
             * @return A reference to the element.
             */
            TYPE& get() override;

            /**
             * @brief Gets the element pointed to by the iterator (const version).
             * @return A const reference to the element.
             */
            TYPE get() const override;

            /**
             * @brief Sets the value of the element pointed to by the iterator.
             * @param data The value to set.
             */
            void set(const TYPE& data) override;

            /**
             * @brief Checks if the iterator is valid.
             * @return True if the iterator is valid, false otherwise.
             */
            [[nodiscard]] bool isValid() const override;

            /**
             * @brief Checks if the iterator is at the previous element relative to another iterator.
             * @param other The other iterator to compare.
             * @return True if the iterator is at the previous element, false otherwise.
             */
            bool atPrev(const iterator<TYPE>* other) const override;

            /**
             * @brief Checks if the iterator is at the next element relative to another iterator.
             * @param other The other iterator to compare.
             * @return True if the iterator is at the next element, false otherwise.
             */
            bool atNext(const iterator<TYPE>* other) const override;

            /**
             * @brief Gets the class name of the iterator.
             * @return The class name as a string.
             */
            [[nodiscard]] std::string className() const override;
        };

        friend Iterator;

        /**
         * @brief Default constructor for blocksList.
         */
        explicit blocksList();

        /**
         * @brief Constructs a blocksList from an initializer list.
         * @param lst The initializer list to construct the blocksList from.
         */
        blocksList(const std::initializer_list<TYPE>& lst);

        /**
         * @brief Constructs a blocksList from an array.
         * @param arr The array to construct the blocksList from.
         */
        explicit blocksList(const array<TYPE>& arr);

        /**
         * @brief Copy constructor for blocksList.
         * @param other The blocksList to copy from.
         */
        blocksList(const blocksList& other);

        /**
         * @brief Assignment operator for blocksList.
         * @param other The blocksList to assign from.
         * @return A reference to this blocksList.
         */
        blocksList& operator=(const blocksList& other);

        /**
         * @brief Move constructor for blocksList.
         * @param other The blocksList to move from.
         */
        blocksList(blocksList&& other) noexcept;

        /**
         * @brief Move assignment operator for blocksList.
         * @param other The blocksList to move from.
         * @return A reference to this blocksList.
         */
        blocksList& operator=(blocksList&& other) noexcept;

        /**
         * @brief Gets the element at the specified index.
         * @param index The index of the element to retrieve.
         * @return The element at the specified index.
         */
        TYPE get(int64_t index) const override;

        /**
         * @brief Gets the size of the blocksList.
         * @return The size of the blocksList.
         */
        [[nodiscard]] uint32_t size() const override;

        /**
         * @brief Gets an iterator to the beginning of the blocksList.
         * @return An iterator to the beginning of the blocksList.
         */
        Iterator* begins() const override;

        /**
         * @brief Gets an iterator to the end of the blocksList.
         * @return An iterator to the end of the blocksList.
         */
        Iterator* ends() const override;

        /**
         * @brief Gets a reference to the element at the specified index.
         * @param index The index of the element to retrieve.
         * @return A reference to the element at the specified index.
         */
        TYPE& operator[](int64_t index) override;

        /**
         * @brief Sets the element at the specified index.
         * @param index The index of the element to set.
         * @param e The value to set the element to.
         */
        void set(int64_t index, const TYPE& e) override;

        /**
         * @brief Finds the index of the first occurrence of the specified element.
         * @param e The element to search for.
         * @return The index of the element, or the size of the blocksList if not found.
         */
        uint32_t indexOf(const TYPE& e) const override;

        /**
         * @brief Pushes an element to the specified index in the blocksList.
         * @param index The index at which to insert the element.
         * @param e The element to insert.
         */
        void push(int64_t index, const TYPE& e) override;

        /**
         * @brief Pops the element at the specified index in the blocksList.
         * @param index The index of the element to pop.
         * @return The element that was popped.
         */
        TYPE pop(int64_t index) override;

        /**
         * @brief Pushes an element to the beginning of the blocksList.
         * @param e The element to push.
         */
        void pushBegin(const TYPE& e) override;

        /**
         * @brief Pops the element from the beginning of the blocksList.
         * @return The element that was popped.
         */
        TYPE popBegin() override;

        /**
         * @brief Pushes an element to the end of the blocksList.
         * @param e The element to push.
         */
        void pushEnd(const TYPE& e) override;

        /**
         * @brief Pops the element from the end of the blocksList.
         * @return The element that was popped.
         */
        TYPE popEnd() override;

        /**
         * @brief Gets the class name of the blocksList.
         * @return The class name as a string.
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Destructor for the blocksList.
         */
        ~blocksList() override;
    };
}// namespace original

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListInit() -> void
    {
        this->map = vector({blockArrayInit()});
        this->size_ = 0;
        this->first_ = POS_INIT + 1;
        this->last_ = POS_INIT;
        this->first_block = this->map.size() / 2;
        this->last_block = this->map.size() / 2;
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::blocksListDestruct() const -> void
    {
        for (auto* block : this->map) {
            delete[] block;
        }
    }

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
        return innerIdxToAbsIdx(this->first_block, this->first_);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::lastAbsIdx() const -> uint32_t
    {
        return innerIdxToAbsIdx(this->last_block, this->last_);
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
    auto original::blocksList<TYPE>::innerIdxOffset(const uint32_t block, const uint32_t pos,
                                                    const int64_t offset) -> couple<uint32_t, uint32_t>
    {
        return absIdxToInnerIdx(static_cast<uint32_t>(static_cast<int64_t>(innerIdxToAbsIdx(block, pos)) + offset));
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::outerIdxToInnerIdx(const int64_t outerIdx) const -> couple<uint32_t, uint32_t>
    {
        return absIdxToInnerIdx(this->outerIdxToAbsIdx(outerIdx));
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::innerIdxToOuterIdx(const uint32_t block, const uint32_t pos) const -> int64_t
    {
        return this->absIdxToOuterIdx(innerIdxToAbsIdx(block, pos));
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::getElem(uint32_t block, uint32_t pos) const -> TYPE&
    {
        return this->map.get(block)[pos];
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::setElem(uint32_t block, uint32_t pos, const TYPE& e) -> void
    {
        this->map.get(block)[pos] = e;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::growNeeded(const uint32_t increment, bool is_first) const -> bool
    {
        return is_first ? firstAbsIdx() < increment
        : lastAbsIdx() + increment > innerIdxToAbsIdx(this->map.size() - 1, BLOCK_MAX_SIZE - 1);
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::moveElements(const uint32_t start_block, const uint32_t start_pos,
                                                  const uint32_t len, const int64_t offset) -> void
    {
        if (offset > 0)
        {
            for (uint32_t i = 0; i < len; i++)
            {
                auto idx = innerIdxOffset(start_block, start_pos, len - 1 - i);
                auto idx_offset = innerIdxOffset(start_block, start_pos, len - 1 - i + offset);
                this->setElem(idx_offset.first(), idx_offset.second(),
                              this->getElem(idx.first(), idx.second()));
            }
        }else
        {
            for (uint32_t i = 0; i < len; i++)
            {
                auto idx = innerIdxOffset(start_block, start_pos, i);
                auto idx_offset = innerIdxOffset(start_block, start_pos, i + offset);
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
    auto original::blocksList<TYPE>::adjust(const uint32_t increment, const bool is_first) -> void
    {
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

        this->cur_pos = other.cur_pos;
        this->cur_block = other.cur_block;
        this->data_ = other.data_;
        this->container_ = other.container_;
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
    auto original::blocksList<TYPE>::Iterator::operator+=(const int64_t steps) const -> void
    {
        auto new_idx = innerIdxOffset(this->cur_block, this->cur_pos, steps);
        this->cur_block = new_idx.first();
        this->cur_pos = new_idx.second();
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::Iterator::operator-=(const int64_t steps) const -> void
    {
        auto new_idx = innerIdxOffset(this->cur_block, this->cur_pos, -steps);
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

        return static_cast<int64_t>(innerIdxToAbsIdx(this->cur_block, this->cur_pos)) -
               static_cast<int64_t>(innerIdxToAbsIdx(other_it->cur_block, other_it->cur_pos));
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
        return this->container_->innerIdxToOuterIdx(this->cur_block, this->cur_pos) >= 0 &&
               this->container_->innerIdxToOuterIdx(this->cur_block, this->cur_pos) < this->container_->size();
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
        : map(), size_(), first_(), last_(), first_block(), last_block()
    {
        this->blocksListInit();
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blocksList(const std::initializer_list<TYPE>& lst) : blocksList() {
        this->adjust(lst.size(), false);
        for (const auto& e : lst) {
            auto new_idx = innerIdxOffset(this->last_block, this->last_, 1);
            this->last_block = new_idx.first();
            this->last_ = new_idx.second();
            this->setElem(this->last_block, this->last_, e);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blocksList(const array<TYPE>& arr) : blocksList() {
        this->adjust(arr.size(), false);
        for (const auto& e : arr) {
            auto new_idx = innerIdxOffset(this->last_block, this->last_, 1);
            this->last_block = new_idx.first();
            this->last_ = new_idx.second();
            this->setElem(this->last_block, this->last_, e);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    original::blocksList<TYPE>::blocksList(const blocksList& other) : blocksList() {
        this->operator=(other);
    }

    template<typename TYPE>
    original::blocksList<TYPE>& original::blocksList<TYPE>::operator=(const blocksList& other) {
        if (this == &other) return *this;

        this->map = vector<TYPE*>{};

        for (int64_t i = 0; i < other.map.size(); ++i) {
            auto* block = blockArrayInit();
            for (uint32_t j = 0; j < BLOCK_MAX_SIZE; ++j) {
                block[j] = other.getElem(i, j);
            }
            this->map.pushEnd(block);
        }

        this->first_ = other.first_;
        this->last_ = other.last_;
        this->size_ = other.size_;
        this->first_block = other.first_block;
        this->last_block = other.last_block;
        return *this;
    }

    template <typename TYPE>
    original::blocksList<TYPE>::blocksList(blocksList&& other) noexcept : blocksList()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE>
    auto original::blocksList<TYPE>::operator=(blocksList&& other) noexcept -> blocksList&
    {
        if (this == &other)
            return *this;

        this->blocksListDestruct();

        this->map = std::move(other.map);
        this->first_ = other.first_;
        this->last_ = other.last_;
        this->first_block = other.first_block;
        this->last_block = other.last_block;
        this->size_ = other.size_;
        other.blocksListDestruct();
        return *this;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::get(int64_t index) const -> TYPE {
        if (this->indexOutOfBound(this->parseNegIndex(index))) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto inner_idx = this->outerIdxToInnerIdx(index);
        return this->getElem(inner_idx.first(), inner_idx.second());
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::size() const -> uint32_t {
        return this->size_;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->first_, this->first_block, &this->map.data(), this);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->last_, this->last_block, &this->map.data(), this);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::operator[](int64_t index) -> TYPE& {
        if (this->indexOutOfBound(this->parseNegIndex(index))) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto inner_idx = this->outerIdxToInnerIdx(index);
        return this->getElem(inner_idx.first(), inner_idx.second());
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::set(int64_t index, const TYPE &e) -> void {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto inner_idx = this->outerIdxToInnerIdx(index);
        this->setElem(inner_idx.first(), inner_idx.second(), e);
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::indexOf(const TYPE &e) const -> uint32_t {
        for (uint32_t i = 0; i < this->size(); ++i) {
            if (auto idx = this->outerIdxToInnerIdx(i);
                this->getElem(idx.first(), idx.second()) == e)
                    return i;
        }
        return this->size();
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::push(int64_t index, const TYPE& e) -> void
    {
        if (this->parseNegIndex(index) == this->size())
        {
            this->pushEnd(e);
        } else if (this->parseNegIndex(index) == 0)
        {
            this->pushBegin(e);
        } else{
            if (this->indexOutOfBound(index))
                throw outOfBoundError();

            index = this->parseNegIndex(index);
            const bool is_first = index <= (this->size() - 1) / 2;
            this->adjust(1, is_first);
            if (is_first){
                this->moveElements(this->first_block, this->first_, index + 1, -1);
                auto new_idx = innerIdxOffset(this->first_block, this->first_, -1);
                this->first_block = new_idx.first();
                this->first_ = new_idx.second();
            } else{
                auto idx = outerIdxToInnerIdx(index);
                this->moveElements(idx.first(), idx.second(), this->size() - index, 1);
                auto new_idx = innerIdxOffset(this->last_block, this->last_, 1);
                this->last_block = new_idx.first();
                this->last_ = new_idx.second();
            }
            this->size_ += 1;
            auto idx = outerIdxToInnerIdx(index);
            this->setElem(idx.first(), idx.second(), e);
        }
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::pop(int64_t index) -> TYPE
    {
        if (this->parseNegIndex(index) == 0)
            return this->popBegin();
        if (this->parseNegIndex(index) == this->size() - 1)
            return this->popEnd();
        if (this->indexOutOfBound(index))
            throw outOfBoundError();

        index = this->parseNegIndex(index);
        auto idx = outerIdxToInnerIdx(index);
        TYPE res = this->getElem(idx.first(), idx.second());
        if (index <= (this->size() - 1) / 2){
            moveElements(this->first_block, this->first_, index, 1);
            auto new_idx = innerIdxOffset(this->first_block, this->first_, 1);
            this->first_block = new_idx.first();
            this->first_ = new_idx.second();
        } else{
            auto idx_offset = innerIdxOffset(idx.first(), idx.second(), 1);
            moveElements(idx_offset.first(), idx_offset.second(), this->size() - 1 - index, -1);
            auto new_idx = innerIdxOffset(this->last_block, this->last_, -1);
            this->last_block = new_idx.first();
            this->last_ = new_idx.second();
        }
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::pushBegin(const TYPE& e) -> void
    {
        this->adjust(1, true);
        auto new_idx = innerIdxOffset(this->first_block, this->first_, -1);
        this->first_block = new_idx.first();
        this->first_ = new_idx.second();
        this->setElem(this->first_block, this->first_, e);
        this->size_ += 1;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::popBegin() -> TYPE
    {
        if (this->empty()) throw noElementError();

        TYPE res = this->getElem(this->first_block, this->first_);
        auto new_idx = innerIdxOffset(this->first_block, this->first_, 1);
        this->first_block = new_idx.first();
        this->first_ = new_idx.second();
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::pushEnd(const TYPE& e) -> void
    {
        this->adjust(1, false);
        auto new_idx = innerIdxOffset(this->last_block, this->last_, 1);
        this->last_block = new_idx.first();
        this->last_ = new_idx.second();
        this->setElem(this->last_block, this->last_, e);
        this->size_ += 1;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::popEnd() -> TYPE
    {
        if (this->empty()) throw noElementError();

        TYPE res = this->getElem(this->last_block, this->last_);
        auto new_idx = innerIdxOffset(this->last_block, this->last_, -1);
        this->last_block = new_idx.first();
        this->last_ = new_idx.second();
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::blocksList<TYPE>::className() const -> std::string {
        return "blocksList";
    }

    template<typename TYPE>
    original::blocksList<TYPE>::~blocksList() {
        this->blocksListDestruct();
    }

#endif //BLOCKSLIST_H
