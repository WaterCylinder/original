#ifndef ORIGINAL_SINGLETON_H
#define ORIGINAL_SINGLETON_H
#include "ownerPtr.h"

namespace original {
    template<typename TYPE>
    class singleton {
        static ownerPtr<TYPE> instance_;

        singleton() = default;

        ~singleton() = default;
    public:
        singleton(const singleton&) = delete;

        singleton& operator=(const singleton&) = delete;

        singleton(singleton&&) = delete;

        singleton& operator=(singleton&&) = delete;

        static bool exist();

        template<typename... Args>
        static void init(Args&&... args);

        static TYPE& instance();

        static void clear();

        template<typename... Args>
        static void reset(Args&&... args);
    };
}

template<typename TYPE>
original::ownerPtr<TYPE> original::singleton<TYPE>::instance_ = original::ownerPtr<TYPE>();

template <typename TYPE>
bool original::singleton<TYPE>::exist() {
    return instance_ != nullptr;
}

template <typename TYPE>
template <typename ... Args>
void original::singleton<TYPE>::init(Args&&... args) {
    if (instance_ == nullptr) {
        instance_ = std::move(makeOwnerPtr<TYPE>(std::forward<Args>(args)...));
    } else {
        throw valueError("Instance already exists, do you mean reset(args...)?");
    }
}

template <typename TYPE>
TYPE& original::singleton<TYPE>::instance() {
    if (instance_ == nullptr) {
        throw nullPointerError("Instance not exist, call init(args...) first");
    }
    return *instance_;
}

template <typename TYPE>
void original::singleton<TYPE>::clear() {
    if (instance_ != nullptr) {
        instance_ = std::move(original::ownerPtr<TYPE>());
    }
}

template <typename TYPE>
template <typename ... Args>
void original::singleton<TYPE>::reset(Args&&... args) {
    instance_ = std::move(makeOwnerPtr<TYPE>(std::forward<Args>(args)...));
}

#endif //ORIGINAL_SINGLETON_H
