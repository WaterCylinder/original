#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once


namespace original {

    template <typename TYPE>
    class container{
    public:
        [[nodiscard]] virtual uint32_t size() const = 0;
        [[nodiscard]] bool empty() const;
        virtual bool contains(const TYPE& e) const = 0 ;
        virtual void add(const TYPE& e) = 0;
        virtual void clear() = 0;
        virtual ~container() = default;
    };
}

    template <typename TYPE>
    bool original::container<TYPE>::empty() const {
        return this->size() == 0;
    }

#endif //CONTAINER_H