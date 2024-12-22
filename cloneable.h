#ifndef CLONABLE_H
#define CLONABLE_H

namespace original {
    class cloneable {
        public:
        [[nodiscard]] virtual cloneable* clone() const = 0;
        virtual ~cloneable() = default;
    };
}

#endif //CLONABLE_H
