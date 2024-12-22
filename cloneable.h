#ifndef CLONABLE_H
#define CLONABLE_H

namespace original {
    class cloneable {
        protected:
            cloneable() = default;
        public:
            [[nodiscard]] virtual cloneable* clone() const;
            virtual ~cloneable() = default;
    };
}

    inline auto original::cloneable::clone() const -> cloneable* {
        return new cloneable(*this);
    }

#endif //CLONABLE_H
