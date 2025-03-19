#ifndef DELETER_H
#define DELETER_H

namespace original {
    template<typename TYPE>
    class deleter{
    public:
        constexpr deleter() noexcept = default;
        virtual ~deleter() = default;

        void operator()(TYPE* ptr) const noexcept;
    };

    template<typename TYPE>
    class deleter<TYPE[]>{
    public:
        constexpr deleter() noexcept = default;
        virtual ~deleter() = default;

        void operator()(TYPE* ptr) const noexcept;
    };
}

template<typename TYPE>
void original::deleter<TYPE>::operator()(TYPE* ptr) const noexcept {
    delete ptr;
}

template<typename TYPE>
void original::deleter<TYPE[]>::operator()(TYPE* ptr) const noexcept {
    delete[] ptr;
}

#endif //DELETER_H
