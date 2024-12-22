#ifndef CLONABLE_H
#define CLONABLE_H

namespace original {
    template<typename DERIVED>
    class baseCloneable {
        protected:
            baseCloneable() = default;
        public:
            [[nodiscard]] virtual DERIVED* clone() const;
            virtual ~baseCloneable() = default;
    };

    class cloneable : public baseCloneable<cloneable> {
        protected:
            cloneable() = default;
        public:
            ~cloneable() override = default;
    };
}

    template<typename DERIVED>
    auto original::baseCloneable<DERIVED>::clone() const -> DERIVED* {
        return new DERIVED(static_cast<const DERIVED&>(*this));
    }
#endif //CLONABLE_H
