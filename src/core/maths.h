#ifndef MATHS_H
#define MATHS_H
#include "error.h"

namespace original{

    constexpr long double E = 2.7182818284590452353602874713526624977572470937000L;
    constexpr long double PI = 3.1415926535897932384626433832795028841971693993751L;

    template<typename TYPE>
    TYPE abs(TYPE a);

    template<typename TYPE>
    TYPE max(TYPE a, TYPE b);

    template<typename TYPE>
    TYPE min(TYPE a, TYPE b);

    double pow(double base, int exp);
}

    template<typename TYPE>
    auto original::abs(TYPE a) -> TYPE
    {
        return a > TYPE{} ? a : -a;
    }

    template<typename TYPE>
    auto original::max(TYPE a, TYPE b) -> TYPE
    {
        return a > b ? a : b;
    }

    template<typename TYPE>
    auto original::min(TYPE a, TYPE b) -> TYPE
    {
        return a < b ? a : b;
    }

    inline auto original::pow(const double base, const int exp) -> double
    {
        if (base == 0 && exp <= 0)
        {
            throw valueError();
        }
        double res = 1.0;
        for (int i = 0; i < abs(exp); i += 1)
        {
            res *= base;
        }
        return exp >= 0 ? res : 1 / res;
    }

#endif //MATHS_H
