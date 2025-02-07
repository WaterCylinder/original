#ifndef COMPARABLE_H
#define COMPARABLE_H

namespace original {
    template <typename DERIVED>
    class comparable {
        public:
            virtual int64_t compareTo(const DERIVED &other) const = 0;
            bool operator==(const DERIVED &other) const;
            bool operator!=(const DERIVED &other) const;
            bool operator<(const DERIVED &other) const;
            bool operator>(const DERIVED &other) const;
            bool operator<=(const DERIVED &other) const;
            bool operator>=(const DERIVED &other) const;
            virtual ~comparable() = default;
    };

    template<typename DERIVED>
    auto comparable<DERIVED>::operator==(const DERIVED &other) const -> bool {
        return compareTo(other) == 0;
    }

    template<typename DERIVED>
    auto comparable<DERIVED>::operator!=(const DERIVED &other) const -> bool {
        return compareTo(other) != 0;
    }

    template<typename DERIVED>
    auto comparable<DERIVED>::operator<(const DERIVED &other) const -> bool {
        return compareTo(other) < 0;
    }

    template<typename DERIVED>
    auto comparable<DERIVED>::operator>(const DERIVED &other) const -> bool {
        return compareTo(other) > 0;
    }

    template<typename DERIVED>
    auto comparable<DERIVED>::operator<=(const DERIVED &other) const -> bool {
        return compareTo(other) <= 0;
    }

    template<typename DERIVED>
    auto comparable<DERIVED>::operator>=(const DERIVED &other) const -> bool {
        return compareTo(other) >= 0;
    }
}

#endif //COMPARABLE_H
