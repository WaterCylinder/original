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
                vectorNode* prev;
                vectorNode* next;
            protected:
                explicit vectorNode(TYPE data, vectorNode* prev = nullptr, vectorNode* next = nullptr);
                vectorNode(const vectorNode& other);
                vectorNode& operator=(const vectorNode& other);
                TYPE& getVal() override;
                const TYPE& getVal() const override;
                void setVal(TYPE data) override;
                vectorNode* getPPrev() const override;
                vectorNode* getPNext() const override;
                void setPPrev(vectorNode* new_prev);
                void setPNext(vectorNode* new_next);
                ~vectorNode();
                static void connect(vectorNode* prev, vectorNode* next);
        };

        size_t size_;
        const size_t INNER_SIZE_INIT = 16;
        size_t max_size;
        size_t inner_begin;
        vectorNode** body;

        static vectorNode** vectorNodeArrayInit(size_t size);
        static void moveElements(vectorNode** old_body, size_t inner_idx,
                          size_t len, vectorNode** new_body, int offset);
        _GLIBCXX_NODISCARD size_t toInnerIdx(int index) const;
        _GLIBCXX_NODISCARD bool outOfMaxSize(size_t increment) const;
        void connectAll();
        void grow(size_t new_size);
        void adjust(size_t increment);
    public:
        explicit vector();
        vector(const vector& other);
        vector(std::initializer_list<TYPE> list);
        explicit vector(array<TYPE> arr);
        vector& operator=(const vector& other);
        bool operator==(const vector& other) const;
        _GLIBCXX_NODISCARD size_t size() const override;
        TYPE get(int index) const override;
        TYPE operator[](int index) const override;
        void set(int index, TYPE e) override;
        size_t indexOf(TYPE e) const override;
        void pushBegin(TYPE e) override;
        void push(int index, TYPE e) override;
        void pushEnd(TYPE e) override;
        TYPE popBegin() override;
        TYPE pop(int index) override;
        TYPE popEnd() override;
        iterator<TYPE>* begins() override;
        iterator<TYPE>* ends() override;
        _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
        ~vector() override;
    };
}

    template<typename TYPE>
    original::vector<TYPE>::vectorNode::vectorNode(TYPE data, vectorNode* prev, vectorNode* next)
        : data_(data), prev(prev), next(next) {}

    template<typename TYPE>
    original::vector<TYPE>::vectorNode::vectorNode(const vectorNode& other)
        : data_(other.data_), prev(other.prev), next(other.next) {}

    template<typename TYPE>
    auto original::vector<TYPE>::vectorNode::operator=(const vectorNode& other) -> vectorNode& {
        if (this != &other) {
            data_ = other.data_;
            prev = other.prev;
            next = other.next;
        }
        return *this;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getVal() -> TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getVal() const -> const TYPE&
    {
        return this->data_;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::setVal(TYPE data) -> void
    {
        this->data_ = data;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getPPrev() const -> vectorNode* {
        return this->prev;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::vectorNode::getPNext() const -> vectorNode* {
        return this->next;
    }

    template <typename TYPE>
    void original::vector<TYPE>::vectorNode::setPPrev(vectorNode* new_prev){
        this->prev = new_prev;
    }

    template <typename TYPE>
    void original::vector<TYPE>::vectorNode::setPNext(vectorNode* new_next){
        this->next = new_next;
    }

    template <typename TYPE>
    original::vector<TYPE>::vectorNode::~vectorNode()= default;

    template <typename TYPE>
    void original::vector<TYPE>::vectorNode::connect(vectorNode* prev, vectorNode* next){
        if (prev != nullptr) prev->setPNext(next);
        if (next != nullptr) next->setPPrev(prev);
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
        const size_t len, vectorNode** new_body, const int offset) -> void{
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
    auto original::vector<TYPE>::toInnerIdx(int index) const -> size_t
    {
        return this->inner_begin + index;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::outOfMaxSize(size_t increment) const -> bool
    {
        return this->inner_begin + this->size() + increment > this->max_size || this->inner_begin - increment < 1;
    }

    template <typename TYPE>
    void original::vector<TYPE>::connectAll(){
        for (int i = 0; i < (int)(this->size() - 1); ++i) {
            vectorNode::connect(this->body[this->toInnerIdx(i)],
                                this->body[this->toInnerIdx(i + 1)]);
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::grow(const size_t new_size) -> void
    {
        vectorNode** new_body = vector::vectorNodeArrayInit(new_size);
        size_t new_begin = (new_size - 1) / 4;
        vector::moveElements(this->body, this->inner_begin,
                             this->size(), new_body, new_begin - this->inner_begin);
        delete[] this->body;
        this->body = new_body;
        this->max_size = new_size;
        this->inner_begin = new_begin;
        this->connectAll();
    }

    template <typename TYPE>
    auto original::vector<TYPE>::adjust(size_t increment) -> void {
        if (!this->outOfMaxSize(increment)) {
            return;
        }
        if (this->max_size >= this->size_ + increment) {
            size_t new_begin = (this->max_size - this->size() - increment) / 2;
            vector::moveElements(this->body, this->inner_begin, this->size(), this->body,
                                 new_begin - this->inner_begin);
            this->inner_begin = new_begin;
            this->connectAll();
        } else {
            size_t new_max_size = (this->size() + increment) * 2;
            this->grow(new_max_size);
        }
    }

    template <typename TYPE>
    original::vector<TYPE>::vector() : size_(0), max_size(this->INNER_SIZE_INIT),
        inner_begin((this->INNER_SIZE_INIT - 1)/2), body(vector::vectorNodeArrayInit(this->INNER_SIZE_INIT)) {}

    template<typename TYPE>
    original::vector<TYPE>::vector(const vector& other) : vector(){
        this->operator=(other);
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(std::initializer_list<TYPE> list) : vector()
    {
        this->adjust(list.size());
        for (TYPE e : list)
        {
            this->body[this->inner_begin + this->size()] = new vectorNode(e);
            this->size_ += 1;
        }
        this->connectAll();
    }

    template<typename TYPE>
    original::vector<TYPE>& original::vector<TYPE>::operator=(const vector& other){
        if (this == &other) return *this;
        for (size_t i = 0; i < this->max_size; ++i) {
            if (this->body[i] != nullptr) {
                delete this->body[i];
            }
        }
        delete[] this->body;
        this->max_size = other.max_size;
        this->inner_begin = other.inner_begin;
        this->size_ = other.size_;
        this->body = vector::vectorNodeArrayInit(this->max_size);
        for (int i = 0; i < this->size(); ++i) {
            TYPE data = other.body[this->toInnerIdx(i)]->getVal();
            this->body[this->toInnerIdx(i)] = new vectorNode(data);
        }
        this->connectAll();
        return *this;
    }

    template<typename TYPE>
    bool original::vector<TYPE>::operator==(const vector& other) const{
        if (this == &other) return true;
        if (this->size() != other.size()) return false;
        for (int i = 0; i < this->size(); ++i) {
            size_t index = this->toInnerIdx(i);
            if (this->body[index]->getVal() != other.body[index]->getVal()){
                return false;
            }
        }
        return true;
    }

    template <typename TYPE>
    original::vector<TYPE>::vector(array<TYPE> arr) : vector()
    {
        this->adjust(arr.size());
        for (int i = 0; i < arr.size(); i += 1)
        {
            this->body[this->toInnerIdx(i)] = new vectorNode(arr.get(i));
            this->size_ += 1;
        }
        this->connectAll();
    }

template <typename TYPE>
    auto original::vector<TYPE>::size() const -> size_t
    {
        return this->size_;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::get(int index) const -> TYPE
    {
        if (this->indexOutOfBound(index))
        {
            throw indexError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        return this->body[index]->getVal();
    }

    template <typename TYPE>
    auto original::vector<TYPE>::operator[](const int index) const -> TYPE
    {
        return this->get(index);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::set(int index, TYPE e) -> void
    {
        if (this->indexOutOfBound(index))
        {
            throw indexError();
        }
        index = this->toInnerIdx(this->parseNegIndex(index));
        this->body[index]->setVal(e);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::indexOf(TYPE e) const -> size_t
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

    template <typename TYPE>
    auto original::vector<TYPE>::pushBegin(TYPE e) -> void
    {
        this->adjust(1);
        this->inner_begin -= 1;
        this->body[this->toInnerIdx(0)] = new vectorNode(e);
        this->size_ += 1;
        vectorNode::connect(this->body[this->toInnerIdx(0)],
                            this->body[this->toInnerIdx(1)]);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::push(int index, TYPE e) -> void
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
                throw indexError();
            }
            this->adjust(1);
            index = this->toInnerIdx(this->parseNegIndex(index));
            size_t rel_idx = index - this->inner_begin;
            if (index - this->inner_begin <= (this->size() - 1) / 2)
            {
                vector::moveElements(this->body, this->inner_begin,
                                     rel_idx + 1, this->body, -1);
                this->inner_begin -= 1;
            }else
            {
                vector::moveElements(this->body, index,
                    this->size() - 1 - rel_idx, this->body, 1);
            }
            this->body[index] = new vectorNode(e);
            this->size_ += 1;
            vectorNode::connect(this->body[index - 1], this->body[index]);
            vectorNode::connect(this->body[index], this->body[index + 1]);
        }
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pushEnd(TYPE e) -> void
    {
        this->adjust(1);
        this->body[this->toInnerIdx(this->size())] = new vectorNode(e);
        this->size_ += 1;
        vectorNode::connect(this->body[this->toInnerIdx(this->size() - 2)],
                            this->body[this->toInnerIdx(this->size() - 1)]);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::popBegin() -> TYPE
    {
        if (this->size() == 0){
            throw noElementError();
        }
        size_t index = this->toInnerIdx(0);
        TYPE res = this->get(0);
        delete this->body[index];
        this->body[index] = nullptr;
        this->inner_begin += 1;
        this->size_ -= 1;
        vectorNode::connect(nullptr, this->body[this->toInnerIdx(0)]);
        return res;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::pop(int index) -> TYPE
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
            throw indexError();
        }
        TYPE res = this->get(index);
        index = this->toInnerIdx(this->parseNegIndex(index));
        delete this->body[index];
        this->body[index] = nullptr;
        size_t rel_idx = index - this->inner_begin;
        if (index - this->inner_begin <= (this->size() - 1) / 2)
        {
            vector::moveElements(this->body, this->inner_begin,
                                 rel_idx, this->body, 1);
            this->inner_begin += 1;
            vectorNode::connect(this->body[index], this->body[index + 1]);
        }else
        {
            vector::moveElements(this->body, index + 1,
                this->size() - 1 - rel_idx, this->body, -1);
            vectorNode::connect(this->body[index - 1], this->body[index]);
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
        size_t index = this->toInnerIdx(this->size() - 1);
        TYPE res = this->get(this->size() - 1);
        delete this->body[index];
        this->body[index] = nullptr;
        this->size_ -= 1;
        vectorNode::connect(this->body[this->toInnerIdx(this->size() - 1)], nullptr);
        return res;
    }

    template <typename TYPE>
    auto original::vector<TYPE>::begins() -> iterator<TYPE>*
    {
        size_t index = this->toInnerIdx(0);
        return new iterator<TYPE>(this->body[index]);
    }

    template <typename TYPE>
    auto original::vector<TYPE>::ends() -> iterator<TYPE>*
    {
        size_t index = this->toInnerIdx(this->size() - 1);
        return new iterator<TYPE>(this->body[index]);
    }

    template <typename TYPE>
    std::string original::vector<TYPE>::toString(const bool enter) const
    {
        std::stringstream ss;
        ss << "vector" << "(";
        for (int i = 0; i < this->size(); i += 1)
        {
            size_t index = this->toInnerIdx(i);
            ss << this->body[index]->toString(false);
            if (i < this->size() - 1)
            {
                ss << "," << " ";
            }
        }
        ss << ")";
        if (enter)
        {
            ss << "\n";
        }
        return ss.str();
    }

    template <typename TYPE>
    original::vector<TYPE>::~vector() {
        for (size_t i = 0; i < this->max_size; ++i) {
            if (this->body[i] != nullptr) {
                delete this->body[i];
            }
        }
        delete[] this->body;
    }

#endif //VECTOR_H
