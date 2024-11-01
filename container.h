#ifndef CONTAINER_H
#define CONTAINER_H
#pragma once

#include <string>
#include "config.h"

namespace original {

    class container{
    public:
        _GLIBCXX_NODISCARD virtual size_t size() const = 0;
        virtual ~container() = default;
    };

}

#endif //CONTAINER_H