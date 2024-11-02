#ifndef VECTOR_H
#define VECTOR_H

#include "serial.h"
#include "iterable.h"
#include "array.h"

namespace original{
    template <typename TYPE>
    class vector final : public serial<TYPE>, public iterable<TYPE>, public printable{
        class vectorNode final : public wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class vector;
            private:
                TYPE data_;
            protected:
                explicit vectorNode(TYPE data);
                TYPE& getVal() override;
                void setVal(TYPE data) override;
                vectorNode* getPPrev() override;
                vectorNode* getPNext() override;
        };

        size_t size_;
        const size_t INNER_SIZE_INIT = 16;
        size_t max_size;
        size_t inner_begin;
        vectorNode** body;

        static vectorNode** vectorNodeArrayInit(size_t size);
        static void moveElements(vectorNode** old_body, size_t inner_idx,
                          size_t len, vectorNode** new_body, size_t offset);
        size_t toInnerIdx(int index);
        _GLIBCXX_NODISCARD bool outOfMaxSize(size_t inner_begin, size_t increment) const;
        void grow(size_t new_size);
        void adjust(size_t increment);
    public:
        explicit vector();
        vector(std::initializer_list<TYPE> list);
//        explicit vector(array<TYPE> arr);
        _GLIBCXX_NODISCARD size_t size() const override;
        TYPE get(int index) override;
//        TYPE operator[](int index) override;
//        void set(int index, TYPE e) override;
        size_t indexOf(TYPE e) override;
//        void pushBegin(TYPE e) override;
//        void push(int index, TYPE e) override;
//        void pushEnd(TYPE e) override;
//        TYPE popBegin() override;
//        TYPE pop(int index) override;
//        TYPE popEnd() override;
//        iterator<TYPE>* begins() override;
//        iterator<TYPE>* ends() override;
//        _GLIBCXX_NODISCARD std::string toString(bool enter) override;
//        ~vector() override;
    };
}

    template<typename TYPE>
    original::vector<TYPE>::vectorNode::vectorNode(TYPE data) : data_(data) {}

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getVal() -> TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::setVal(TYPE data) -> void
    {
        this->data_ = data;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getPPrev() -> vectorNode* {
        return this - 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getPNext() -> vectorNode* {
        return this + 1;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNodeArrayInit(const size_t size) -> vectorNode**
    {
        auto** arr = new vectorNode*[size];
        for (int i = 0; i < size; i += 1)
        {
            arr[i] = nullptr;
        }
        return arr;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::moveElements(vectorNode** old_body, const size_t inner_idx,
        const size_t len, vectorNode** new_body, const size_t offset) -> void{
        if (offset > 0)
        {
            for (int i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + len - 1 - i] = old_body[inner_idx + len - 1 - i];
                if (old_body == new_body)
                    old_body[inner_idx + len - 1 - i] = nullptr;
            }
        }else
        {
            for (int i = 0; i < len; i += 1)
            {
                new_body[inner_idx + offset + i] = old_body[inner_idx + i];
                if (old_body == new_body)
                    old_body[inner_idx + i] = nullptr;
            }
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::toInnerIdx(int index) -> size_t
    {
        return this->inner_begin + index;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::outOfMaxSize(size_t inner_begin, size_t increment) const -> bool
    {
        return inner_begin + this->size() - 1 + increment > this->max_size || inner_begin - increment < 0;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::grow(const size_t new_size) -> void
    {
        vectorNode** new_body = vector::vectorNodeArrayInit(new_size);
        size_t new_begin = (new_size - 1) / 4;
        vector::moveElements(this->body, this->inner_begin,
                             this->size(), new_body, new_begin - this->inner_begin);
        delete this->body;
        this->body = new_body;
        this->max_size = new_size;
        this->inner_begin = new_begin;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::adjust(size_t increment) -> void
    {
        if (!this->outOfMaxSize(this->inner_begin, increment))
        {
            return;
        }
        if (const int size_diff = this->max_size - this->size() - increment; size_diff >= 0)
        {
            size_t new_begin = size_diff / 2;
            vector::moveElements(this->body, this->inner_begin, this->size(), this->body,
                                 new_begin - this->inner_begin);
            this->inner_begin = new_begin;
        }else
        {
            const size_t new_max_size = (this->size() + increment) * 2;
            grow(new_max_size);
        }
    }

    template <typename TYPE>
    original::vector<TYPE>::vector() : size_(0), max_size(this->INNER_SIZE_INIT),
        inner_begin((this->INNER_SIZE_INIT - 1)/2), body(vector::vectorNodeArrayInit(this->INNER_SIZE_INIT)) {}

    template <typename TYPE>
    original::vector<TYPE>::vector(std::initializer_list<TYPE> list) : vector()
    {
        vector::adjust(list.size());
        for (TYPE e : list)
        {
            this->body[this->inner_begin + this->size()] = new vectorNode(e);
            this->size_ += 1;
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::size() const -> size_t
    {
        return this->size_;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::indexOf(TYPE e) -> size_t
    {
    for (int i = 0; i < this->size(); i += 1)
    {
        if (this->get(i) == e)
        {
            return i;
        }
    }
    return this->size();
    }

#endif //VECTOR_H
