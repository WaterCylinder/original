#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "allocator.h"
#include "comparator.h"
#include "wrapper.h"


namespace original {
    // TODO
    template<typename TYPE, typename COMP = increaseComparator<TYPE>, typename ALLOC = allocator<TYPE>>
    class hashTable {

        class hashNode final : public wrapper<TYPE> {
        private:
            TYPE data;
            hashNode* next;
        public:
            friend class hashTable;

            explicit hashNode(const TYPE& data = TYPE{}, hashNode* next = nullptr);

            hashNode(const hashNode& other);

            hashNode& operator=(const hashNode& other) const;

            TYPE& getVal() override;

            const TYPE& getVal() const override;

            void setVal(TYPE data) override;

            hashNode* getPNext() const override;

            void setPNext(hashNode* new_next);

            static void connect(hashNode* prev, hashNode* next);
        };
    };
}

template<typename TYPE, typename COMP, typename ALLOC>
original::hashTable<TYPE, COMP, ALLOC>::hashNode::hashNode(const TYPE &data, hashNode* next)
    : data(data), next(next) {}

#endif //HASHTABLE_H
