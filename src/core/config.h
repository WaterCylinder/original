#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Platform-independent integer and floating-point type definitions
 * @details Provides unified integer and floating-point type aliases for cross-platform consistency.
 * Ensures fixed-width integer and standardized floating-point usage throughout the codebase.
 */

#include "cstdint"

namespace original {

    /**
     * @brief Unsigned 8-bit integer type (byte)
     * @details Typically used for raw byte manipulation and binary data handling.
     * @note Range: 0 to 255
     */
    using byte = std::uint8_t;

    /**
     * @brief Signed 8-bit integer type
     * @details Used for small signed numeric values.
     * @note Range: -128 to 127
     */
    using s_byte = std::int8_t;

    /**
     * @brief 64-bit signed integer type for arithmetic operations
     * @details Primary type for most arithmetic operations where large range is needed.
     * @note Range: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
     * @note Use this for calculations that may require large numbers
     */
    using integer = std::int64_t;

    /**
     * @brief 32-bit unsigned integer type for sizes and indexes
     * @details Used for array indexing, sizes, and counts where negative values are not needed.
     * @note Range: 0 to 4,294,967,295
     * @warning Not suitable for very large containers (>4GB)
     */
    using u_integer = std::uint32_t;

    using ul_integer = std::uint64_t;

    /**
     * @brief Double-precision floating-point type
     * @details Standard floating-point type for most numerical calculations.
     * @note Typically 64-bit IEEE 754 floating-point (15-17 decimal digits precision)
     * @note Use this for most floating-point operations requiring precision
     */
    using floating = double;

    /**
     * @brief Extended precision floating-point type
     * @details Highest precision floating-point type for critical numerical calculations.
     * @note Typically 80-bit or 128-bit extended precision (18-21 decimal digits precision)
     * @note Use this when maximum floating-point precision is required
     */
    using l_floating = long double;
}

#endif //CONFIG_H