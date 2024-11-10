#ifndef COMPARATOR_H
#define COMPARATOR_H

namespace original
{
    template<typename TYPE>
    class comparator{
        public:
            virtual ~comparator() = default;
            virtual bool compare(const TYPE& t1, const TYPE& t2) const = 0;
            bool operator()(const TYPE& t1, const TYPE& t2) const;
    };

    template<typename TYPE>
    class increaseComparator final : public comparator<TYPE>{
        public:
            bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    template<typename TYPE>
    class decreaseComparator final : public comparator<TYPE>{
        public:
            bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    template<typename TYPE>
    class equalComparator final : public comparator<TYPE> {
        public:
            bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    template<typename TYPE>
    class notEqualComparator final : public comparator<TYPE> {
        public:
            bool compare(const TYPE& t1, const TYPE& t2) const override;
        };

    template<typename TYPE>
    class increaseNotStrictComparator final : public comparator<TYPE>{
        public:
            bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

    template<typename TYPE>
    class decreaseNotStrictComparator final : public comparator<TYPE>{
        public:
            bool compare(const TYPE& t1, const TYPE& t2) const override;
    };

}

    template <typename TYPE>
    auto original::comparator<TYPE>::operator()(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return this->compare(t1, t2);
    }

    template <typename TYPE>
    auto original::increaseComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 < t2;
    }

    template<typename TYPE>
    auto original::decreaseComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 > t2;
    }

    template <typename TYPE>
    auto original::equalComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 == t2;
    }

    template <typename TYPE>
    auto original::notEqualComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 != t2;
    }

template<typename TYPE>
    auto original::increaseNotStrictComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 < t2 || t1 == t2;
    }


    template <typename TYPE>
    auto original::decreaseNotStrictComparator<TYPE>::compare(const TYPE& t1, const TYPE& t2) const -> bool
    {
        return t1 > t2 || t1 == t2;
    }

#endif //COMPARATOR_H
