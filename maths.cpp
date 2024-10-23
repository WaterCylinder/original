#include "maths.h"

namespace original {

    template<typename TYPE>
    TYPE original::abs(TYPE a) {
        return a > 0 ? a : -a;
    }

    template<typename TYPE>
    TYPE original::max(TYPE a, TYPE b) {
        return a > b ? a : b;
    }

    template<typename TYPE>
    TYPE original::min(TYPE a, TYPE b) {
        return a < b ? a : b;
    }

}