#ifndef FORWARDCHAIN_H
#define FORWARDCHAIN_H

#include "singleDirectionIterator.h"
#include "array.h"
#include "baseList.h"

/**
 * @file forwardChain.h
 * @brief Non-cyclic singly linked list implementation
 * @details This file provides an implementation of a singly linked list with efficient front and back operations.
 * The class includes:
 * - Single-direction iterator support
 * - Basic list operations such as push, pop, get, and indexOf
 * - Memory management with custom node handling
 */


namespace original {

    /**
     * @class forwardChain
     * @tparam TYPE The type of elements stored in the forward chain
     * @brief A singly linked list implementation.
     * @extends baseList
     * @extends iterationStream
     * @details The forwardChain class implements a singly linked list where elements are stored in nodes.
     *          Each node points to the next node, and the list supports operations like push, pop, get, and indexOf.
     */
    template <typename TYPE>
    class forwardChain final : public baseList<TYPE>, public iterationStream<TYPE, forwardChain<TYPE>>{

        /**
         * @class forwardChainNode
         * @brief Internal node structure for elements in forwardChain.
         * @extends wrapper
         * @details Represents a node in the linked list containing the element data and a pointer to the next node.
         */
        class forwardChainNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class forwardChain;
            private:
                TYPE data_;         ///< The data of the node
                forwardChainNode* next; ///< Pointer to the next node in the chain
            protected:

                /**
                 * @brief Constructs a forwardChainNode with given data and next pointer.
                 * @param data The data to store in the node.
                 * @param next Pointer to the next node (default is nullptr).
                 */
                explicit forwardChainNode(const TYPE& data = TYPE{}, forwardChainNode* next = nullptr);

                /**
                 * @brief Copy constructor for forwardChainNode.
                 * @param other The node to copy from.
                 */
                forwardChainNode(const forwardChainNode& other);

                /**
                 * @brief Assignment operator for forwardChainNode.
                 * @param other The node to assign from.
                 * @return A reference to this node.
                 */
                forwardChainNode& operator=(const forwardChainNode& other);

                /**
                 * @brief Gets the value stored in the node.
                 * @return A reference to the value stored in the node.
                 */
                TYPE& getVal() override;

                /**
                 * @brief Gets the value stored in the node (const version).
                 * @return A const reference to the value stored in the node.
                 */
                const TYPE& getVal() const override;

                /**
                 * @brief Sets the value of the node.
                 * @param data The value to set the node to.
                 */
                void setVal(TYPE data) override;

                /**
                 * @brief Gets the pointer to the previous node (not supported in this class).
                 * @return Throws an exception.
                 */
                forwardChainNode* getPPrev() const override;

                /**
                 * @brief Gets the pointer to the next node.
                 * @return A pointer to the next node.
                 */
                forwardChainNode* getPNext() const override;

                /**
                 * @brief Sets the pointer to the next node.
                 * @param new_next The new next node pointer.
                 */
                void setPNext(forwardChainNode* new_next);

                /**
                 * @brief Connects two nodes by adjusting their pointers.
                 * @param prev The previous node.
                 * @param next The next node.
                 */
                static void connect(forwardChainNode* prev, forwardChainNode* next);
        };

        uint32_t size_;         ///< The number of elements in the chain
        forwardChainNode* begin_; ///< Pointer to the first node in the chain

        /**
         * @brief Gets the first node (after the sentinel).
         * @return A pointer to the first node in the chain.
         */
        forwardChainNode* beginNode() const;

        /**
         * @brief Finds the node at the specified index.
         * @param index The index of the node to find.
         * @return A pointer to the node at the specified index.
         */
        forwardChainNode* findNode(int64_t index) const;

        /**
         * @brief Initializes the chain with a sentinel node.
         */
        void chainInit();

        /**
         * @brief Adds a node to the beginning of the chain.
         * @param node The node to add to the beginning.
         */
        void firstAdd(forwardChainNode* node);

        /**
         * @brief Deletes the last node of the chain.
         * @return The last node that was deleted.
         */
        forwardChainNode* lastDelete();

        /**
         * @brief Destroys the chain by deleting all nodes.
         */
        void chainDestruction();
    public:

        /**
         * @class Iterator
         * @brief Iterator for forwardChain, supports single-direction traversal
         * @extends singleDirectionIterator
         * @details Allows forward iteration through the forwardChain with operations like cloning, comparison, etc.
         */
        class Iterator final : public singleDirectionIterator<TYPE>
        {
            /**
             * @brief Constructs an Iterator from a given forwardChainNode pointer.
             * @param ptr Pointer to the forwardChainNode the iterator will point to.
             */
            explicit Iterator(forwardChainNode* ptr);
        public:
            friend forwardChain;

            /**
            * @brief Copy constructor for Iterator.
            * @param other The iterator to copy from.
            */
            Iterator(const Iterator& other);

            /**
             * @brief Assignment operator for Iterator.
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
             * @brief Checks if this iterator is at the previous element relative to another iterator.
             * @param other The iterator to compare to.
             * @return True if this iterator is at the previous element, false otherwise.
             */
            bool atPrev(const iterator<TYPE> *other) const override;

            /**
             * @brief Checks if this iterator is at the next element relative to another iterator.
             * @param other The iterator to compare to.
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
         * @brief Default constructor for forwardChain.
         */
        explicit forwardChain();

        /**
         * @brief Copy constructor for forwardChain.
         * @param other The forwardChain to copy from.
         */
        forwardChain(const forwardChain& other);

        /**
         * @brief Constructs a forwardChain from an initializer list.
         * @param list The initializer list to construct the forwardChain from.
         */
        forwardChain(std::initializer_list<TYPE> list);

        /**
         * @brief Constructs a forwardChain from an array.
         * @param arr The array to construct the forwardChain from.
         */
        explicit forwardChain(const array<TYPE>& arr);

        /**
         * @brief Assignment operator for forwardChain.
         * @param other The forwardChain to assign from.
         * @return A reference to this forwardChain.
         */
        forwardChain& operator=(const forwardChain& other);

        /**
         * @brief Move constructor for forwardChain.
         * @param other The forwardChain to move from.
         */
        forwardChain(forwardChain&& other) noexcept;

        /**
         * @brief Move assignment operator for forwardChain.
         * @param other The forwardChain to move from.
         * @return A reference to this forwardChain.
         */
        forwardChain& operator=(forwardChain&& other) noexcept;

        /**
         * @brief Gets the size of the forwardChain.
         * @return The number of elements in the forwardChain.
         */
        [[nodiscard]] uint32_t size() const override;

        /**
         * @brief Gets the element at the specified index.
         * @param index The index of the element to retrieve.
         * @return The element at the specified index.
         */
        TYPE get(int64_t index) const override;

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
        void set(int64_t index, const TYPE &e) override;

        /**
         * @brief Finds the index of the first occurrence of the specified element.
         * @param e The element to search for.
         * @return The index of the element, or the size of the forwardChain if not found.
         */
        uint32_t indexOf(const TYPE &e) const override;

        /**
         * @brief Pushes an element to the beginning of the forwardChain.
         * @param e The element to push to the beginning.
         */
        void pushBegin(const TYPE &e) override;

        /**
         * @brief Pushes an element at the specified index in the forwardChain.
         * @param index The index at which to insert the element.
         * @param e The element to push.
         */
        void push(int64_t index, const TYPE &e) override;

        /**
         * @brief Pushes an element to the end of the forwardChain.
         * @param e The element to push to the end.
         */
        void pushEnd(const TYPE &e) override;

        /**
         * @brief Pops an element from the beginning of the forwardChain.
         * @return The element that was popped.
         */
        TYPE popBegin() override;

        /**
         * @brief Pops an element at the specified index in the forwardChain.
         * @param index The index of the element to pop.
         * @return The element that was popped.
         */
        TYPE pop(int64_t index) override;

        /**
         * @brief Pops an element from the end of the forwardChain.
         * @return The element that was popped.
         */
        TYPE popEnd() override;

        /**
         * @brief Gets an iterator to the beginning of the forwardChain.
         * @return An iterator to the beginning of the forwardChain.
         */
        Iterator* begins() const override;

        /**
         * @brief Gets an iterator to the end of the forwardChain.
         * @return An iterator to the end of the forwardChain.
         */
        Iterator* ends() const override;

        /**
         * @brief Gets the class name of the forwardChain.
         * @return The class name as a string.
         */
        [[nodiscard]] std::string className() const override;

        /**
         * @brief Destructor for the forwardChain.
         */
        ~forwardChain() override;
    };
}

    template <typename TYPE>
    original::forwardChain<TYPE>::forwardChainNode::forwardChainNode(const TYPE& data, forwardChainNode* next)
        : data_(data), next(next) {}

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChainNode::forwardChainNode(const forwardChainNode &other)
        : data_(other.data_), next(other.next) {}

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::operator=(
        const forwardChainNode &other) -> forwardChainNode & {
        if (this != &other) {
            data_ = other.data_;
            next = other.next;
        }
        return *this;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getVal() -> TYPE& {
        return this->data_;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getVal() const -> const TYPE& {
        return this->data_;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::setVal(TYPE data) -> void {
        this->data_ = data;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getPPrev() const -> forwardChainNode* {
        throw unSupportedMethodError();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::getPNext() const -> forwardChainNode* {
        return this->next;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::setPNext(forwardChainNode *new_next) -> void {
        this->next = new_next;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::forwardChainNode::connect(
        forwardChainNode *prev, forwardChainNode *next) -> void {
        if (prev != nullptr) prev->setPNext(next);
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::beginNode() const -> forwardChainNode*
    {
        return this->begin_->getPNext();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::findNode(const int64_t index) const -> forwardChainNode* {
        if (this->size() == 0) return this->begin_;
        auto* cur = this->beginNode();
        for(uint32_t i = 0; i < index; i++)
        {
            cur = cur->getPNext();
        }
        return cur;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::chainInit() -> void
    {
        auto* pivot = new forwardChainNode();
        this->size_ = 0;
        this->begin_ = pivot;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::firstAdd(forwardChainNode* node) -> void
    {
        forwardChainNode::connect(this->findNode(0), node);
        this->size_ += 1;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::lastDelete() -> forwardChainNode*
    {
        auto* last = this->beginNode();
        delete this->begin_;
        this->chainInit();
        return last;
    }

    template <typename TYPE>
    auto original::forwardChain<TYPE>::chainDestruction() -> void
    {
        auto* cur = this->begin_;
        while (cur)
        {
            auto* next = cur->getPNext();
            delete cur;
            cur = next;
        }
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::Iterator::Iterator(forwardChainNode *ptr)
        : singleDirectionIterator<TYPE>(ptr) {}

    template<typename TYPE>
    original::forwardChain<TYPE>::Iterator::Iterator(const Iterator &other)
        : singleDirectionIterator<TYPE>(nullptr) {
        this->operator=(other);
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::operator=(const Iterator &other) -> Iterator & {
        if (this == &other) return *this;
        singleDirectionIterator<TYPE>::operator=(other);
        return *this;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::clone() const -> Iterator* {
        return new Iterator(*this);
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::atPrev(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr && this->_ptr->getPNext() == other_it->_ptr;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::atNext(const iterator<TYPE> *other) const -> bool {
        auto other_it = dynamic_cast<const Iterator*>(other);
        return other_it != nullptr && other_it->_ptr->getPNext() == this->_ptr;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::Iterator::className() const -> std::string {
        return "forwardChain::Iterator";
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain() : size_(0)
    {
        this->chainInit();
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(const forwardChain &other) : forwardChain() {
        this->operator=(other);
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(std::initializer_list<TYPE> list) : forwardChain() {
        for (auto e: list) {
            auto* cur_node = new forwardChainNode(e);
            if (this->size() == 0)
            {
                this->firstAdd(cur_node);
            }else
            {
                auto* end = this->findNode(this->size() - 1);
                forwardChainNode::connect(end, cur_node);
                this->size_ += 1;
            }
        }
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(const array<TYPE>& arr) : forwardChain() {
        for (uint32_t i = 0; i < arr.size(); i++) {
            auto* cur_node = new forwardChainNode(arr.get(i));
            if (this->size() == 0)
            {
                this->firstAdd(cur_node);
            }else
            {
                auto* end = this->findNode(this->size() - 1);
                forwardChainNode::connect(end, cur_node);
                this->size_ += 1;
            }
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::operator=(const forwardChain &other) -> forwardChain& {
        if (this == &other) return *this;
        this->chainDestruction();
        this->size_ = other.size_;
        if (this->size() != 0){
            auto* other_ = other.begin_;
            this->begin_ = new forwardChainNode(other_->getVal());
            auto* this_ = this->begin_;
            while (other_->getPNext() != nullptr){
                other_ = other_->getPNext();
                forwardChainNode::connect(this_, new forwardChainNode(other_->getVal()));
                this_ = this_->getPNext();
            }
        } else{
            this->chainInit();
        }
        return *this;
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::forwardChain(forwardChain &&other) noexcept : forwardChain() {
        this->operator=(std::move(other));
    }

    template<typename TYPE>
    original::forwardChain<TYPE> & original::forwardChain<TYPE>::operator=(forwardChain &&other) noexcept {
        if (this == &other)
            return *this;

        this->chainDestruction();
        this->begin_ = other.begin_;
        this->size_ = other.size_;
        other.chainInit();
        return *this;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::size() const -> uint32_t {
        return this->size_;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::get(int64_t index) const -> TYPE {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        return cur->getVal();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::operator[](int64_t index) -> TYPE& {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        return cur->getVal();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::set(int64_t index, const TYPE &e) -> void {
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        auto* cur = this->findNode(this->parseNegIndex(index));
        cur->setVal(e);
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::indexOf(const TYPE &e) const -> uint32_t {
        uint32_t i = 0;
        for (auto* current = this->begin_; current != nullptr; current = current->getPNext()) {
            if (current->getVal() == e) {
                return i;
            }
            i += 1;
        }
        return this->size();
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::pushBegin(const TYPE &e) -> void {
        auto* new_node = new forwardChainNode(e);
        if (this->size() == 0){
            this->firstAdd(new_node);
        } else{
            auto* next = this->beginNode();
            forwardChainNode::connect(this->begin_, new_node);
            forwardChainNode::connect(new_node, next);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::push(int64_t index, const TYPE &e) -> void {
        index = this->parseNegIndex(index);
        if (index == 0){
            this->pushBegin(e);
        } else if (index == this->size()){
            this->pushEnd(e);
        } else{
            if (this->indexOutOfBound(index)){
                throw outOfBoundError();
            }
            auto* new_node = new forwardChainNode(e);
            auto* prev = this->findNode(index - 1);
            auto* cur = prev->getPNext();
            forwardChainNode::connect(prev, new_node);
            forwardChainNode::connect(new_node, cur);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::pushEnd(const TYPE &e) -> void {
        auto* new_node = new forwardChainNode(e);
        if (this->size() == 0){
            this->firstAdd(new_node);
        } else{
            auto* end = this->findNode(this->size() - 1);
            forwardChainNode::connect(end, new_node);
            this->size_ += 1;
        }
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::popBegin() -> TYPE {
        TYPE res;
        if (this->size() == 0){
            throw noElementError();
        }

        res = this->beginNode()->getVal();
        if (this->size() == 1){
            delete this->lastDelete();
        } else{
            auto* del = this->beginNode();
            auto* new_begin = del->getPNext();
            delete del;
            forwardChainNode::connect(this->begin_, new_begin);
            this->size_ -= 1;
        }
        return res;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::pop(int64_t index) -> TYPE {
        index = this->parseNegIndex(index);
        if (index == 0){
            return this->popBegin();
        }
        if (index == this->size() - 1){
            return this->popEnd();
        }
        if (this->indexOutOfBound(index)){
            throw outOfBoundError();
        }
        TYPE res;
        auto* prev = this->findNode(index - 1);
        auto* cur = prev->getPNext();
        res = cur->getVal();
        auto* next = cur->getPNext();
        forwardChainNode::connect(prev, next);
        delete cur;
        this->size_ -= 1;
        return res;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::popEnd() -> TYPE {
        TYPE res;
        if (this->size() == 0){
            throw noElementError();
        }
        if (this->size() == 1){
            res = this->beginNode()->getVal();
            delete this->lastDelete();
        } else{
            auto* new_end = this->findNode(this->size() - 2);
            auto* end = new_end->getPNext();
            res = end->getVal();
            delete end;
            forwardChainNode::connect(new_end, nullptr);
            this->size_ -= 1;
        }
        return res;
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::begins() const -> Iterator* {
        return new Iterator(this->beginNode());
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::ends() const -> Iterator* {
        return new Iterator(this->findNode(this->size() - 1));
    }

    template<typename TYPE>
    auto original::forwardChain<TYPE>::className() const -> std::string {
        return "forwardChain";
    }

    template<typename TYPE>
    original::forwardChain<TYPE>::~forwardChain() {
        this->chainDestruction();
    }

#endif //FORWARDCHAIN_H
