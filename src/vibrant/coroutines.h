#ifndef ORIGINAL_COROUTINES_H
#define ORIGINAL_COROUTINES_H
#include "optional.h"
#include <coroutine>
#include <exception>


namespace original {
    class coroutine {
    public:
        template<typename TYPE>
        class generator {
            struct promise_type {
                bool initial_staus = false;
                alternative<TYPE> value_;
                std::exception_ptr e_;

                generator get_return_object();

                static std::suspend_always initial_suspend();

                static std::suspend_always final_suspend() noexcept;

                static void return_void();

                std::suspend_always yield_value(TYPE value);

                void unhandled_exception();

                void rethrow_if_exception() const;
            };

            class iterator {
                generator* gen_;
                bool end_;

                iterator();

                explicit iterator(generator* gen);
            public:
                friend coroutine;

                iterator& operator++();

                TYPE operator*();

                bool operator!=(const iterator& other) const;

                bool operator==(const iterator& other) const;
            };

            using handle = std::coroutine_handle<promise_type>;

            handle handle_;
        public:
            using promise_type = promise_type;
            using iterator = iterator;

            generator(const generator&) = delete;
            generator& operator=(const generator&) = delete;

            generator(generator&& other) noexcept;

            generator& operator=(generator&& other) noexcept;

            generator() = default;

            explicit generator(handle h);

            iterator begin();

            static iterator end();

            [[nodiscard]] bool launched() const;

            [[nodiscard]] bool hasNext() const;

            [[nodiscard]] alternative<TYPE> peek() const;

            alternative<TYPE> next();

            ~generator();
        };
    };
}

template <typename TYPE>
original::coroutine::generator<TYPE>
original::coroutine::generator<TYPE>::promise_type::get_return_object()
{
    return generator{handle::from_promise(*this)};
}

template <typename TYPE>
std::suspend_always original::coroutine::generator<TYPE>::promise_type::initial_suspend()
{
    return std::suspend_always{};
}

template <typename TYPE>
std::suspend_always original::coroutine::generator<TYPE>::promise_type::final_suspend() noexcept
{
    return std::suspend_always{};
}

template <typename TYPE>
void original::coroutine::generator<TYPE>::promise_type::return_void() {}

template <typename TYPE>
std::suspend_always original::coroutine::generator<TYPE>::promise_type::yield_value(TYPE value)
{
    if (!this->initial_staus) {
        this->initial_staus = true;
    }
    this->value_.set(std::move(value));
    return std::suspend_always{};
}

template <typename TYPE>
void original::coroutine::generator<TYPE>::promise_type::unhandled_exception()
{
    this->e_ = std::current_exception();
}

template <typename TYPE>
void original::coroutine::generator<TYPE>::promise_type::rethrow_if_exception() const
{
    if (this->e_)
        throw this->e_;
}

template <typename TYPE>
original::coroutine::generator<TYPE>::iterator::iterator() : gen_(nullptr), end_(true) {}

template <typename TYPE>
original::coroutine::generator<TYPE>::iterator::iterator(generator* gen) : gen_(gen), end_(false)
{
    if (!this->gen_) {
        this->end_ = true;
        return;
    }
    ++*this;
}

template <typename TYPE>
original::coroutine::generator<TYPE>::iterator&
original::coroutine::generator<TYPE>::iterator::operator++()
{
    if (!this->gen_)
        return *this;
    auto v = this->gen_->next();
    if (!v) {
        this->end_ = true;
    }
    return *this;
}

template <typename TYPE>
TYPE original::coroutine::generator<TYPE>::iterator::operator*()
{
    if (!this->gen_)
        throw nullPointerError("Dereferencing end iterator");
    return *this->gen_->peek();
}

template <typename TYPE>
bool original::coroutine::generator<TYPE>::iterator::operator!=(const iterator& other) const
{
    if (!this->gen_ && !other.gen_)
        return true;
    if (!this->gen_ || !other.gen_)
        return this->end_ != other.end_;

    return this->gen_ != other.gen_ || this->end_ != other.end_;
}

template <typename TYPE>
bool original::coroutine::generator<TYPE>::iterator::operator==(const iterator& other) const
{
    return !(*this != other);
}

template <typename TYPE>
original::coroutine::generator<TYPE>::generator(generator&& other) noexcept
{
    this->handle_ = other.handle_;
    other.handle_ = nullptr;
}

template <typename TYPE>
original::coroutine::generator<TYPE>&
original::coroutine::generator<TYPE>::operator=(generator&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    this->handle_ = other.handle_;
    other.handle_ = nullptr;
    return *this;
}

template <typename TYPE>
original::coroutine::generator<TYPE>::generator(handle h) : handle_(std::move(h)) {}

template <typename TYPE>
original::coroutine::generator<TYPE>::iterator
original::coroutine::generator<TYPE>::begin()
{
    return iterator{this};
}

template <typename TYPE>
original::coroutine::generator<TYPE>::iterator
original::coroutine::generator<TYPE>::end()
{
    return iterator{};
}

template <typename TYPE>
bool original::coroutine::generator<TYPE>::launched() const
{
    return this->handle_.promise().initial_staus;
}

template <typename TYPE>
bool original::coroutine::generator<TYPE>::hasNext() const
{
    return this->handle_ && !this->handle_.done();
}

template <typename TYPE>
original::alternative<TYPE> original::coroutine::generator<TYPE>::peek() const
{
    return this->handle_.promise().value_;
}

template <typename TYPE>
original::alternative<TYPE>
original::coroutine::generator<TYPE>::next()
{
    if (!this->hasNext())
        return alternative<TYPE>{};

    this->handle_.resume();

    if (!this->hasNext())
        return alternative<TYPE>{};

    this->handle_.promise().rethrow_if_exception();
    return this->handle_.promise().value_;
}

template <typename TYPE>
original::coroutine::generator<TYPE>::~generator()
{
    if (this->handle_)
        this->handle_.destroy();
}

#endif //ORIGINAL_COROUTINES_H
