#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <functional>
#include <iostream>

namespace original {

    template<typename TYPE>
    class filter {
    protected:
        virtual bool match(const TYPE& t) const = 0;
    public:
        explicit filter() = default;
        bool operator()(const TYPE& t) const;
    };

    template<typename TYPE>
    class equalFilter final : public filter<TYPE> {
        TYPE threshold;
        bool match(const TYPE& t) const override;
    public:
        explicit equalFilter(const TYPE& threshold);
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
    bool original::filter<TYPE>::operator()(const TYPE& t) const {
        return this->match(t);
    }

    template<typename TYPE>
    bool original::equalFilter<TYPE>::match(const TYPE& t) const {
        return t == threshold;
    }

    template<typename TYPE>
    original::equalFilter<TYPE>::equalFilter(const TYPE& threshold)
            : threshold(threshold) {}

    template<typename TYPE>
    bool original::rangeFilter<TYPE>::match(const TYPE& t) const {
        return t >= low && t <= high;
    }

    template<typename TYPE>
    original::rangeFilter<TYPE>::rangeFilter(const TYPE& low, const TYPE& high)
            : low(low), high(high) {}


#endif // FILTER_H
