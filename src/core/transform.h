#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "cloneable.h"

#include "maths.h"
#include "container.h"

namespace original{

    template<typename TYPE>
    class transform : public cloneable {
        protected:
            virtual void apply(TYPE& t);
        public:
            ~transform() override = default;
            transform* clone() const override;
            explicit transform() = default;
            virtual void operator()(TYPE& t);
    };

    template<typename TYPE>
    class addOptTransform final : public transform<TYPE>{
            TYPE t_;
            void apply(TYPE &t) override;
        public:
            addOptTransform* clone() const override;
            explicit addOptTransform(const TYPE& t);
    };

    template<typename TYPE>
    class assignOptTransform final : public transform<TYPE>{
            TYPE val_;
            void apply(TYPE &t) override;
        public:
            assignOptTransform* clone() const override;
            explicit assignOptTransform(const TYPE& val);
    };

    template<typename TYPE>
    class multiOptTransform final : public transform<TYPE>{
            TYPE t_;
            void apply(TYPE &t) override;
        public:
            multiOptTransform* clone() const override;
            explicit multiOptTransform(const TYPE& t);
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
    original::assignOptTransform<TYPE>::assignOptTransform(const TYPE& val)
        : val_(val) {}

    template <typename TYPE>
    auto original::assignOptTransform<TYPE>::clone() const -> assignOptTransform*
    {
        return new assignOptTransform(*this);
    }

    template<typename TYPE>
    auto original::assignOptTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = this->val_;
    }

    template<typename TYPE>
    original::addOptTransform<TYPE>::addOptTransform(const TYPE &t) : t_(t) {}

    template <typename TYPE>
    auto original::addOptTransform<TYPE>::clone() const -> addOptTransform*
    {
        return new addOptTransform(*this);
    }

    template<typename TYPE>
    auto original::addOptTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = t + this->t_;
    }

    template<typename TYPE>
    original::multiOptTransform<TYPE>::multiOptTransform(const TYPE &t) : t_(t) {}

    template <typename TYPE>
    auto original::multiOptTransform<TYPE>::clone() const -> multiOptTransform*
    {
        return new multiOptTransform(*this);
    }

    template<typename TYPE>
    auto original::multiOptTransform<TYPE>::apply(TYPE& t) -> void
    {
        t = t * this->t_;
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
