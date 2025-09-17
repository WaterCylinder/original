#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Platform-independent type definitions and compiler/platform detection
 * @details Provides unified integer and floating-point type aliases for cross-platform consistency.
 * Includes comprehensive platform and compiler detection macros and consteval functions.
 * Ensures fixed-width integer and standardized floating-point usage throughout the codebase.
 */

#include <cstdint>

/**
 * @defgroup PlatformDetection Platform Detection Macros
 * @brief Macros for identifying the target platform
 * @{
 */

#if defined(_WIN32) || defined(_WIN64)
#define ORIGINAL_PLATFORM_WINDOWS 1
#ifdef _WIN64
#define ORIGINAL_PLATFORM_WINDOWS_64 1
#elif
#define ORIGINAL_PLATFORM_WINDOWS_32 1
#else
    #define ORIGINAL_PLATFORM_WINDOWS 0
#endif
#elif defined(__linux__)
#define ORIGINAL_PLATFORM_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define ORIGINAL_PLATFORM_MACOS 1
#elif defined(__unix__)
    #define ORIGINAL_PLATFORM_UNIX 1
#else
    #define ORIGINAL_PLATFORM_UNKNOWN 1
#endif
/** @} */ // end of PlatformDetection group

/**
 * @defgroup CompilerDetection Compiler Detection Macros
 * @brief Macros for identifying the compiler being used
 * @{
 */
#if defined(__clang__)
#define ORIGINAL_COMPILER_CLANG 1
    #define ORIGINAL_COMPILER_VERSION __clang_major__.__clang_minor__.__clang_patchlevel__
#elif defined(__GNUC__) || defined(__GNUG__)
#define ORIGINAL_COMPILER_GCC 1
#define ORIGINAL_COMPILER_VERSION __GNUC__.__GNUC_MINOR__.__GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
#define ORIGINAL_COMPILER_MSVC 1
    #define ORIGINAL_COMPILER_VERSION _MSC_VER
#else
    #define ORIGINAL_COMPILER_UNKNOWN 1
#endif
/** @} */ // end of CompilerDetection group

/**
 * @namespace original
 * @brief Main namespace for the project Original
 * @details This namespace serves as the main container for all modules and
 * functionality within the Original project. It includes various classes, functions,
 * and data structures that form the backbone of the system.
 */
namespace original {

    /**
     * @defgroup PlatformDetectionFunctions Platform Detection Functions
     * @brief Compile-time functions for platform detection
     * @{
     */

    /**
     * @brief Checks if compiling for Windows platform
     * @return true if compiling for Windows, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_WINDOWS(){
        #if ORIGINAL_PLATFORM_WINDOWS
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling for 32-bit Windows
     * @return true if compiling for Win32, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_WIN32(){
        #if ORIGINAL_PLATFORM_WINDOWS_32
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling for 64-bit Windows
     * @return true if compiling for Win64, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_WIN64(){
        #if ORIGINAL_PLATFORM_WINDOWS_64
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling for Linux platform
     * @return true if compiling for Linux, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_LINUX(){
        #if ORIGINAL_PLATFORM_LINUX
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling for macOS platform
     * @return true if compiling for macOS, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_MACOS(){
        #if ORIGINAL_PLATFORM_MACOS
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling for Unix-like platform (excluding Linux/macOS)
     * @return true if compiling for Unix, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_UNIX(){
        #if ORIGINAL_PLATFORM_UNIX
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling for unknown platform
     * @return true if platform detection failed, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool ON_UNKNOWN_PLATFORM(){
        #if ORIGINAL_PLATFORM_UNKNOWN
            return true;
        #else
            return false;
        #endif
    }
    /** @} */ // end of PlatformDetectionFunctions group

    /**
     * @defgroup CompilerDetectionFunctions Compiler Detection Functions
     * @brief Compile-time functions for compiler detection
     * @{
     */

    /**
     * @brief Checks if compiling with Clang
     * @return true if using Clang compiler, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool USING_CLANG(){
        #if ORIGINAL_COMPILER_CLANG
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling with GCC
     * @return true if using GCC compiler, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool USING_GCC(){
        #if ORIGINAL_COMPILER_GCC
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling with MSVC
     * @return true if using Microsoft Visual C++ compiler, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool USING_MSVC(){
        #if ORIGINAL_COMPILER_MSVC
            return true;
        #else
            return false;
        #endif
    }

    /**
     * @brief Checks if compiling with unknown compiler
     * @return true if compiler detection failed, false otherwise
     * @note Evaluated at compile-time
     */
    consteval bool USING_UNKNOWN_COMPLIER(){
        #if ORIGINAL_COMPILER_UNKNOWN
            return true;
        #else
            return false;
        #endif
    }
    /** @} */ // end of CompilerDetectionFunctions group

    /**
     * @defgroup TypeDefinitions Type Definitions
     * @brief Platform-independent type aliases
     * @{
     */

    /**
     * @brief Unsigned 8-bit integer type (byte)
     * @details Typically used for raw byte manipulation and binary data handling.
     * @note Range: 0 to 255
     * @note Equivalent to std::uint8_t
     */
    using byte = std::uint8_t;

    /**
     * @brief Signed 8-bit integer type
     * @details Used for small signed numeric values.
     * @note Range: -128 to 127
     * @note Equivalent to std::int8_t
     */
    using s_byte = std::int8_t;

    /**
     * @brief 64-bit signed integer type for arithmetic operations
     * @details Primary type for most arithmetic operations where large range is needed.
     * @note Range: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
     * @note Use this for calculations that may require large numbers
     * @note Equivalent to std::int64_t
     */
    using integer = std::int64_t;

    /**
     * @brief 32-bit unsigned integer type for sizes and indexes
     * @details Used for array indexing, sizes, and counts where negative values are not needed.
     * @note Range: 0 to 4,294,967,295
     * @warning Not suitable for very large containers (>4GB)
     * @note Equivalent to std::uint32_t
     */
    using u_integer = std::uint32_t;

    /**
     * @brief 64-bit unsigned integer type
     * @details Large unsigned integer type for big sizes and counters.
     * Guaranteed to be exactly 64 bits wide across all platforms.
     * @note Range: 0 to 18,446,744,073,709,551,615
     * @note Preferred for: Large containers, filesystem sizes, big counters
     * @see u_integer For smaller unsigned needs
     * @note Equivalent to std::uint64_t
     */
    using ul_integer = std::uint64_t;

    /**
     * @brief Double-precision floating-point type
     * @details Standard floating-point type for most numerical calculations.
     * @note Typically 64-bit IEEE 754 floating-point (15-17 decimal digits' precision)
     * @note Use this for most floating-point operations requiring precision
     * @note Equivalent to double
     */
    using floating = double;

    /**
     * @brief Extended precision floating-point type
     * @details Highest precision floating-point type for critical numerical calculations.
     * @note Typically 80-bit or 128-bit extended precision (18-21 decimal digits' precision)
     * @note Use this when maximum floating-point precision is required
     * @note Equivalent to long double
     */
    using l_floating = long double;
    /** @} */ // end of TypeDefinitions group
}

#endif //CONFIG_H