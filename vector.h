#ifndef VECTOR_H
#define VECTOR_H

#include "serial.h"
#include "iterable.h"

namespace original{
    template <typename TYPE>
    class vector final : public serial<TYPE>, public iterable<TYPE>, public printable{
        class vectorNode final : public virtual wrapper<TYPE>{
            public:
                friend class iterator<TYPE>;
                friend class vector;
            private:
                TYPE data;
                vectorNode* prev;
                vectorNode* next;
            protected:
                explicit vectorNode(TYPE data, vectorNode* prev = nullptr, vectorNode* next = nullptr);
                vectorNode(const vectorNode& other);
                // todo
//                vectorNode& operator=(const vectorNode& other);
//                vectorNode(vectorNode&& other) _GLIBCXX_NOEXCEPT;
//                TYPE getVal() const override;
//                vectorNode* getPPrev() override;
//                vectorNode* getPNext() override;
//                void setPPrev(vectorNode* new_prev);
//                void setPNext(vectorNode* new_next);
//                static void connect(vectorNode* prev, vectorNode* next);
        };
    };
}

    template<typename TYPE>
    original::vector<TYPE>::vectorNode::vectorNode(
            TYPE data, original::vector<TYPE>::vectorNode* prev, original::vector<TYPE>::vectorNode* next)
            : wrapper<TYPE>(data), data(data), prev(prev), next(next) {}

    template<typename TYPE>
    original::vector<TYPE>::vectorNode::vectorNode(
            const original::vector<TYPE>::vectorNode &other)
            : wrapper<TYPE>(other.data), data(other.data), prev(other.prev), next(other.prev) {}

#endif //VECTOR_H
