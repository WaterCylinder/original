#ifndef PRINTABLE_H
#define PRINTABLE_H
#pragma once

#include <memory>
#include "sstream"

namespace original {
    class printable {
    public:
        virtual ~printable() = 0;
        [[__nodiscard__]] virtual std::string className() const;
        [[__nodiscard__]] virtual std::string toString(bool enter) const;
        [[__nodiscard__]] const char* toCString(bool enter) const;

        template<typename TYPE>
        static std::string formatString(const TYPE& t);
        template <typename TYPE>
        static std::string formatString(TYPE* const& ptr);
        template<typename TYPE>
        static const char* formatCString(const TYPE& t);
        template<typename TYPE>
        static std::string formatEnum(const TYPE& t);

        friend std::ostream& operator<<(std::ostream& os, const printable& p);
    };

    std::ostream& operator<<(std::ostream& os, const printable& p);
}

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

    inline auto original::printable::toCString(const bool enter) const -> const char*
    {
        static auto result =
            std::make_unique<std::string>(this->toString(enter));
        return result->c_str();
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
        if (ptr == nullptr) {
            return "nullptr";
        }
        std::stringstream ss;
        ss << "#" << ptr;
        return ss.str();
    }

    inline std::ostream& original::operator<<(std::ostream& os, const printable& p){
        os << p.toString(false);
        return os;
    }

#endif // PRINTABLE_H