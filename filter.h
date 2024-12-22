#ifndef FILTER_H
#define FILTER_H
#include <cloneable.h>

namespace original {

    template<typename TYPE>
    class filter : public cloneable {
    protected:
        virtual bool match(const TYPE& t) const;
    public:
        ~filter() override = default;
        filter* clone() const override;
        explicit filter() = default;
        bool operator()(const TYPE& t) const;
    };

    template<typename TYPE>
    class equalFilter final : public filter<TYPE> {
        TYPE target;
        bool match(const TYPE& t) const override;
    public:
        explicit equalFilter(const TYPE& target);
        equalFilter* clone() const override;
    };

    template<typename TYPE>
    class notEqualFilter final : public filter<TYPE> {
        TYPE target;
        bool match(const TYPE& t) const override;
    public:
        explicit notEqualFilter(const TYPE& target);
        notEqualFilter* clone() const override;
    };

    template<typename TYPE>
    class lessFilter final : public filter<TYPE> {
        TYPE low;
        bool match(const TYPE& t) const override;
    public:
        explicit lessFilter(const TYPE& low);
        lessFilter* clone() const override;
    };

    template<typename TYPE>
    class greaterFilter final : public filter<TYPE> {
        TYPE high;
        bool match(const TYPE& t) const override;
    public:
        explicit greaterFilter(const TYPE& high);
        greaterFilter* clone() const override;
    };

    template<typename TYPE>
    class notLessFilter final : public filter<TYPE> {
        TYPE high;
        bool match(const TYPE& t) const override;
    public:
        explicit notLessFilter(const TYPE& high);
        notLessFilter* clone() const override;
    };

    template<typename TYPE>
    class notGreaterFilter final : public filter<TYPE> {
        TYPE low;
        bool match(const TYPE& t) const override;
    public:
        explicit notGreaterFilter(const TYPE& low);
        notGreaterFilter* clone() const override;
    };

    template<typename TYPE>
    class rangeFilter final : public filter<TYPE> {
        TYPE low, high;
        bool match(const TYPE& t) const override;
    public:
        explicit rangeFilter(const TYPE& low, const TYPE& high);
        rangeFilter* clone() const override;
    };

} // namespace original

    template<typename TYPE>
    bool original::filter<TYPE>::match(const TYPE&) const {
        return true;
    }

    template <typename TYPE>
    auto original::filter<TYPE>::clone() const -> filter*
    {
        return new filter(*this);
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

    template <typename TYPE>
    auto original::equalFilter<TYPE>::clone() const -> equalFilter*
    {
        return new equalFilter(*this);
    }

    template<typename TYPE>
    bool original::notEqualFilter<TYPE>::match(const TYPE& t) const {
        return t != target;
    }

    template<typename TYPE>
    original::notEqualFilter<TYPE>::notEqualFilter(const TYPE& target)
            : target(target) {}

    template <typename TYPE>
    auto original::notEqualFilter<TYPE>::clone() const -> notEqualFilter*
    {
        return new notEqualFilter(*this);
    }

    template<typename TYPE>
    bool original::lessFilter<TYPE>::match(const TYPE& t) const {
        return t < low;
    }

    template<typename TYPE>
    original::lessFilter<TYPE>::lessFilter(const TYPE& low)
            : low(low) {}

    template <typename TYPE>
    auto original::lessFilter<TYPE>::clone() const -> lessFilter*
    {
        return new lessFilter(*this);
    }

    template<typename TYPE>
    bool original::greaterFilter<TYPE>::match(const TYPE& t) const {
        return t > high;
    }

    template<typename TYPE>
    original::greaterFilter<TYPE>::greaterFilter(const TYPE& high)
            : high(high) {}

    template <typename TYPE>
    auto original::greaterFilter<TYPE>::clone() const -> greaterFilter*
    {
        return new greaterFilter(*this);
    }

    template<typename TYPE>
    bool original::notLessFilter<TYPE>::match(const TYPE& t) const {
        return t >= high;
    }

    template<typename TYPE>
    original::notLessFilter<TYPE>::notLessFilter(const TYPE& high)
            : high(high) {}

    template <typename TYPE>
    auto original::notLessFilter<TYPE>::clone() const -> notLessFilter*
    {
        return new notLessFilter(*this);
    }

    template<typename TYPE>
    bool original::notGreaterFilter<TYPE>::match(const TYPE& t) const {
        return t <= low;
    }

    template<typename TYPE>
    original::notGreaterFilter<TYPE>::notGreaterFilter(const TYPE& low)
            : low(low) {}

    template <typename TYPE>
    auto original::notGreaterFilter<TYPE>::clone() const -> notGreaterFilter*
    {
        return new notGreaterFilter(*this);
    }

    template<typename TYPE>
    bool original::rangeFilter<TYPE>::match(const TYPE& t) const {
        return t >= low && t <= high;
    }

    template<typename TYPE>
    original::rangeFilter<TYPE>::rangeFilter(const TYPE& low, const TYPE& high)
            : low(low), high(high) {}

    template <typename TYPE>
    auto original::rangeFilter<TYPE>::clone() const -> rangeFilter*
    {
        return new rangeFilter(*this);
    }

#endif // FILTER_H
