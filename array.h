#ifndef ARRAY_H
#define ARRAY_H
#include <initializer_list>
#include <string>

namespace original{

    template<typename TYPE>
    class array{
        size_t size_;
        TYPE* body_;

        bool indexOutOfBound(int index);

        public:
        explicit array(int size);
        array(std::initializer_list<TYPE> lst);
        array(const array& other);
        auto operator=(const array& other) -> array&;
        array(array&& other) noexcept;

        ~array();

        [[nodiscard]] size_t size() const;
        TYPE get(int index);
        void set(int index, TYPE e);
        std::string toString(bool enter = false);
    };

}

#endif //ARRAY_H
