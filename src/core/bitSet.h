#ifndef BITSET_H
#define BITSET_H
#include "array.h"
#include "couple.h"
#include "iterationStream.h"


namespace original {
    /**
     * @file bitSet.h
     * @brief BitSet class declaration.
     * @details This file contains the declaration of the bitSet class, which implements a
     *          space-efficient data structure for storing a set of bits.
     */

    /**
     * @class bitSet
     * @brief A class representing a set of bits, offering functionality to manipulate and query individual bits.
     * @extends baseArray
     * @extends iterationStream
     * @details The bitSet class allows efficient manipulation of individual bits using bitwise operations.
     *          It utilizes a dynamic array of 64-bit blocks to store bits and provides methods to access
     *          and modify them. Iterators are available for traversing through the bits.
     */
    template<template <typename> typename ALLOC = allocator>
    class bitSet final : public baseArray<bool, ALLOC<bool>>, public iterationStream<bool, bitSet<ALLOC>>{
            using underlying_type = u_integer;

            static constexpr integer BLOCK_MAX_SIZE = sizeof(underlying_type) * 8; ///< Maximum number of bits in a block.
            array<underlying_type, ALLOC<underlying_type>> map; ///< Array to store the blocks of bits.
            u_integer size_; ///< The total number of bits in the set.
            ALLOC<underlying_type> allocator_underlying;


            /**
             * @brief Initializes the bitSet with the given size.
             * @param size The size of the bitSet.
             */
            void bitsetInit(u_integer size);

            /**
            * @brief Gets the value of a specific bit in a block.
            * @param block_value The block containing the bit.
            * @param bit The bit index within the block.
            * @return The value of the bit (true or false).
            */
            [[nodiscard]] static bool getBitFromBlock(underlying_type block_value, integer bit);

            /**
             * @brief Sets a specific bit in a block.
             * @param block_value The block containing the bit.
             * @param bit The bit index within the block.
             * @return The new block value with the bit set.
             */
            [[nodiscard]] static underlying_type setBitFromBlock(underlying_type block_value, integer bit);

            /**
             * @brief Clears a specific bit in a block.
             * @param block_value The block containing the bit.
             * @param bit The bit index within the block.
             * @return The new block value with the bit cleared.
             */
            [[nodiscard]] static underlying_type clearBitFromBlock(underlying_type block_value, integer bit);

            /**
             * @brief Clears the higher bits in a block beyond a specified index.
             * @param block_value The block containing the bits.
             * @param bit The index to clear higher bits from.
             * @return The new block value with higher bits cleared.
             */
            [[nodiscard]] static underlying_type clearHigherBitsFromBlock(underlying_type block_value, integer bit);

            /**
             * @brief Clears any redundant bits (bits beyond the logical size of the set).
             */
            void clearRedundantBits();

            /**
             * @brief Gets the value of a specific bit in the bitSet.
             * @param bit The index of the bit within the block.
             * @param block The block index containing the bit.
             * @return The value of the bit (true or false).
             */
            [[nodiscard]] bool getBit(integer bit, integer block) const;

            /**
             * @brief Sets a specific bit in the bitSet.
             * @param bit The index of the bit to set.
             * @param block The block index to set the bit in.
             */
            void setBit(integer bit, integer block);

            /**
             * @brief Clears a specific bit in the bitSet.
             * @param bit The index of the bit to clear.
             * @param block The block index to clear the bit in.
             */
            void clearBit(integer bit, integer block);

            /**
             * @brief Writes a specific bit in the bitSet (sets or clears based on value).
             * @param bit The index of the bit.
             * @param block The block index.
             * @param value The value to set the bit to (true or false).
             */
            void writeBit(integer bit, integer block, bool value);

            /**
             * @brief Converts an index to an inner block and bit index.
             * @param index The global index.
             * @return A couple containing the block index and bit index within the block.
             */
            static couple<u_integer, integer> toInnerIdx(integer index);

            /**
             * @brief Converts inner block and bit indices to a global index.
             * @param cur_block The block index.
             * @param cur_bit The bit index within the block.
             * @return The global index.
             */
            static integer toOuterIdx(u_integer cur_block, integer cur_bit);

        public:

        /**
         * @class Iterator
         * @brief An iterator for traversing the bits in a bitSet.
         * @extends baseIterator
         * @details This iterator allows iteration over the bits of a bitSet, providing functionality to
         *          move forward, backward, and access or modify the bits.
         */
        class Iterator final : public baseIterator<bool> {
                mutable integer cur_bit; ///< The current bit index.
                mutable integer cur_block; ///< The current block index.
                mutable underlying_type* block_; ///< Pointer to the current block.
                const bitSet* container_; ///< Pointer to the containing bitSet.

                /**
                 * @brief Constructs an iterator.
                 * @param bit The starting bit index.
                 * @param block The starting block index.
                 * @param block_p Pointer to the block data.
                 * @param container The bitSet container the iterator belongs to.
                 */
                explicit Iterator(integer bit, integer block, underlying_type* block_p, const bitSet* container);

                /**
                 * @brief Checks if two iterators are equal.
                 * @param other The iterator to compare to.
                 * @return True if the iterators are equal, false otherwise.
                 */
                bool equalPtr(const iterator *other) const override;

            public:
                friend class bitSet;

                /**
                 * @brief Clones the iterator.
                 * @return A new iterator pointing to the same position.
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
                 * @brief Checks if the iterator is at the previous element.
                 * @param other The other iterator to compare.
                 * @return True if this iterator is at the previous element relative to the other iterator.
                 */
                bool atPrev(const iterator *other) const override;

                /**
                 * @brief Checks if the iterator is at the next element.
                 * @param other The other iterator to compare.
                 * @return True if this iterator is at the next element relative to the other iterator.
                 */
                bool atNext(const iterator *other) const override;

                /**
                 * @brief Moves the iterator to the next element.
                 */
                void next() const override;

                /**
                 * @brief Moves the iterator to the previous element.
                 */
                void prev() const override;

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
                 * @brief Advances the iterator by the given number of steps.
                 * @param steps The number of steps to move forward.
                 */
                void operator+=(integer steps) const override;

                /**
                 * @brief Moves the iterator backward by the given number of steps.
                 * @param steps The number of steps to move backward.
                 */
                void operator-=(integer steps) const override;

                /**
                 * @brief Computes the distance between two iterators.
                 * @param other The other iterator.
                 * @return The distance between the two iterators in terms of the number of elements.
                 */
                integer operator-(const iterator& other) const override;

                /**
                 * @brief Gets the value of the current bit.
                 * @return A reference to the current bit.
                 */
                bool& get() override;

                /**
                 * @brief Returns the class name of this iterator.
                 * @return A string representing the class name, which is "bitset::Iterator".
                 */
                [[nodiscard]] std::string className() const override;

                /**
                 * @brief Gets the value of the current bit (const version).
                 * @return The value of the current bit.
                 */
                [[nodiscard]] bool get() const override;

                /**
                 * @brief Sets the value of the current bit.
                 * @param data The value to set the bit to.
                 */
                void set(const bool &data) override;

                /**
                 * @brief Checks if the iterator is valid.
                 * @return True if the iterator is valid, false otherwise.
                 */
                [[nodiscard]] bool isValid() const override;
        };

            /**
             * @brief Constructs a bitSet with the given size.
             * @param size The size of the bitSet.
             */
            explicit bitSet(u_integer size, const ALLOC<underlying_type>& allocator = ALLOC<underlying_type>{});

            /**
             * @brief Constructs a bitSet from an initializer list.
             * @param lst The initializer list of boolean values.
             */
            bitSet(const std::initializer_list<bool>& lst);

            /**
             * @brief Copy constructor.
             * @param other The bitSet to copy.
             */
            bitSet(const bitSet& other);

            /**
             * @brief Copy assignment operator.
             * @param other The bitSet to copy.
             * @return A reference to this bitSet.
             */
            bitSet& operator=(const bitSet& other);

            /**
             * @brief Move constructor.
             * @param other The bitSet to move.
             */
            bitSet(bitSet&& other) noexcept;

            /**
             * @brief Move assignment operator.
             * @param other The bitSet to move.
             * @return A reference to this bitSet.
             */
            bitSet& operator=(bitSet&& other) noexcept;

            /**
             * @brief Counts the number of bits set to true.
             * @return The count of true bits.
             */
            [[nodiscard]] u_integer count() const;

            /**
             * @brief Resizes the bitSet to the given size.
             * @param new_size The new size for the bitSet.
             * @return A new resized bitSet.
             */
            [[nodiscard]] bitSet resize(u_integer new_size) const;

            /**
             * @brief Gets the size of the bitSet.
             * @return The size of the bitSet.
             */
            [[nodiscard]] u_integer size() const override;

            /**
             * @brief Gets the iterator to the beginning of the bitSet.
             * @return An iterator pointing to the beginning.
             */
            [[nodiscard]] Iterator* begins() const override;

            /**
             * @brief Gets the iterator to the end of the bitSet.
             * @return An iterator pointing to the end.
             */
            [[nodiscard]] Iterator* ends() const override;

            /**
             * @brief Gets the value of a specific bit by index.
             * @param index The index of the bit.
             * @return The value of the bit.
             */
            [[nodiscard]] bool get(integer index) const override;

            /**
             * @brief Gets the reference of a specific bit by index.
             * @param index The index of the bit.
             * @throw original::unSupportedMethodError
             */
            bool& operator[](integer index) override;

            /**
             * @brief Sets the value of a specific bit by index.
             * @param index The index of the bit.
             * @param e The value to set the bit to.
             */
            void set(integer index, const bool &e) override;

            /**
             * @brief Finds the index of the first occurrence of a specific value.
             * @param e The value to find.
             * @return The index of the first occurrence.
             */
            [[nodiscard]] u_integer indexOf(const bool &e) const override;

            ~bitSet() override = default;

            /**
             * @brief Performs a bitwise AND operation between two bitSets.
             * @param other The bitSet to AND with.
             * @return The result of the AND operation.
             */
            bitSet& operator&=(const bitSet& other);

            /**
             * @brief Performs a bitwise OR operation between two bitSets.
             * @param other The bitSet to OR with.
             * @return The result of the OR operation.
             */
            bitSet& operator|=(const bitSet& other);

            /**
             * @brief Performs a bitwise XOR operation between two bitSets.
             * @param other The bitSet to XOR with.
             * @return The result of the XOR operation.
             */
            bitSet& operator^=(const bitSet& other);

            /**
             * @brief Gets the class name for the bitSet.
             * @return The class name as a string.
             */
            [[nodiscard]] std::string className() const override;


            template<typename Callback = transform<bool>>
            void forEach(Callback operation = Callback{}) = delete;

            /**
             * @brief Performs a bitwise AND operation between two bitSets.
             * @param lbs The left bitSet.
             * @param rbs The right bitSet.
             * @return The result of the AND operation.
             */
            template<template <typename> typename ALLOC_>
            friend bitSet<ALLOC_> operator&(const bitSet<ALLOC_>& lbs, const bitSet<ALLOC_>& rbs);

            /**
             * @brief Performs a bitwise OR operation between two bitSets.
             * @param lbs The left bitSet.
             * @param rbs The right bitSet.
             * @return The result of the OR operation.
             */
            template<template <typename> typename ALLOC_>
            friend bitSet<ALLOC_> operator|(const bitSet<ALLOC_>& lbs, const bitSet<ALLOC_>& rbs);

            /**
             * @brief Performs a bitwise XOR operation between two bitSets.
             * @param lbs The left bitSet.
             * @param rbs The right bitSet.
             * @return The result of the XOR operation.
             */
            template<template <typename> typename ALLOC_>
            friend bitSet<ALLOC_> operator^(const bitSet<ALLOC_>& lbs, const bitSet<ALLOC_>& rbs);

            /**
             * @brief Performs a bitwise NOT operation on a bitSet.
             * @param bs The bitSet to negate.
             * @return The result of the NOT operation.
             */
            template<template <typename> typename ALLOC_>
            friend bitSet<ALLOC_> operator~(const bitSet<ALLOC_>& bs);
    };

    template<template <typename> typename ALLOC_>
    bitSet<ALLOC_> operator&(const bitSet<ALLOC_>& lbs, const bitSet<ALLOC_>& rbs);

    template<template <typename> typename ALLOC_>
    bitSet<ALLOC_> operator|(const bitSet<ALLOC_>& lbs, const bitSet<ALLOC_>& rbs);

    template<template <typename> typename ALLOC_>
    bitSet<ALLOC_> operator^(const bitSet<ALLOC_>& lbs, const bitSet<ALLOC_>& rbs);

    template<template <typename> typename ALLOC_>
    bitSet<ALLOC_> operator~(const bitSet<ALLOC_>& bs);
}

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::bitsetInit(const u_integer size) -> void
    {
        this->map = array<underlying_type, ALLOC<underlying_type>>((size + BLOCK_MAX_SIZE - 1) / BLOCK_MAX_SIZE, this->allocator_underlying);
        this->size_ = size;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::getBitFromBlock(const underlying_type block_value, const integer bit) -> bool {
        return block_value & static_cast<underlying_type>(1) << bit;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::setBitFromBlock(const underlying_type block_value, const integer bit) -> underlying_type {
        return block_value | static_cast<underlying_type>(1) << bit;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::clearBitFromBlock(const underlying_type block_value, const integer bit) -> underlying_type {
        return block_value & ~(static_cast<underlying_type>(1) << bit);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::clearHigherBitsFromBlock(const underlying_type block_value, const integer bit) -> underlying_type
    {
        return block_value & (static_cast<underlying_type>(1) << (bit + 1)) - static_cast<underlying_type>(1);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::clearRedundantBits() -> void
    {
        this->map.set(-1, clearHigherBitsFromBlock(this->map.get(-1), toInnerIdx(this->size() - 1).second()));
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::getBit(const integer bit, const integer block) const -> bool {
        return getBitFromBlock(this->map.get(block), bit);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::setBit(const integer bit, const integer block) -> void {
        this->map.set(block, setBitFromBlock(this->map.get(block), bit));
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::clearBit(const integer bit, const integer block) -> void {
        this->map.set(block, clearBitFromBlock(this->map.get(block), bit));
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::writeBit(const integer bit, const integer block, const bool value) -> void {
        value ? this->setBit(bit, block) : this->clearBit(bit, block);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::toInnerIdx(const integer index) -> couple<u_integer, integer> {
        return {static_cast<u_integer>(index / BLOCK_MAX_SIZE), index % BLOCK_MAX_SIZE};
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::toOuterIdx(const u_integer cur_block, const integer cur_bit) -> integer
    {
        return cur_block *  BLOCK_MAX_SIZE + cur_bit;
    }

    template<template <typename> typename ALLOC>
    original::bitSet<ALLOC>::Iterator::Iterator(const integer bit, const integer block, underlying_type* block_p, const bitSet* container)
        : cur_bit(bit), cur_block(block), block_(block_p), container_(container) {}

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::equalPtr(const iterator *other) const -> bool {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr
            && this->cur_bit == other_it->cur_bit
            && this->cur_block == other_it->cur_block;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::hasNext() const -> bool {
        return toOuterIdx(this->cur_block, this->cur_bit) < this->container_->size() - 1;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::hasPrev() const -> bool {
        return toOuterIdx(this->cur_block, this->cur_bit) > 0;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::atPrev(const iterator *other) const -> bool {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        if (other_it == nullptr)
            return false;
        return this->operator-(*other_it) == -1;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::atNext(const iterator *other) const -> bool {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        if (other_it == nullptr)
            return false;
        return this->operator-(*other_it) == 1;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::next() const -> void {
        this->operator+=(1);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::prev() const -> void {
        this->operator-=(1);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::getPrev() const -> Iterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto* it = this->clone();
        it->prev();
        return it;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::getNext() const -> Iterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto* it = this->clone();
        it->next();
        return it;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::operator+=(const integer steps) const -> void
    {
        auto new_idx = toInnerIdx(toOuterIdx(this->cur_block, this->cur_bit) + steps);
        this->cur_block = new_idx.first();
        this->cur_bit = new_idx.second();
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::operator-=(const integer steps) const -> void
    {
        auto new_idx = toInnerIdx(toOuterIdx(this->cur_block, this->cur_bit) - steps);
        this->cur_block = new_idx.first();
        this->cur_bit = new_idx.second();
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::operator-(const iterator &other) const -> integer {
        auto* other_it = dynamic_cast<const Iterator*>(&other);
        if (other_it == nullptr)
            return this > &other ?
                std::numeric_limits<integer>::max() :
                std::numeric_limits<integer>::min();
        if (this->container_ != other_it->container_)
            return this->container_ > other_it->container_ ?
                std::numeric_limits<integer>::max() :
                std::numeric_limits<integer>::min();

        return toOuterIdx(this->cur_block, this->cur_bit) - toOuterIdx(other_it->cur_block, other_it->cur_bit);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::get() -> bool & {
        throw unSupportedMethodError();
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::className() const -> std::string {
        return "bitSet::Iterator";
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::get() const -> bool
    {
        if (!this->isValid()) throw outOfBoundError();
        return getBitFromBlock(*this->block_, this->cur_bit);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::set(const bool &data) -> void {
        if (!this->isValid()) throw outOfBoundError();
        *this->block_ = data ?
         setBitFromBlock(*this->block_, this->cur_bit) : clearBitFromBlock(*this->block_, this->cur_bit);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::Iterator::isValid() const -> bool {
        const auto outer = toOuterIdx(this->cur_block, this->cur_bit);
        return outer >= 0 && outer < this->container_->size();
    }

    template<template <typename> typename ALLOC>
    original::bitSet<ALLOC>::bitSet(const u_integer size, const ALLOC<underlying_type>& allocator)
        : baseArray<bool, ALLOC<bool>>(ALLOC<bool>{}), size_(), allocator_underlying(allocator) {
            this->bitsetInit(size);
    }

    template<template <typename> typename ALLOC>
    original::bitSet<ALLOC>::bitSet(const std::initializer_list<bool>& lst) : bitSet(lst.size()) {
        u_integer i = 0;
        for (const auto& e : lst) {
            auto idx = toInnerIdx(i);
            this->writeBit(idx.second(), idx.first(), e);
            i += 1;
        }
    }

    template<template <typename> typename ALLOC>
    original::bitSet<ALLOC>::bitSet(const bitSet &other) : bitSet(other.size()) {
        this->operator=(other);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::operator=(const bitSet &other) -> bitSet& {
        if (this == &other) return *this;
        this->map = other.map;
        this->size_ = other.size_;
        if constexpr (ALLOC<bool>::propagate_on_container_copy_assignment::value){
            this->allocator = other.allocator;
        }
        return *this;
    }

    template<template <typename> typename ALLOC>
    original::bitSet<ALLOC>::bitSet(bitSet&& other)  noexcept : bitSet(0)
    {
        this->operator=(std::move(other));
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::operator=(bitSet&& other)  noexcept -> bitSet&
    {
        if (this == &other)
            return *this;

        this->map = std::move(other.map);
        this->size_ = other.size_;
        other.bitsetInit(0);
        if constexpr (ALLOC<bool>::propagate_on_container_move_assignment::value){
            this->allocator = other.allocator;
        }
        return *this;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::count() const -> u_integer {
        u_integer count = 0;
        for (const auto& e : this->map) {
            auto n = e;
            while (n) {
                n &= n - 1;
                count += 1;
            }
        }
        return count;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::resize(const u_integer new_size) const -> bitSet {
        if (this->size() == new_size) {
            return *this;
        }

        auto nb = bitSet(new_size);
        const u_integer blocks_min = min(nb.map.size(),
                                        this->map.size());
        for (u_integer i = 0; i < blocks_min; i++) {
            nb.map.set(i, this->map.get(i));
        }
        nb.clearRedundantBits();
        return nb;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::size() const -> u_integer
    {
        return this->size_;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::begins() const -> Iterator* {
        return new Iterator(0, 0, &this->map.data(), this);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::ends() const -> Iterator* {
        return new Iterator(toInnerIdx(this->size() - 1).second(), this->map.size() - 1,
                    &this->map.data() + this->map.size() - 1, this);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::get(integer index) const -> bool {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto idx = toInnerIdx(index);
        return this->getBit(idx.second(), idx.first());
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::operator[](integer) -> bool& {
        throw unSupportedMethodError();
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::set(integer index, const bool &e) -> void {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto idx = toInnerIdx(index);
        this->writeBit(idx.second(), idx.first(), e);
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::indexOf(const bool &e) const -> u_integer {
        for (u_integer i = 0; i < this->size(); i++) {
            if (auto idx = toInnerIdx(i);
                e == this->getBit(idx.second(), idx.first())) {
                return i;
            }
        }
        return this->size();
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::operator&=(const bitSet &other) -> bitSet& {
        if (this->size() != other.size()) {
            const auto resized = other.resize(this->size());
            for (u_integer i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) & resized.map.get(i));
            }
        }else {
            for (u_integer i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) & other.map.get(i));
            }
        }
        return *this;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::operator|=(const bitSet &other) -> bitSet& {
        if (this->size() != other.size()) {
            const auto resized = other.resize(this->size());
            for (u_integer i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) | resized.map.get(i));
            }
        }else {
            for (u_integer i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) | other.map.get(i));
            }
        }
        return *this;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::operator^=(const bitSet &other) -> bitSet& {
        if (this->size() != other.size()) {
            const auto resized = other.resize(this->size());
            for (u_integer i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) ^ resized.map.get(i));
            }
        }else {
            for (u_integer i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) ^ other.map.get(i));
            }
        }
        return *this;
    }

    template<template <typename> typename ALLOC>
    auto original::bitSet<ALLOC>::className() const -> std::string {
        return "bitSet";
    }

    template<template <typename> typename ALLOC_>
    auto original::operator&(const bitSet<ALLOC_> &lbs, const bitSet<ALLOC_> &rbs) -> bitSet<ALLOC_> {
        bitSet bs(lbs);
        return bs &= rbs;
    }

    template<template <typename> typename ALLOC_>
    auto original::operator|(const bitSet<ALLOC_> &lbs, const bitSet<ALLOC_> &rbs) -> bitSet<ALLOC_> {
        bitSet bs(lbs);
        return bs |= rbs;
    }

    template<template <typename> typename ALLOC_>
    auto original::operator^(const bitSet<ALLOC_> &lbs, const bitSet<ALLOC_> &rbs) -> bitSet<ALLOC_> {
        bitSet bs(lbs);
        return bs ^= rbs;
    }

    template<template <typename> typename ALLOC_>
    auto original::operator~(const bitSet<ALLOC_> &bs) -> bitSet<ALLOC_> {
        bitSet nbs(bs);
        for (u_integer i = 0; i < nbs.map.size(); i++) {
            nbs.map.set(i, ~nbs.map.get(i));
        }
        nbs.clearRedundantBits();
        return nbs;
    }

#endif //BITSET_H
