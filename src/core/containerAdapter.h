#ifndef CONTAINERADAPTER_H
#define CONTAINERADAPTER_H
#include "baseList.h"
#include "types.h"
#include "printable.h"

namespace original {
    template <typename TYPE, template <typename> typename SERIAL>
    requires ExtendsOf<baseList<TYPE>, SERIAL<TYPE>>
    class containerAdapter : public printable, public container<TYPE>{
    protected:
        SERIAL<TYPE> serial_;

        explicit containerAdapter(const SERIAL<TYPE>& serial);
    public:
        [[nodiscard]] uint32_t size() const override;
        void clear();
        bool contains(const TYPE &e) const override;
        [[nodiscard]] std::string className() const override;
        [[nodiscard]] std::string toString(bool enter) const override;
        ~containerAdapter() override = default;
    };
}

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    original::containerAdapter<TYPE, SERIAL>::containerAdapter(const SERIAL<TYPE>& serial)
        : serial_(serial) {}

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::size() const -> uint32_t {
        return serial_.size();
    }

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, template <typename> class SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::contains(const TYPE &e) const -> bool {
        return serial_.contains(e);
    }

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::className() const -> std::string {
        return "containerAdapter";
    }

    template<typename TYPE, template <typename> typename SERIAL>
    requires original::ExtendsOf<original::baseList<TYPE>, SERIAL<TYPE>>
    auto original::containerAdapter<TYPE, SERIAL>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << this->className() << "(";
        bool first = true;
        for (const auto e : this->serial_)
        {
            if (!first) ss << ", ";
            ss << printable::formatString(e);
            first = false;
        }
        ss << ")";
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //CONTAINERADAPTER_H
