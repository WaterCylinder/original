#ifndef ITERATOR_H
#define ITERATOR_H

#include "cloneable.h"
#include "comparable.h"
#include "error.h"
#include "printable.h"

namespace original {

/**
 * @file iterator.h
 * @brief Defines the iterator class for traversing and manipulating container elements.
 * @details Provides the base class for iterators, implementing common operations such as dereferencing,
 *          comparison, cloning, and navigating through elements. Derived classes must implement specific
 *          behavior such as how to move to the next or previous element, and how to get and set the element's value.
 */

/**
 * @class iterator
 * @tparam TYPE The type of elements the iterator traverses.
 * @brief Base iterator interface that supports common operations for iteration.
 * @extends printable
 * @extends cloneable
 * @extends comparable
 * @details This class provides common operations for iterators, including dereferencing, moving forward and backward,
 *          and comparing iterators. It also supports cloning, and printing elements.
 *
 * Derived classes are expected to provide specific implementations for methods like `next()`, `prev()`,
 * `get()`, and `set()`. These implementations define the behavior of the iterator in a particular container.
 *
 * Implements comparison based on a custom `compareTo` method which compares iterators by the distance between them.
 */
template<typename TYPE>
class iterator : public printable, public cloneable, public comparable<iterator<TYPE>> {
protected:
    /**
     * @brief Checks if two iterators point to the same object.
     * @param other The iterator to compare with.
     * @return True if the iterators point to the same object, false otherwise.
     */
    virtual bool equalPtr(const iterator* other) const = 0;

public:
    /**
     * @brief Dereferences the iterator to get the element.
     * @return A reference to the element pointed to by the iterator.
     */
    TYPE& operator*();

    /**
     * @brief Dereferences the iterator to get a copy of the element.
     * @return A copy of the element pointed to by the iterator.
     */
    TYPE operator*() const;

    /**
     * @brief Moves the iterator forward by one position.
     */
    void operator++() const;

    /**
     * @brief Moves the iterator forward by one position (postfix).
     * @param postfix Unused parameter for postfix syntax.
     */
    void operator++(int postfix) const;

    /**
     * @brief Moves the iterator backward by one position.
     */
    void operator--() const;

    /**
     * @brief Moves the iterator backward by one position (postfix).
     * @param postfix Unused parameter for postfix syntax.
     */
    void operator--(int postfix) const;

    /**
     * @brief Adds a number of steps to the current position of iterator.
     * @param steps The number of steps to move forward.
     */
    virtual void operator+=(int64_t steps) const = 0;

    /**
     * @brief Subtracts a number of steps from the current position of iterator.
     * @param steps The number of steps to move backward.
     */
    virtual void operator-=(int64_t steps) const = 0;

    /**
     * @brief Compares two iterators to determine their relative positions.
     * @param other The iterator to compare with.
     * @return A negative value if this iterator is before the other, zero if equal, and a positive value if after.
     */
    int64_t compareTo(const iterator& other) const override;

    /**
     * @brief Returns the distance between this iterator and another iterator.
     * @param other The iterator to compare against.
     * @return The number of steps between the two iterators.
     */
    virtual int64_t operator-(const iterator& other) const = 0;

    /**
     * @brief Creates a clone of the iterator.
     * @return A new copy of the iterator.
     */
    iterator* clone() const override = 0;

    /**
     * @brief Checks if the iterator is valid (i.e., points to a valid element).
     * @return True if the iterator is valid, false otherwise.
     */
    explicit operator bool() const;

    /**
     * @brief Checks if there is a next element.
     * @return True if there is a next element, false otherwise.
     */
    [[nodiscard]] virtual bool hasNext() const = 0;

    /**
     * @brief Checks if there is a previous element.
     * @return True if there is a previous element, false otherwise.
     */
    [[nodiscard]] virtual bool hasPrev() const = 0;

    /**
     * @brief Checks if this iterator is positioned at the previous element.
     * @param other The iterator to compare with.
     * @return True if this iterator is at the previous position of the other, false otherwise.
     */
    virtual bool atPrev(const iterator* other) const = 0;

    /**
     * @brief Checks if this iterator is positioned at the next element.
     * @param other The iterator to compare with.
     * @return True if this iterator is at the next position of the other, false otherwise.
     */
    virtual bool atNext(const iterator* other) const = 0;

    /**
     * @brief Checks if this iterator is positioned at the previous element.
     * @param other The iterator to compare with.
     * @return True if this iterator is at the previous position of the other, false otherwise.
     */
    bool atPrev(const iterator& other) const;

    /**
     * @brief Checks if this iterator is positioned at the next element.
     * @param other The iterator to compare with.
     * @return True if this iterator is at the next position of the other, false otherwise.
     */
    bool atNext(const iterator& other) const;

    /**
     * @brief Moves the iterator to the next element.
     */
    virtual void next() const = 0;

    /**
     * @brief Moves the iterator to the previous element.
     */
    virtual void prev() const = 0;

    /**
     * @brief Returns a new iterator pointing to the next element.
     * @return A new iterator pointing to the next element.
     */
    virtual iterator* getNext() const;

    /**
     * @brief Returns a new iterator pointing to the previous element.
     * @return A new iterator pointing to the previous element.
     */
    virtual iterator* getPrev() const = 0;

    /**
     * @brief Gets the element pointed to by the iterator.
     * @return A reference to the element.
     */
    virtual TYPE& get() = 0;

    /**
     * @brief Gets a copy of the element pointed to by the iterator.
     * @return A copy of the element.
     */
    virtual TYPE get() const = 0;

    /**
     * @brief Returns a copy of the element.
     * @return A copy of the element.
     */
    virtual TYPE getElem() const;

    /**
     * @brief Sets the element pointed to by the iterator.
     * @param data The new value to set.
     */
    virtual void set(const TYPE& data) = 0;

    /**
     * @brief Checks if two iterators are equal.
     * @param other The iterator to compare with.
     * @return True if the iterators are equal, false otherwise.
     */
    bool equal(const iterator* other) const;

    /**
     * @brief Checks if two iterators are equal.
     * @param other The iterator to compare with.
     * @return True if the iterators are equal, false otherwise.
     */
    bool equal(const iterator& other) const;

    /**
     * @brief Checks if the iterator is valid.
     * @return True if the iterator is valid, false otherwise.
     */
    [[nodiscard]] virtual bool isValid() const = 0;

    /**
     * @brief Returns the class name of the iterator.
     * @return A string representing the class name.
     */
    [[nodiscard]] std::string className() const override;

    /**
     * @brief Returns a string representation of the iterator.
     * @param enter If true, adds a newline after the string.
     * @return A string representing the iterator.
     */
    [[nodiscard]] std::string toString(bool enter) const override;

    /**
     * @brief Virtual destructor for proper cleanup of derived objects.
     */
    ~iterator() override = default;

    // Friends for operator overloading
    template<typename T>
    friend iterator<T>* operator+(const iterator<T>& it, int64_t steps);

    template<typename T>
    friend iterator<T>* operator-(const iterator<T>& it, int64_t steps);
};

/**
 * @brief A base class for basic iterators.
 * @extends iterator
 * @tparam TYPE The type of the elements the iterator will traverse.
 * @details The `baseIterator` serves as the foundation for basic iterators. It defines common
 *          functionality for iterators, and it can be extended by other iterator types. For
 *          more specialized or adapted iterators, see @ref original::iterable::iterAdaptor and other
 *          iterator types that inherit from `baseIterator`.
 */
template <typename TYPE>
class baseIterator : public iterator<TYPE> {
public:
    /**
     * @brief Creates a clone of the iterator.
     * @return A new copy of the iterator.
     */
    baseIterator* clone() const override = 0;

    /**
     * @brief Virtual destructor for proper cleanup of derived objects.
     */
    ~baseIterator() override = default;
};

/**
 * @brief Adds a number of steps to the iterator's current position and returns a new iterator.
 * @tparam T The type of the elements the iterator will traverse.
 * @param it The iterator to move.
 * @param steps The number of steps to move forward.
 * @return A new iterator that is moved forward by the specified steps from the original iterator.
 * @details This operator does not modify the original iterator, but creates a new one that is
 *          advanced by `steps` positions.
 */
template <typename T>
auto operator+(const iterator<T>& it, int64_t steps) -> iterator<T>*;

/**
 * @brief Subtracts a number of steps from the iterator's current position and returns a new iterator.
 * @tparam T The type of the elements the iterator will traverse.
 * @param it The iterator to move.
 * @param steps The number of steps to move backward.
 * @return A new iterator that is moved backward by the specified steps from the original iterator.
 * @details This operator does not modify the original iterator, but creates a new one that is
 *          moved backward by `steps` positions.
 */
template <typename T>
auto operator-(const iterator<T>& it, int64_t steps) -> iterator<T>*;

} // namespace original
    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() -> TYPE& {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator*() const -> TYPE {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++() const -> void {
        this->next();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator++(int) const -> void {
        this->operator++();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator--() const -> void {
        this->prev();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::operator--(int) const -> void {
        this->operator--();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::compareTo(const iterator &other) const -> int64_t {
        return this->operator-(other);
    }

    template<typename TYPE>
    original::iterator<TYPE>::operator bool() const {
        return this->isValid();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atPrev(const iterator &other) const -> bool {
        return this->atPrev(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::atNext(const iterator &other) const -> bool {
        return this->atNext(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::getNext() const -> iterator* {
        if (!this->isValid()) throw outOfBoundError();
        auto it = this->clone();
        it->next();
        return it;
    }

    template <typename TYPE>
    auto original::iterator<TYPE>::getElem() const -> TYPE
    {
        return this->get();
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::equal(const iterator *other) const -> bool {
        return this->equalPtr(other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::equal(const iterator &other) const -> bool {
        return this->equal(&other);
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::className() const -> std::string {
        return "iterator";
    }

    template<typename TYPE>
    auto original::iterator<TYPE>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className() << "(";
        if (this->isValid()) ss << formatString(this->get());
        ss << ")";
        if (enter) {
            ss << "\n";
        }
        return ss.str();
    }

    template <typename TYPE>
    auto original::operator+(const iterator<TYPE>& it, int64_t steps) -> iterator<TYPE>*
    {
        auto* nit = it.clone();
        nit->operator+=(steps);
        return nit;
    }

    template <typename TYPE>
    auto original::operator-(const iterator<TYPE>& it, int64_t steps) -> iterator<TYPE>*
    {
        auto* nit = it.clone();
        nit->operator-=(steps);
        return nit;
    }

#endif //ITERATOR_H
