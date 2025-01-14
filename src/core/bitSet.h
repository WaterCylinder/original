#ifndef BITSET_H
#define BITSET_H
#include "array.h"
#include "couple.h"
#include "iterationStream.h"


namespace original {
    class bitSet final : public serial<bool>, public iterationStream<bool>{
            using underlying_type = uint64_t;

            static constexpr int64_t BLOCK_MAX_SIZE = sizeof(underlying_type) * 8;
            array<underlying_type> map;
            uint32_t size_;

            void bitsetInit(uint32_t size);
            [[nodiscard]] static bool getBitFromBlock(underlying_type block_value, int64_t bit);
            [[nodiscard]] static underlying_type setBitFromBlock(underlying_type block_value, int64_t bit);
            [[nodiscard]] static underlying_type clearBitFromBlock(underlying_type block_value, int64_t bit);
            [[nodiscard]] static underlying_type clearHigherBitsFromBlock(underlying_type block_value, int64_t bit);
            void clearRedundantBits();
            [[nodiscard]] bool getBit(int64_t bit, int64_t block) const;
            void setBit(int64_t bit, int64_t block);
            void clearBit(int64_t bit, int64_t block);
            void writeBit(int64_t bit, int64_t block, bool value);
            static couple<uint32_t, int64_t> toInnerIdx(int64_t index);
            static int64_t toOuterIdx(uint32_t cur_block, int64_t cur_bit);
        public:
            class Iterator final : public baseIterator<bool> {
                    mutable int64_t cur_bit;
                    mutable int64_t cur_block;
                    mutable underlying_type* block_;
                    const bitSet* container_;

                    explicit Iterator(int64_t bit, int64_t block, underlying_type* block_p, const bitSet* container);
                    bool equalPtr(const iterator *other) const override;
                public:
                    friend class bitSet;
                    Iterator* clone() const override;
                    [[nodiscard]] bool hasNext() const override;
                    [[nodiscard]] bool hasPrev() const override;
                    bool atPrev(const iterator *other) const override;
                    bool atNext(const iterator *other) const override;
                    void next() const override;
                    void prev() const override;
                    Iterator* getPrev() const override;
                    Iterator* getNext() const override;
                    void operator+=(int64_t steps) const override;
                    void operator-=(int64_t steps) const override;
                    int64_t operator-(const iterator& other) const override;
                    bool& get() override;
                    [[nodiscard]] std::string className() const override;
                    bool get() const override;
                    void set(const bool &data) override;
                    [[nodiscard]] bool isValid() const override;
            };

            explicit bitSet(uint32_t size);
            bitSet(const std::initializer_list<bool>& lst);
            bitSet(const bitSet& other);
            bitSet& operator=(const bitSet& other);
            bitSet(bitSet&& other) noexcept;
            bitSet& operator=(bitSet&& other) noexcept;
            bool operator==(const bitSet& other) const;
            [[nodiscard]] uint32_t count() const;
            [[nodiscard]] bitSet resize(uint32_t new_size) const;
            [[nodiscard]] uint32_t size() const override;
            [[nodiscard]] Iterator* begins() const override;
            [[nodiscard]] Iterator* ends() const override;
            [[nodiscard]] bool get(int64_t index) const override;
            bool& operator[](int64_t index) override;
            void set(int64_t index, const bool &e) override;
            [[nodiscard]] uint32_t indexOf(const bool &e) const override;
            bitSet& operator&=(const bitSet& other);
            bitSet& operator|=(const bitSet& other);
            bitSet& operator^=(const bitSet& other);
            [[nodiscard]] std::string className() const override;

            template<typename Callback = transform<bool>>
            void forEach(Callback operation = Callback{}) = delete;

            friend bitSet operator&(const bitSet& lbs, const bitSet& rbs);
            friend bitSet operator|(const bitSet& lbs, const bitSet& rbs);
            friend bitSet operator^(const bitSet& lbs, const bitSet& rbs);
            friend bitSet operator~(const bitSet& bs);
    };

    bitSet operator&(const bitSet& lbs, const bitSet& rbs);
    bitSet operator|(const bitSet& lbs, const bitSet& rbs);
    bitSet operator^(const bitSet& lbs, const bitSet& rbs);
    bitSet operator~(const bitSet& bs);
}

    inline auto original::bitSet::bitsetInit(const uint32_t size) -> void
    {
        this->map = array<underlying_type>((size + BLOCK_MAX_SIZE - 1) / BLOCK_MAX_SIZE);
        this->size_ = size;
    }

    inline auto original::bitSet::getBitFromBlock(const underlying_type block_value, const int64_t bit) -> bool {
        return block_value & static_cast<underlying_type>(1) << bit;
    }

    inline auto original::bitSet::setBitFromBlock(const underlying_type block_value, const int64_t bit) -> underlying_type {
        return block_value | static_cast<underlying_type>(1) << bit;
    }

    inline auto original::bitSet::clearBitFromBlock(const underlying_type block_value, const int64_t bit) -> underlying_type {
        return block_value & ~(static_cast<underlying_type>(1) << bit);
    }

    inline auto original::bitSet::clearHigherBitsFromBlock(const underlying_type block_value, const int64_t bit) -> underlying_type
    {
        return block_value & (static_cast<underlying_type>(1) << bit + 1) - static_cast<underlying_type>(1);
    }

    inline auto original::bitSet::clearRedundantBits() -> void
    {
        this->map.set(-1, clearHigherBitsFromBlock(this->map.get(-1), toInnerIdx(this->size() - 1).second()));
    }

    inline auto original::bitSet::getBit(const int64_t bit, const int64_t block) const -> bool {
        return getBitFromBlock(this->map.get(block), bit);
    }

    inline auto original::bitSet::setBit(const int64_t bit, const int64_t block) -> void {
        this->map.set(block, setBitFromBlock(this->map.get(block), bit));
    }

    inline auto original::bitSet::clearBit(const int64_t bit, const int64_t block) -> void {
        this->map.set(block, clearBitFromBlock(this->map.get(block), bit));
    }

    inline void original::bitSet::writeBit(const int64_t bit, const int64_t block, const bool value) {
        value ? this->setBit(bit, block) : this->clearBit(bit, block);
    }

    inline auto original::bitSet::toInnerIdx(const int64_t index) -> couple<uint32_t, int64_t> {
        return {static_cast<uint32_t>(index / BLOCK_MAX_SIZE), index % BLOCK_MAX_SIZE};
    }

    inline auto original::bitSet::toOuterIdx(const uint32_t cur_block, const int64_t cur_bit) -> int64_t
    {
        return cur_block *  BLOCK_MAX_SIZE + cur_bit;
    }

    inline original::bitSet::Iterator::Iterator(const int64_t bit, const int64_t block, underlying_type* block_p, const bitSet* container)
        : cur_bit(bit), cur_block(block), block_(block_p), container_(container) {}

    inline auto original::bitSet::Iterator::equalPtr(const iterator *other) const -> bool {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr
            && this->cur_bit == other_it->cur_bit
            && this->cur_block == other_it->cur_block;
    }

    inline auto original::bitSet::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    inline auto original::bitSet::Iterator::hasNext() const -> bool {
        return toOuterIdx(this->cur_block, this->cur_bit) < this->container_->size() - 1;
    }

    inline auto original::bitSet::Iterator::hasPrev() const -> bool {
        return toOuterIdx(this->cur_block, this->cur_bit) > 0;
    }

    inline auto original::bitSet::Iterator::atPrev(const iterator *other) const -> bool {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        if (other_it == nullptr)
            return false;
        return this->operator-(*other_it) == -1;
    }

    inline auto original::bitSet::Iterator::atNext(const iterator *other) const -> bool {
        auto* other_it = dynamic_cast<const Iterator*>(other);
        if (other_it == nullptr)
            return false;
        return this->operator-(*other_it) == 1;
    }

    inline auto original::bitSet::Iterator::next() const -> void {
        this->operator+=(1);
    }

    inline auto original::bitSet::Iterator::prev() const -> void {
        this->operator-=(1);
    }

    inline auto original::bitSet::Iterator::getPrev() const -> Iterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto* it = this->clone();
        it->prev();
        return it;
    }

    inline auto original::bitSet::Iterator::getNext() const -> Iterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto* it = this->clone();
        it->next();
        return it;
    }

    inline auto original::bitSet::Iterator::operator+=(const int64_t steps) const -> void
    {
        if (!this->isValid()) throw outOfBoundError();
        auto new_idx = toInnerIdx(toOuterIdx(this->cur_block, this->cur_bit) + steps);
        this->cur_block = new_idx.first();
        this->cur_bit = new_idx.second();
    }

    inline auto original::bitSet::Iterator::operator-=(const int64_t steps) const -> void
    {
        if (!this->isValid()) throw outOfBoundError();
        auto new_idx = toInnerIdx(toOuterIdx(this->cur_block, this->cur_bit) - steps);
        this->cur_block = new_idx.first();
        this->cur_bit = new_idx.second();
    }

    inline int64_t original::bitSet::Iterator::operator-(const iterator& other) const
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

        return toOuterIdx(this->cur_block, this->cur_bit) - toOuterIdx(other_it->cur_block, other_it->cur_bit);
    }

    inline auto original::bitSet::Iterator::get() -> bool & {
        throw unSupportedMethodError();
    }

    inline auto original::bitSet::Iterator::className() const -> std::string {
        return "bitSet::Iterator";
    }

    inline auto original::bitSet::Iterator::get() const -> bool
    {
        if (!this->isValid()) throw outOfBoundError();
        return getBitFromBlock(*this->block_, this->cur_bit);
    }

    inline void original::bitSet::Iterator::set(const bool &data) {
        if (!this->isValid()) throw outOfBoundError();
        *this->block_ = data ?
         setBitFromBlock(*this->block_, this->cur_bit) : clearBitFromBlock(*this->block_, this->cur_bit);
    }

    inline auto original::bitSet::Iterator::isValid() const -> bool {
        const auto outer = toOuterIdx(this->cur_block, this->cur_bit);
        return outer >= 0 && outer < this->container_->size();
    }

    inline original::bitSet::bitSet(const uint32_t size)
        : size_()
    {
        this->bitsetInit(size);
    }

    inline original::bitSet::bitSet(const std::initializer_list<bool>& lst) : bitSet(lst.size()) {
        uint32_t i = 0;
        for (const auto& e : lst) {
            auto idx = toInnerIdx(i);
            this->writeBit(idx.second(), idx.first(), e);
            i += 1;
        }
    }

    inline original::bitSet::bitSet(const bitSet &other) : bitSet(other.size()) {
        this->operator=(other);
    }

    inline auto original::bitSet::operator=(const bitSet &other) -> bitSet& {
        if (this == &other) return *this;
        this->map = other.map;
        this->size_ = other.size_;
        return *this;
    }

    inline original::bitSet::bitSet(bitSet&& other)  noexcept : bitSet(0)
    {
        this->operator=(std::move(other));
    }

    inline auto original::bitSet::operator=(bitSet&& other)  noexcept -> bitSet&
    {
        if (this == &other)
            return *this;

        this->map = std::move(other.map);
        this->size_ = other.size_;
        other.bitsetInit(0);
        return *this;
    }

    inline auto original::bitSet::operator==(const bitSet &other) const -> bool {
        if (this == &other) return true;
        return this->map == other.map && this->size_ == other.size_;
    }

    inline auto original::bitSet::count() const -> uint32_t {
        uint32_t count = 0;
        for (const auto& e : this->map) {
            auto n = e;
            while (n) {
                n &= n - 1;
                count += 1;
            }
        }
        return count;
    }

    inline auto original::bitSet::resize(const uint32_t new_size) const -> bitSet {
        if (this->size() == new_size) {
            return *this;
        }

        auto nb = bitSet(new_size);
        const uint32_t blocks_min = min(nb.map.size(),
                                        this->map.size());
        for (uint32_t i = 0; i < blocks_min; i++) {
            nb.map.set(i, this->map.get(i));
        }
        nb.clearRedundantBits();
        return nb;
    }

    inline auto original::bitSet::size() const -> uint32_t {
        return this->size_;
    }

    inline auto original::bitSet::begins() const -> Iterator* {
        return new Iterator(0, 0, &this->map.data(), this);
    }

    inline auto original::bitSet::ends() const -> Iterator* {
        return new Iterator(toInnerIdx(this->size() - 1).second(), this->map.size() - 1,
                    &this->map.data() + this->map.size() - 1, this);
    }

    inline auto original::bitSet::get(int64_t index) const -> bool {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto idx = toInnerIdx(index);
        return this->getBit(idx.second(), idx.first());
    }

    inline auto original::bitSet::operator[](int64_t index) -> bool& {
        throw unSupportedMethodError();
    }

    inline auto original::bitSet::set(int64_t index, const bool &e) -> void {
        if (this->indexOutOfBound(index)) throw outOfBoundError();
        index = this->parseNegIndex(index);
        auto idx = toInnerIdx(index);
        this->writeBit(idx.second(), idx.first(), e);
    }

    inline auto original::bitSet::indexOf(const bool &e) const -> uint32_t {
        for (uint32_t i = 0; i < this->size(); i++) {
            if (auto idx = toInnerIdx(i);
                e == this->getBit(idx.second(), idx.first())) {
                return i;
            }
        }
        return this->size();
    }

    inline auto original::bitSet::operator&=(const bitSet &other) -> bitSet& {
        if (this->size() != other.size()) {
            const auto resized = other.resize(this->size());
            for (uint32_t i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) & resized.map.get(i));
            }
        }else {
            for (uint32_t i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) & other.map.get(i));
            }
        }
        return *this;
    }

    inline auto original::bitSet::operator|=(const bitSet &other) -> bitSet& {
        if (this->size() != other.size()) {
            const auto resized = other.resize(this->size());
            for (uint32_t i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) | resized.map.get(i));
            }
        }else {
            for (uint32_t i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) | other.map.get(i));
            }
        }
        return *this;
    }

    inline auto original::bitSet::operator^=(const bitSet &other) -> bitSet& {
        if (this->size() != other.size()) {
            const auto resized = other.resize(this->size());
            for (uint32_t i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) ^ resized.map.get(i));
            }
        }else {
            for (uint32_t i = 0; i < this->map.size(); i++) {
                this->map.set(i, this->map.get(i) ^ other.map.get(i));
            }
        }
        return *this;
    }

    inline auto original::bitSet::className() const -> std::string {
        return "bitSet";
    }

    inline auto original::operator&(const bitSet &lbs, const bitSet &rbs) -> bitSet {
        bitSet bs(lbs);
        return bs &= rbs;
    }

    inline auto original::operator|(const bitSet &lbs, const bitSet &rbs) -> bitSet {
        bitSet bs(lbs);
        return bs |= rbs;
    }

    inline auto original::operator^(const bitSet &lbs, const bitSet &rbs) -> bitSet {
        bitSet bs(lbs);
        return bs ^= rbs;
    }

    inline auto original::operator~(const bitSet &bs) -> bitSet {
        bitSet nbs(bs);
        for (uint32_t i = 0; i < nbs.map.size(); i++) {
            nbs.map.set(i, ~nbs.map.get(i));
        }
        nbs.clearRedundantBits();
        return nbs;
    }

#endif //BITSET_H
