#ifndef FILTER_H
#define FILTER_H

namespace original {

    template<typename TYPE>
    class filter {
    protected:
        virtual bool match(const TYPE& t) const;
    public:
        explicit filter() = default;
        bool operator()(const TYPE& t) const;
    };

    template<typename TYPE>
    class equalFilter final : public filter<TYPE> {
        TYPE target;
        bool match(const TYPE& t) const override;
    public:
        explicit equalFilter(const TYPE& target);
    };

    template<typename TYPE>
    class notEqualFilter final : public filter<TYPE> {
        TYPE target;
        bool match(const TYPE& t) const override;
    public:
        explicit notEqualFilter(const TYPE& target);
    };

    template<typename TYPE>
    class lessFilter final : public filter<TYPE> {
        TYPE low;
        bool match(const TYPE& t) const override;
    public:
        explicit lessFilter(const TYPE& low);
    };

    template<typename TYPE>
    class greaterFilter final : public filter<TYPE> {
        TYPE high;
        bool match(const TYPE& t) const override;
    public:
        explicit greaterFilter(const TYPE& high);
    };

    template<typename TYPE>
    class notLessFilter final : public filter<TYPE> {
        TYPE high;
        bool match(const TYPE& t) const override;
    public:
        explicit notLessFilter(const TYPE& high);
    };

    template<typename TYPE>
    class notGreaterFilter final : public filter<TYPE> {
        TYPE low;
        bool match(const TYPE& t) const override;
    public:
        explicit notGreaterFilter(const TYPE& low);
    };

    template<typename TYPE>
    class rangeFilter final : public filter<TYPE> {
        TYPE low, high;
        bool match(const TYPE& t) const override;
    public:
        explicit rangeFilter(const TYPE& low, const TYPE& high);
    };

} // namespace original

    template<typename TYPE>
    bool original::filter<TYPE>::match(const TYPE&) const {
        return true;
    }

    template<typename TYPE>
    bool original::filter<TYPE>::operator()(const TYPE& t) const {
        return this->match(t);
    }

    template<typename TYPE>
    bool original::equalFilter<TYPE>::match(const TYPE& t) const {
        return t == target;
    }

    template<typename TYPE>
    original::equalFilter<TYPE>::equalFilter(const TYPE& target)
            : target(target) {}

    template<typename TYPE>
    bool original::notEqualFilter<TYPE>::match(const TYPE& t) const {
        return t != target;
    }

    template<typename TYPE>
    original::notEqualFilter<TYPE>::notEqualFilter(const TYPE& target)
            : target(target) {}

    template<typename TYPE>
    bool original::lessFilter<TYPE>::match(const TYPE& t) const {
        return t < low;
    }

    template<typename TYPE>
    original::lessFilter<TYPE>::lessFilter(const TYPE& low)
            : low(low) {}

    template<typename TYPE>
    bool original::greaterFilter<TYPE>::match(const TYPE& t) const {
        return t > high;
    }

    template<typename TYPE>
    original::greaterFilter<TYPE>::greaterFilter(const TYPE& high)
            : high(high) {}

    template<typename TYPE>
    bool original::notLessFilter<TYPE>::match(const TYPE& t) const {
        return t >= high;
    }

    template<typename TYPE>
    original::notLessFilter<TYPE>::notLessFilter(const TYPE& high)
            : high(high) {}

    template<typename TYPE>
    bool original::notGreaterFilter<TYPE>::match(const TYPE& t) const {
        return t <= low;
    }

    template<typename TYPE>
    original::notGreaterFilter<TYPE>::notGreaterFilter(const TYPE& low)
            : low(low) {}

    template<typename TYPE>
    bool original::rangeFilter<TYPE>::match(const TYPE& t) const {
        return t >= low && t <= high;
    }

    template<typename TYPE>
    original::rangeFilter<TYPE>::rangeFilter(const TYPE& low, const TYPE& high)
            : low(low), high(high) {}


#endif // FILTER_H
