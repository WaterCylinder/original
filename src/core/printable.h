#ifndef PRINTABLE_H
#define PRINTABLE_H
#pragma once

#include <memory>
#include "sstream"

/**
 * @file printable.h
 * @brief Interface for polymorphic string formatting and output.
 * @details Defines the printable base class with comprehensive string conversion
 *          utilities, supporting automatic formatting of primitive types, enums,
 *          pointers, and custom class hierarchies.
 */

namespace original {

/**
 * @class printable
 * @brief Base class providing polymorphic string conversion capabilities.
 * @details Inherit from this class to enable automatic string representation
 *          for output streams and C-style string access. Provides extensible
 *          formatting through template specializations.
 */
class printable {
    mutable std::string cache_string_; ///< Buffer for C-string conversions

public:
    virtual ~printable() = 0;

    /**
     * @brief Gets the class name for type identification.
     * @return Class name as string.
     * @details Override in derived classes to provide accurate type names.
     *
     * @code{.cpp}
     * class MyClass : public printable {
     *     std::string className() const override { return "MyClass"; }
     * };
     * @endcode
     */
    [[nodiscard]] virtual std::string className() const;

    /**
     * @brief Generates formatted string representation.
     * @param enter Whether to append newline.
     * @return Formatted string in "ClassName(data)" format.
     *
     * @code{.cpp}
     *
     * Myclass : public printable{};
     *
     * MyClass obj;
     * std::cout << obj.toString(true); // Outputs "printable(@0x7ffd) \n"
     * @endcode
     */
    [[nodiscard]] virtual std::string toString(bool enter) const;

    /**
     * @brief Explicit conversion to std::string.
     * @return String representation without newline.
     */
    explicit operator std::string() const;

    /**
     * @brief Explicit conversion to C-style string.
     * @return Null-terminated C-string (lifetime tied to object).
     */
    explicit operator const char*() const;

    /**
     * @brief Direct C-string access with formatting control.
     * @param enter Append newline if true.
     * @return Managed C-string buffer.
     */
    [[nodiscard]] const char* toCString(bool enter) const;

    // ----------------- Static Format Utilities -----------------

    /**
     * @brief Universal value-to-string conversion.
     * @tparam TYPE Input value type
     * @param t Value to format
     * @return Formatted string with type-specific handling
     * @details Automatically handles:
     *          - Enums (via formatEnum)
     *          - Strings (quoted)
     *          - Chars (quoted)
     *          - Pointers (hex address)
     *          - Specializations for nullptr_t, bool, etc.
     *
     * @code{.cpp}
     * printable::formatString(42);        // "42"
     * printable::formatString("hello");   // "\"hello\""
     * printable::formatString('A');       // "'A'"
     * @endcode
     */
    template<typename TYPE>
    static std::string formatString(const TYPE& t);

    /**
     * @brief Pointer-specific formatting.
     * @tparam TYPE Pointer type
     * @param ptr Pointer to format
     * @return String with hex address prefixed by @
     *
     * @code{.cpp}
     * int* p = nullptr;
     * printable::formatString(p); // "@0x0"
     * @endcode
     */
    template <typename TYPE>
    static std::string formatString(TYPE* const& ptr);

    /**
     * @brief C-string cache for temporary usage.
     * @tparam TYPE Input type
     * @param t Value to format
     * @return Static C-string buffer (thread-unsafe)
     *
     * @warning Not thread-safe due to static buffer
     */
    template<typename TYPE>
    static const char* formatCString(const TYPE& t);

    /**
     * @brief Enum formatting utility.
     * @tparam TYPE Enum type
     * @param t Enum value
     * @return String in "EnumName(value)" format
     *
     * @code{.cpp}
     * enum Color { Red };
     * printable::formatEnum(Color::Red); // "Color(0)"
     * @endcode
     */
    template<typename TYPE>
    static std::string formatEnum(const TYPE& t);

    friend std::ostream& operator<<(std::ostream& os, const printable& p);
};

/**
 * @brief Stream insertion operator for printable objects.
 * @param os Output stream
 * @param p Printable object
 * @return Modified output stream
 *
 * @code{.cpp}
 * Myclass : public printable{};
 *
 * MyClass obj;
 * std::cout << obj; // Outputs "printable(@0x7ffd) \n"
 * @endcode
 */
std::ostream& operator<<(std::ostream& os, const printable& p);

} // namespace original

// ----------------- Definitions of printable.h -----------------

inline original::printable::~printable() = default;

inline auto original::printable::className() const -> std::string
{
    return "printable";
}

inline auto original::printable::toString(const bool enter) const -> std::string
{
    std::stringstream ss;
    ss << this->className() << "(" << formatString(this) << ")";
    if (enter) ss << "\n";
    return ss.str();
}

inline original::printable::operator std::string() const {
    return this->toString(false);
}

inline original::printable::operator const char*() const {
    return this->toCString(false);
}

inline auto original::printable::toCString(const bool enter) const -> const char*
{
    this->cache_string_ = this->toString(enter);
    return this->cache_string_.c_str();
}

template<typename TYPE>
auto original::printable::formatString(const TYPE& t) -> std::string
{
    if constexpr (std::is_enum_v<TYPE>) {
        return formatEnum(t);
    } else {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
}

template <typename TYPE>
auto original::printable::formatCString(const TYPE& t) -> const char*
{
    static auto result =
            std::make_unique<std::string>(formatString<TYPE>(t));
    return result->c_str();
}

template <typename TYPE>
auto original::printable::formatEnum(const TYPE& t) -> std::string
{
    const std::string enumName = typeid(t).name();
    const int enumValue = static_cast<std::underlying_type_t<TYPE>>(t);
    return enumName + "(" + std::to_string(enumValue) + ")";
}

template<>
inline auto original::printable::formatString<std::nullptr_t>(const std::nullptr_t&) -> std::string
{
    return "nullptr";
}

template<>
inline auto original::printable::formatString<std::string>(const std::string& t) -> std::string
{
    return "\"" + t + "\"";
}

template<>
inline auto original::printable::formatString<char>(const char& t) -> std::string
{
    return "'" + std::string(1, t) + "'";
}

template<>
inline auto original::printable::formatString<bool>(const bool& t) -> std::string
{
    return t != 0 ? "true" : "false";
}

template <typename TYPE>
auto original::printable::formatString(TYPE* const& ptr) -> std::string
{
    std::stringstream ss;
    ss << "@" << ptr;
    return ss.str();
}

template<>
inline auto original::printable::formatString<const char>(const char* const &ptr) -> std::string {
    return formatString<std::string>(ptr);
}

inline std::ostream& original::operator<<(std::ostream& os, const printable& p){
    os << p.toString(false);
    return os;
}

#endif // PRINTABLE_H