#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Platform-independent integer type definitions
 * @details Provides unified integer type aliases for cross-platform consistency.
 * Ensures fixed-width integer usage throughout the codebase.
 */

#include "cstdint"

namespace original {

    using integer = std::int64_t;  ///< 64-bit signed integer type for arithmetic operations

    using u_integer = std::uint32_t; ///< 32-bit unsigned integer type for sizes/indexes
}

#endif //CONFIG_H