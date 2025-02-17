#ifndef VECTOR_H
#define VECTOR_H

/**
 * @file vector.h
 * @brief Dynamic array container with automatic resizing
 * @details Implements a vector container with efficient operations
 * and auto-centering memory management. Supports random access and iterator-based traversal.
 */

#include "baseList.h"
#include "iterationStream.h"
#include "array.h"

namespace original{
    /**
     * @class vector
     * @tparam TYPE Element type stored in the vector
     * @brief Dynamic array container with amortized constant time operations
     * @extends baseList
     * @extends iterationStream
     * @details Features include:
     * - Auto-resizing with centered memory allocation
     * - Random access via operator[] with bounds checking
     * - Bidirectional element insertion/removal
     * - STL-style iterator support
     * - Initializer list and array conversion
     */
    template <typename TYPE>
    class vector final : public baseList<TYPE>, public iterationStream<TYPE, vector<TYPE>>{
        uint32_t size_;                 ///< Current number of elements
        const uint32_t INNER_SIZE_INIT = 16; ///< Initial buffer capacity
        uint32_t max_size;              ///< Current buffer capacity
        uint32_t inner_begin;           ///< Starting index in circular buffer
        TYPE* body;                     ///< Internal storage buffer

        /**
         * @brief Initializes the vector with default settings.
         */
        void vectorInit();

        /**
         * @brief Destroys the internal storage of the vector.
         */
        void vectorDestruct() const;

        /**
         * @brief Allocates and initializes an array of a given size.
         * @param size The size of the array to initialize.
         * @return Pointer to the newly allocated array.
         */
        static TYPE* vectorArrayInit(uint32_t size);

        /**
         * @brief Moves elements from the old buffer to the new buffer.
         * @param old_body The original array to move elements from.
         * @param inner_idx The starting index for moving.
         * @param len The number of elements to move.
         * @param new_body The new buffer to move elements to.
         * @param offset The offset to apply when moving the elements.
         */
        static void moveElements(TYPE* old_body, uint32_t inner_idx,
                                 uint32_t len, TYPE* new_body, int64_t offset);

        /**
         * @brief Converts an index to an inner buffer index.
         * @param index The index to convert.
         * @return The corresponding index in the internal buffer.
         */
        [[nodiscard]] uint32_t toInnerIdx(int64_t index) const;

        /**
         * @brief Checks if a given increment would exceed the inner buffer size of the vector.
         * @param increment The increment to check.
         * @return True if the increment would exceed max size, false otherwise.
         */
        [[nodiscard]] bool outOfMaxSize(uint32_t increment) const;

        /**
         * @brief Grows the vector's internal buffer to accommodate more elements.
         * @param new_size The new size for the buffer.
         */
        void grow(uint32_t new_size);

        /**
         * @brief Adjusts the vector's internal buffer to accommodate an increment in size.
         * @param increment The number of elements to accommodate.
         */
        void adjust(uint32_t increment);

    public:
        /**
         * @class Iterator
         * @brief Random access iterator implementation for vector
         * @extends randomAccessIterator
         * @details Provides full random access capabilities including:
         * - Pointer arithmetic operations
         * - Element dereferencing
         * - Comparison operators
         */
        class Iterator final : public randomAccessIterator<TYPE>
        {
                /**
                 * @brief Constructs an iterator for the vector.
                 * @param ptr Pointer to the element in the vector that the iterator will point to.
                 * @param container Pointer to the container (vector) the iterator belongs to.
                 * @param pos The position of the iterator within the container.
                 */
                explicit Iterator(TYPE* ptr, const vector* container, int64_t pos);
            public:
                friend vector;

            /**
            * @brief Copy constructor for the iterator.
            * @param other The iterator to copy.
            */
            Iterator(const Iterator& other);

            /**
             * @brief Assignment operator for the iterator.
             * @param other The iterator to assign from.
             * @return A reference to this iterator.
             */
            Iterator& operator=(const Iterator& other);

            /**
             * @brief Clones the iterator.
             * @return A new iterator pointing to the same position.
             */
            Iterator* clone() const override;

            /**
             * @brief Checks if the iterator is at the previous element relative to another iterator.
             * @param other The other iterator to compare to.
             * @return True if this iterator is at the previous element, false otherwise.
             */
            bool atPrev(const iterator<TYPE> *other) const override;

            /**
             * @brief Checks if the iterator is at the next element relative to another iterator.
             * @param other The other iterator to compare to.
             * @return True if this iterator is at the next element, false otherwise.
             */
            bool atNext(const iterator<TYPE> *other) const override;

            /**
             * @brief Gets the class name of the iterator.
             * @return The class name as a string.
             */
            [[nodiscard]] std::string className() const override;
        };

        /**
          * @brief Default constructor for the vector.
          */
        explicit vector();

        /**
         * @brief Copy constructor for the vector.
         * @param other The vector to copy from.
         */
        vector(const vector& other);

        /**
         * @brief Constructs a vector from an initializer list.
         * @param list The initializer list to construct the vector from.
         */
        vector(const std::initializer_list<TYPE>& list);

        /**
         * @brief Constructs a vector from an array.
         * @param arr The array to construct the vector from.
         */
        explicit vector(const array<TYPE>& arr);

        /**
         * @brief Assignment operator for the vector.
         * @param other The vector to assign from.
         * @return A reference to this vector.
         */
        vector& operator=(const vector& other);

        /**
         * @brief Move constructor for the vector.
         * @param other The vector to move from.
         */
        vector(vector&& other) noexcept;

        /**
         * @brief Move assignment operator for the vector.
         * @param other The vector to move from.
         * @return A reference to this vector.
         */
        vector& operator=(vector&& other) noexcept;

        /**
         * @brief Gets the size of the vector.
         * @return The number of elements in the vector.
         */
        [[nodiscard]] uint32_t size() const override;

        /**
         * @brief Gets a reference to the first element in the vector.
         * @return A reference to the first element.
         */
        TYPE& data() const;

        /**
         * @brief Gets an element at the specified index.
         * @param index The index of the element.
         * @return The element at the specified index.
         */
        TYPE get(int64_t index) const override;

        /**
         * @brief Gets a reference to the element at the specified index.
         * @param index The index of the element.
         * @return A reference to the element at the specified index.
         */
        TYPE& operator[](int64_t index) override;

        /**
         * @brief Sets the element at the specified index.
         * @param index The index of the element to set.
         * @param e The value to set.
         */
        void set(int64_t index, const TYPE &e) override;

        /**
         * @brief Finds the index of the first occurrence of the specified element.
         * @param e The element to find.
         * @return The index of the element, or the size of the vector if not found.
         */
        uint32_t indexOf(const TYPE &e) const override;

        /**
         * @brief Inserts an element at the beginning of the vector.
         * @param e The element to insert.
         */
        void pushBegin(const TYPE &e) override;

        /**
         * @brief Inserts an element at the specified index in the vector.
         * @param index The index to insert the element at.
         * @param e The element to insert.
         */
        void push(int64_t index, const TYPE &e) override;

        /**
         * @brief Inserts an element at the end of the vector.
         * @param e The element to insert.
         */
        void pushEnd(const TYPE &e) override;

        /**
         * @brief Removes and returns the first element in the vector.
         * @return The removed element.
         */
        TYPE popBegin() override;

        /**
         * @brief Removes and returns the element at the specified index.
         * @param index The index of the element to remove.
         * @return The removed element.
         */
        TYPE pop(int64_t index) override;

        /**
         * @brief Removes and returns the last element in the vector.
         * @return The removed element.
         */
        TYPE popEnd() override;

        /**
         * @brief Gets an iterator to the beginning of the vector.
         * @return An iterator to the beginning.
         */
        Iterator* begins() const override;

        /**
         * @brief Gets an iterator to the end of the vector.
         * @return An iterator to the end.
         */
        Iterator* ends() const override;

        /**
         * @brief Gets the class name of the vector.
         * @return The class name as a string.
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Destructor for the vector.
         */
        ~vector() override;
    };
}

    template <typename TYPE>
    auto original::vector<TYPE>::vectorInit() -> void
    {
        this->size_ = 0;
        this->max_size = this->INNER_SIZE_INIT;
        this->inner_begin = (this->INNER_SIZE_INIT - 1)/2;
        this->body = vector::vectorArrayInit(this->INNER_SIZE_INIT);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorDestruct() const -> void
    {
        delete[] this->body;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorArrayInit(const uint32_t size) -> TYPE* {
        auto arr = new TYPE[size];
        for (uint32_t i = 0; i < size; i++) {
            arr[i] = TYPE{};
        }
        return arr;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::moveElements(TYPE* old_body, const uint32_t inner_idx,
                                              const uint32_t len, TYPE* new_body, const int64_t offset) -> void{
        if (offset > 0)
        {
            for (uint32_t i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + len - 1 - i] = old_body[inner_idx + len - 1 - i];
            }
        }else
        {
            for (uint32_t i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + i] = old_body[inner_idx + i];
            }
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::toInnerIdx(int64_t index) const -> uint32_t
    {
        return this->inner_begin + index;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::outOfMaxSize(uint32_t increment) const -> bool
    {
        return this->inner_begin + this->size() + increment > this->max_size - 1 || static_cast<int64_t>(this->inner_begin) - static_cast<int64_t>(increment) < 0;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::grow(const uint32_t new_size) -> void
    {
        TYPE* new_body = vector::vectorArrayInit(new_size);
        uint32_t new_begin = (new_size - 1) / 4;
        const int64_t offset = static_cast<int64_t>(new_begin) - static_cast<int64_t>(this->inner_begin);
        vector::moveElements(this->body, this->inner_begin,
                             this->size(), new_body, offset);
        delete[] this->body;
        this->body = new_body;
        this->max_size = new_size;
        this->inner_begin = new_begin;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::adjust(uint32_t increment) -> void {
        if (!this->outOfMaxSize(increment)) {
            return;
        }
        uint32_t new_begin = (this->max_size - this->size() - increment) / 2;
        if (this->max_size >= this->size_ + increment && new_begin > 0) {
            const int64_t offset = static_cast<int64_t>(new_begin) - static_cast<int64_t>(this->inner_begin);
            vector::moveElements(this->body, this->inner_begin, this->size(),
                                 this->body, offset);
            this->inner_begin = new_begin;
        } else {
            const uint32_t new_max_size = (this->size() + increment) * 2;
            this->grow(new_max_size);
        }
    }

    template <typename TYPE>
    original::vector<TYPE>::Iterator::Iterator(TYPE* ptr, const vector* container, int64_t pos)
        : randomAccessIterator<TYPE>(ptr, container, pos) {}

    template <typename TYPE>
    original::vector<TYPE>::Iterator::Iterator(const Iterator& other)
        : randomAccessIterator<TYPE>(nullptr, nullptr, 0)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::Iterator::operator=(const Iterator& other) -> Iterator&
    {
        if (this == &other) {
            return *this;
        }
        randomAccessIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return this->_ptr + 1 == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it->_ptr + 1 == this->_ptr;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::Iterator::className() const -> std::string {
        return "vector::Iterator";
    }

    template <typename TYPE>
    original::vector<TYPE>::vector() : size_(), max_size(), inner_begin()
    {
        this->vectorInit();
    }

    template<typename TYPE>
    original::vector<TYPE>::vector(const vector& other) : vector(){
        this->operator=(other);
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(const std::initializer_list<TYPE>& list) : vector()
    {
        this->adjust(list.size());
        for (const TYPE& e: list)
        {
            this->body[this->inner_begin + this->size()] = e;
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::vector<TYPE>::operator=(const vector& other) -> vector&
    {
        if (this == &other)
            return *this;

        this->vectorDestruct();

        this->max_size = other.max_size;
        this->inner_begin = other.inner_begin;
        this->size_ = other.size_;
        this->body = vector::vectorArrayInit(this->max_size);
        for (uint32_t i = 0; i < this->size(); ++i) {
            const TYPE& data = other.body[this->toInnerIdx(i)];
            this->body[this->toInnerIdx(i)] = data;
        }
        return *this;
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(vector&& other) noexcept : vector()
    {
        this->operator=(std::move(other));
    }

    template <typename TYPE>
    auto original::vector<TYPE>::operator=(vector&& other) noexcept -> vector&
    {
        if (this == &other)
            return *this;

        this->vectorDestruct();
        this->body = std::move(other.body);
        this->max_size = other.max_size;
        this->inner_begin = other.inner_begin;
        this->size_ = other.size_;
        other.vectorInit();
        return *this;
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(const array<TYPE>& arr) : vector()
    {
        this->adjust(arr.size());
        for (uint32_t i = 0; i < arr.size(); i += 1)
        {
            this->body[this->toInnerIdx(i)] = arr.get(i);
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::size() const -> uint32_t
    {
        return this->size_;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::data() const -> TYPE& {
        return this->body[this->toInnerIdx(0)];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::get(int64_t index) const -> TYPE
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::operator[](int64_t index) -> TYPE&
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index];
    }

    template <typename TYPE>
    auto original::vector<TYPE>::set(int64_t index, const TYPE &e) -> void
    {
        if (this->indexOutOfBound(index))
        {
            throw outOfBoundError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        this->body[index] = e;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::indexOf(const TYPE &e) const -> uint32_t
    {
        for (uint32_t i = 0; i < this->size(); i += 1)
        {
            if (this->get(i) == e)
            {
                return i;
            }
        }
        return this->size();
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushBegin(const TYPE &e) -> void
    {
        this->adjust(1);
        this->inner_begin -= 1;
        this->body[this->toInnerIdx(0)] = e;
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::push(int64_t index, const TYPE &e) -> void
    {
        if (this->parseNegIndex(index) == this->size())
        {
            this->pushEnd(e);
        }else if (this->parseNegIndex(index) == 0)
        {
            this->pushBegin(e);
        }else
        {
            if (this->indexOutOfBound(index))
            {
                throw outOfBoundError();
            }
            this->adjust(1);
            index = this->toInnerIdx(this->parseNegIndex(index));
            uint32_t rel_idx = index - this->inner_begin;
            if (index - this->inner_begin <= (this->size() - 1) / 2)
            {
                vector::moveElements(this->body, this->inner_begin,
                                     rel_idx + 1, this->body, -1);
                this->inner_begin -= 1;
            }else
            {
                vector::moveElements(this->body, index,
                                     this->size() - rel_idx, this->body, 1);
            }
            this->body[this->toInnerIdx(rel_idx)] = e;
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushEnd(const TYPE &e) -> void
    {
        this->adjust(1);
        this->body[this->toInnerIdx(this->size())] = e;
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::popBegin() -> TYPE
    {
        if (this->size() == 0){
            throw noElementError();
        }
        TYPE res = this->get(0);
        this->inner_begin += 1;
        this->size_ -= 1;
        return res;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pop(int64_t index) -> TYPE
    {
        if (this->parseNegIndex(index) == 0)
        {
            return this->popBegin();
        }
        if (this->parseNegIndex(index) == this->size() - 1)
        {
            return this->popEnd();
        }
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        TYPE res = this->get(index);
        index = this->toInnerIdx(this->parseNegIndex(index));
        uint32_t rel_idx = index - this->inner_begin;
        if (index - this->inner_begin <= (this->size() - 1) / 2)
        {
            vector::moveElements(this->body, this->inner_begin,
                                 rel_idx, this->body, 1);
            this->inner_begin += 1;
        }else
        {
            vector::moveElements(this->body, index + 1,
                                 this->size() - 1 - rel_idx, this->body, -1);
        }
        this->size_ -= 1;
        return res;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::popEnd() -> TYPE
    {
        if (this->size() == 0){
            throw noElementError();
        }
        TYPE res = this->get(this->size() - 1);
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::vector<TYPE>::begins() const -> Iterator* {
        return new Iterator(&this->body[this->toInnerIdx(0)], this, 0);
    }

    template<typename TYPE>
    auto original::vector<TYPE>::ends() const -> Iterator* {
        return new Iterator(&this->body[this->toInnerIdx(this->size() - 1)], this, this->size() - 1);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::className() const -> std::string
    {
        return "vector";
    }

    template <typename TYPE>
    original::vector<TYPE>::~vector() {
        this->vectorDestruct();
    }

#endif //VECTOR_H
