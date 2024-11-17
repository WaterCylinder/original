#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "maths.h"

namespace original{

    template<typename TYPE>
    class transform{
        protected:
            virtual void apply(TYPE& t);
        public:
            virtual ~transform() = default;
            virtual transform* clone() const;
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

    template<typename TYPE>
    class absTransform final : public transform<TYPE>{
            void apply(TYPE &t) override;
        public:
            absTransform* clone() const override;
            explicit absTransform() = default;
    };

    template<typename TYPE>
    class copyTransform final : public transform<TYPE>{
            container<TYPE>* container_;
            void apply(TYPE &t) override;
        public:
            copyTransform* clone() const override;
            explicit copyTransform(container<TYPE>& container);
    };

} // namespace original

    template<typename TYPE>
    void original::transform<TYPE>::apply(TYPE&) {}

    template <typename TYPE>
    auto original::transform<TYPE>::clone() const -> transform*
    {
        return new transform(*this);
    }

    template<typename TYPE>
    auto original::transform<TYPE>::operator()(TYPE& t) -> void
    {
        this->apply(t);
    }

    template<typename TYPE>
    original::addTransform<TYPE>::addTransform(const TYPE &num) : num(num) {}

    template <typename TYPE>
    auto original::addTransform<TYPE>::clone() const -> addTransform*
    {
        return new addTransform(*this);
    }

    template<typename TYPE>
    auto original::addTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = t + this->num;
    }

    template<typename TYPE>
    original::multiplyTransform<TYPE>::multiplyTransform(const TYPE &num) : num(num) {}

    template <typename TYPE>
    auto original::multiplyTransform<TYPE>::clone() const -> multiplyTransform*
    {
        return new multiplyTransform(*this);
    }

    template<typename TYPE>
    auto original::multiplyTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = t * this->num;
    }

    template <typename TYPE>
    auto original::absTransform<TYPE>::clone() const -> absTransform*
    {
        return new absTransform(*this);
    }

    template<typename TYPE>
    auto original::absTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = original::abs(t);
    }

    template<typename TYPE>
    original::copyTransform<TYPE>::copyTransform(container<TYPE>& container)
        : container_(&container) {}

    template <typename TYPE>
    auto original::copyTransform<TYPE>::clone() const -> copyTransform*
    {
        return new copyTransform(*this);
    }

    template<typename TYPE>
    auto original::copyTransform<TYPE>::apply(TYPE& t) -> void
    {
        this->container_->add(t);
    }

#endif //TRANSFORM_H
