#ifndef TRANSFORM_H
#define TRANSFORM_H

namespace original{

    template<typename TYPE>
    class transform{
        protected:
            virtual void apply(TYPE& t);
        public:
            virtual ~transform() = default;
            virtual transform* clone() const = 0;
            explicit transform() = default;
            virtual void operator()(TYPE& t);
    };

    template<typename TYPE>
    class addTransform final : public transform<TYPE>{
            TYPE num;
            void apply(TYPE &t) override;
        public:
            addTransform* clone() const override;
            explicit addTransform(const TYPE& num);
    };

    template<typename TYPE>
    class multiplyTransform final : public transform<TYPE>{
            TYPE num;
            void apply(TYPE &t) override;
        public:
            multiplyTransform* clone() const override;
            explicit multiplyTransform(const TYPE& num);
        };

} // namespace original

    template<typename TYPE>
    void original::transform<TYPE>::apply(TYPE&) {}

    template<typename TYPE>
    void original::transform<TYPE>::operator()(TYPE &t) {
        this->apply(t);
    }

    template<typename TYPE>
    original::addTransform<TYPE>::addTransform(const TYPE &num) : num(num) {}

    template <typename TYPE>
    original::addTransform<TYPE>* original::addTransform<TYPE>::clone() const
    {
        return new addTransform(*this);
    }

    template<typename TYPE>
    void original::addTransform<TYPE>::apply(TYPE &t) {
        t = t + this->num;
    }

    template<typename TYPE>
    original::multiplyTransform<TYPE>::multiplyTransform(const TYPE &num) : num(num) {}

    template <typename TYPE>
    original::multiplyTransform<TYPE>* original::multiplyTransform<TYPE>::clone() const
    {
        return new multiplyTransform(*this);
    }

    template<typename TYPE>
    void original::multiplyTransform<TYPE>::apply(TYPE &t) {
        t = t * this->num;
    }

#endif //TRANSFORM_H
