#ifndef QUEUE_H
#define QUEUE_H

namespace original {
    template<typename TYPE, typename SERIAL = chain<TYPE>>
    class queue : public iterationStream<TYPE>{
        SERIAL serial_;
    public:
        explicit queue(const SERIAL& serial = chain<TYPE>{});
        _GLIBCXX_NODISCARD size_t size() const;
        _GLIBCXX_NODISCARD bool empty() const;
        void clear();
        void push(TYPE e);
        TYPE pop();
        TYPE top() const;
        TYPE bottom() const;
        iterator<TYPE>* begins() const override;
        iterator<TYPE>* ends() const override;
        _GLIBCXX_NODISCARD std::string toString(bool enter) const override;
    };
}

    template<typename TYPE, typename SERIAL>
    original::queue<TYPE, SERIAL>::queue(const SERIAL& serial) : serial_{serial} {}

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::size() const -> size_t {
        return serial_.size();
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::empty() const -> bool {
        return serial_.empty();
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::clear() -> void {
        serial_.clear();
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::push(TYPE e) -> void {
        serial_.pushEnd(e);
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::pop() -> TYPE {
        return serial_.popBegin();
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::top() const -> TYPE {
        return serial_[-1];
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::bottom() const -> TYPE {
        return serial_[0];
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::begins() const -> iterator<TYPE>* {
        return serial_.begins();
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::ends() const -> iterator<TYPE>* {
        return serial_.ends();
    }

    template<typename TYPE, typename SERIAL>
    auto original::queue<TYPE, SERIAL>::toString(const bool enter) const -> std::string {
        std::stringstream ss;
        ss << "queue" << this->elementsString();;
        if (enter) ss << "\n";
        return ss.str();
    }

#endif //QUEUE_H
