#ifndef TRANSFORM_H
#define TRANSFORM_H

namespace original{

    template<typename TYPE>
    class transform{
        protected:
            virtual void apply(TYPE& t) const;
        public:
            explicit transform() = default;
            void operator()(TYPE& t) const;
    };

    template<typename TYPE>
    class addTransform final : public transform<TYPE>{
            TYPE num;
            void apply(TYPE &t) const override;
        public:
            explicit addTransform(const TYPE& num);
    };

} // namespace original

    template<typename TYPE>
    void original::transform<TYPE>::apply(TYPE&) const {}

    template<typename TYPE>
    void original::transform<TYPE>::operator()(TYPE &t) const {
        this->apply(t);
    }

    template<typename TYPE>
    original::addTransform<TYPE>::addTransform(const TYPE &num) : num(num) {}

    template<typename TYPE>
    void original::addTransform<TYPE>::apply(TYPE &t) const {
        t = t + this->num;
    }

#endif //TRANSFORM_H
