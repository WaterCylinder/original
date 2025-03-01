#ifndef ITERABLE_H
#define ITERABLE_H

#include "transform.h"
#include "types.h"
#include "iterator.h"

/**
 * @file iterable.h
 * @brief Base interface for iterable container types
 * @details Defines the core abstraction for iterator-enabled containers. Provides:
 * - iterable<TYPE> base class with iterator access methods
 * - iterAdaptor nested class implementing iterator pattern
 * - Range-based for loop support via begin()/end() methods
 * - Element traversal and manipulation operations
 *
 * @see iterator.h For base iterator implementation
 * @see transform.h For operation callback templates
 */

namespace original {

    /**
     * @class iterable
     * @brief A base class for iterable containers that support iterators.
     * @tparam TYPE The type of elements contained in the iterable.
     * @details This class defines the `iterable` container interface, which provides methods for obtaining iterators
     *          and applying operations on elements using iterators. It also supports iteration using C++ range-based for loops
     *          by providing `begin()` and `end()` methods.
     */
    template <typename TYPE>
    class iterable {
    public:

        /**
         * @class iterAdaptor
         * @brief An iterator adapter for the iterable container.
         * @details The `iterAdaptor` class is an implementation of the iterator interface that adapts
         *          the base iterator (`baseIterator<TYPE>`) and provides additional iterator functionalities
         *          to the `iterable` class.
         */
        class iterAdaptor final : public iterator<TYPE> {
            baseIterator<TYPE>* it_;  ///< Pointer to the base iterator

            /**
             * @brief Constructs an `iterAdaptor` from a base iterator.
             * @param it A pointer to the base iterator to be adapted.
             */
            explicit iterAdaptor(baseIterator<TYPE>* it);

        protected:
            /**
             * @brief Compares the current iterator with another iterator.
             * @param other The iterator to compare with.
             * @return `true` if the iterators are pointing to the same element.
             */
            bool equalPtr(const iterator<TYPE>* other) const override;

            /**
             * @brief Creates a copy of the current iterator.
             * @return A new `iterAdaptor` pointing to the same element as the current iterator.
             */
            iterAdaptor* clone() const override;

            /**
             * @brief Gets the previous iterator.
             * @return A new `iterAdaptor` pointing to the previous element.
             */
            iterAdaptor* getPrev() const override;

            /**
             * @brief Gets the next iterator.
             * @return A new `iterAdaptor` pointing to the next element.
             */
            iterAdaptor* getNext() const override;

        public:
            friend class iterable;

            /**
             * @brief Copy constructor for the `iterAdaptor`.
             * @param other The `iterAdaptor` to copy.
             */
            iterAdaptor(const iterAdaptor& other);

            /**
             * @brief Copy assignment operator for the `iterAdaptor`.
             * @param other The `iterAdaptor` to copy.
             * @return A reference to the current `iterAdaptor`.
             */
            iterAdaptor& operator=(const iterAdaptor& other);

            /**
             * @brief Gets the underlying iterator.
             * @return A reference to the underlying base iterator.
             */
            const iterator<TYPE>& getIt() const;

            /**
             * @brief Checks if there is a next element.
             * @return `true` if there is a next element.
             */
            [[nodiscard]] bool hasNext() const override;

            /**
             * @brief Checks if there is a previous element.
             * @return `true` if there is a previous element.
             */
            [[nodiscard]] bool hasPrev() const override;

            /**
             * @brief Checks if the current iterator is at the previous element relative to another iterator.
             * @param other The iterator to compare with.
             * @return `true` if this iterator is positioned just before the other iterator.
             */
            bool atPrev(const iterator<TYPE>* other) const override;

            /**
             * @brief Checks if the current iterator is at the next element relative to another iterator.
             * @param other The iterator to compare with.
             * @return `true` if this iterator is positioned just after the other iterator.
             */
            bool atNext(const iterator<TYPE>* other) const override;

            /**
             * @brief Moves the iterator to the next element.
             */
            void next() const override;

            /**
             * @brief Moves the iterator to the previous element.
             */
            void prev() const override;

            /**
             * @brief Advances the iterator by a specified number of steps.
             * @param steps The number of steps to move forward.
             */
            void operator+=(integer steps) const override;

            /**
             * @brief Moves the iterator backward by a specified number of steps.
             * @param steps The number of steps to move backward.
             */
            void operator-=(integer steps) const override;

            /**
             * @brief Calculates the distance between this iterator and another iterator.
             * @param other The iterator to compare with.
             * @return The number of steps between the two iterators.
             */
            integer operator-(const iterator<TYPE>& other) const override;

            /**
             * @brief Gets the value of the element the iterator is pointing to.
             * @return A reference to the element.
             */
            TYPE& get() override;

            /**
             * @brief Sets the value of the element the iterator is pointing to.
             * @param data The value to set.
             */
            void set(const TYPE& data) override;

            /**
             * @brief Gets the value of the element the iterator is pointing to (const version).
             * @return The element value.
             */
            TYPE get() const override;

            /**
             * @brief Checks if the iterator is pointing to a valid element.
             * @return `true` if the iterator is valid, `false` otherwise.
             */
            [[nodiscard]] bool isValid() const override;

            /**
             * @brief Returns the class name.
             * @return The string "iterAdaptor".
             */
            [[nodiscard]] std::string className() const override;

            /**
             * @brief Converts the iterator to a string representation.
             * @param enter If `true`, adds a newline at the end.
             * @return A string representation of the iterator.
             */
            [[nodiscard]] std::string toString(bool enter) const override;

            /**
             * @brief Destructor for `iterAdaptor`.
             * @details Deletes the underlying base iterator.
             */
            ~iterAdaptor() override;
        };

        virtual ~iterable() = default;

        /**
         * @brief Returns an iterator pointing to the beginning of the iterable container.
         * @return An iterator to the first element.
         * @details This function allows using the range-based `for` loop syntax.
         */
        iterAdaptor begin();

        /**
         * @brief Returns an iterator pointing to the end of the iterable container.
         * @return An iterator to one past the last element.
         * @details This function allows using the range-based `for` loop syntax.
         */
        iterAdaptor end();

        /**
         * @brief Returns a constant iterator pointing to the beginning of the iterable container.
         * @return A constant iterator to the first element.
         * @details This function allows using the range-based `for` loop syntax.
         */
        iterAdaptor begin() const;

        /**
         * @brief Returns a constant iterator pointing to the end of the iterable container.
         * @return A constant iterator to one past the last element.
         * @details This function allows using the range-based `for` loop syntax.
         */
        iterAdaptor end() const;

        /**
         * @brief Returns an iterator pointing to the first element.
         * @return An iterator to the first element.
         */
        iterAdaptor first();

        /**
         * @brief Returns an iterator pointing to the last element.
         * @return An iterator to the last element.
         */
        iterAdaptor last();

        /**
         * @brief Returns a constant iterator pointing to the first element.
         * @return A constant iterator to the first element.
         */
        iterAdaptor first() const;

        /**
         * @brief Returns a constant iterator pointing to the last element.
         * @return A constant iterator to the last element.
         */
        iterAdaptor last() const;

        /**
         * @brief Returns the iterator to the beginning of the container.
         * @return A pointer to the base iterator that marks the start of the container.
         * @details This method is used internally by `begin()`, but can be accessed directly for other purposes.
         */
        virtual baseIterator<TYPE>* begins() const = 0;

        /**
         * @brief Returns the iterator to the end of the container.
         * @return A pointer to the base iterator that marks the end of the container.
         * @details This method is used internally by `end()`, but can be accessed directly for other purposes.
         */
        virtual baseIterator<TYPE>* ends() const = 0;

        /**
         * @brief Applies a given operation to each element in the iterable container.
         * @tparam Callback A callable object that defines the operation to be applied to each element.
         * @param operation The operation to be applied.
         */
        template<typename Callback = transform<TYPE>>
        requires Operation<Callback, TYPE>
        void forEach(Callback operation = Callback{});

        /**
         * @brief Applies a given operation to each element in the iterable container (const version).
         * @tparam Callback A callable object that defines the operation to be applied to each element.
         * @param operation The operation to be applied.
         */
        template<typename Callback = transform<TYPE>>
        requires Operation<Callback, TYPE>
        void forEach(const Callback& operation = Callback{}) const;
    };
}

    template <typename TYPE>
    original::iterable<TYPE>::iterAdaptor::iterAdaptor(baseIterator<TYPE>* it) : it_(it) {}

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::equalPtr(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(other);
        if (other_it == nullptr)
            return this->it_->equal(other);
        return this->it_->equal(other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::clone() const -> iterAdaptor*
    {
        return new iterAdaptor(*this);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::getPrev() const -> iterAdaptor*
    {
        auto* it = this->clone();
        it->next();
        return it;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::getNext() const -> iterAdaptor*
    {
        auto* it = this->clone();
        it->prev();
        return it;
    }

    template <typename TYPE>
    original::iterable<TYPE>::iterAdaptor::iterAdaptor(const iterAdaptor& other) : iterAdaptor(nullptr)
    {
        this->operator=(other);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator=(const iterAdaptor& other) -> iterAdaptor&
    {
        if (this == &other) return *this;

        delete this->it_;
        this->it_ = other.it_->clone();
        return *this;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::getIt() const -> const iterator<TYPE>&
    {
        return *this->it_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::hasNext() const -> bool
    {
        return this->it_->hasNext();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::hasPrev() const -> bool
    {
        return this->it_->hasPrev();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::atPrev(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(other);
        if (other_it == nullptr)
            return this->it_->atPrev(other);
        return this->it_->atPrev(other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::atNext(const iterator<TYPE>* other) const -> bool
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(other);
        if (other_it == nullptr)
            return this->it_->atNext(other);
        return this->it_->atNext(other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::next() const -> void
    {
        this->it_->next();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::prev() const -> void
    {
        this->it_->prev();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator+=(integer steps) const -> void
    {
        this->it_->operator+=(steps);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator-=(integer steps) const -> void
    {
        this->it_->operator-=(steps);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::operator-(const iterator<TYPE>& other) const -> integer
    {
        auto* other_it = dynamic_cast<const iterAdaptor*>(&other);
        if (other_it == nullptr)
            return this->it_->operator-(other);
        return this->it_->operator-(*other_it->it_);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::get() -> TYPE&
    {
        return this->it_->get();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::set(const TYPE& data) -> void
    {
        this->it_->set(data);
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::get() const -> TYPE
    {
        return this->it_->getElem();
    }

    template<typename TYPE>
    bool original::iterable<TYPE>::iterAdaptor::isValid() const {
        return this->it_->isValid();
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::className() const -> std::string {
        return "iterAdaptor";
    }

    template<typename TYPE>
    auto original::iterable<TYPE>::iterAdaptor::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className();
        ss << "(" << *this->it_ << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

    template<typename TYPE>
    original::iterable<TYPE>::iterAdaptor::~iterAdaptor() {
        delete this->it_;
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() -> iterAdaptor {
        return iterAdaptor(this->begins());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() -> iterAdaptor {
        auto* it = this->ends();
        it->next();
        return iterAdaptor(it);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::begin() const -> iterAdaptor {
        return iterAdaptor(this->begins());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::end() const -> iterAdaptor{
        auto* it = this->ends();
        it->next();
        return iterAdaptor(it);
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::first() -> iterAdaptor
    {
        return this->begin();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::last() -> iterAdaptor
    {
        return iterAdaptor(this->ends());
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::first() const -> iterAdaptor
    {
        return this->begin();
    }

    template <typename TYPE>
    auto original::iterable<TYPE>::last() const -> iterAdaptor
    {
        return iterAdaptor(this->ends());
    }

    template <typename TYPE>
    template<typename Callback>
    requires original::Operation<Callback, TYPE>
    auto original::iterable<TYPE>::forEach(Callback operation) -> void
    {
        for (auto* it = this->begins(); it->isValid(); it->next()) {
            operation(it->get());
        }
    }

    template<typename TYPE>
    template<typename Callback>
    requires original::Operation<Callback, TYPE>
    auto original::iterable<TYPE>::forEach(const Callback &operation) const -> void {
        for (auto* it = this->begins(); it->isValid(); it->next()) {
            operation(it->getElem());
        }
    }

#endif //ITERABLE_H
