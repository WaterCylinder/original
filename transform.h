#ifndef TRANSFORM_H
#define TRANSFORM_H

namespace original{

    template<typename TYPE>
    class transform{
        protected:
            virtual void apply(TYPE& t);
        public:
            virtual ~transform() = default;
            explicit transform() = default;
            virtual void operator()(TYPE& t);
    };

    template<typename TYPE>
    class addTransform final : public transform<TYPE>{
            TYPE num;
            void apply(TYPE &t) override;
        public:
            explicit addTransform(const TYPE& num);
    };

} // namespace original

    template<typename TYPE>
    void original::transform<TYPE>::apply(TYPE&) {}

    template<typename TYPE>
    void original::transform<TYPE>::operator()(TYPE &t) {
        std::cout << "Base apply called\n"; // Debug line
        this->apply(t);
    }

    template<typename TYPE>
    original::addTransform<TYPE>::addTransform(const TYPE &num) : num(num) {}

    template<typename TYPE>
    void original::addTransform<TYPE>::apply(TYPE &t) {
        std::cout << "Derived apply called\n"; // Debug line
        t = t + this->num;
    }

#endif //TRANSFORM_H
