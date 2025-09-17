#ifndef ORIGINAL_SINGLETON_H
#define ORIGINAL_SINGLETON_H
#include "ownerPtr.h"

namespace original {
    /**
     * @class singleton
     * @tparam TYPE Type of the singleton instance
     * @brief Thread-safe singleton pattern implementation with ownership management
     * @details Provides a global point of access to a single instance of TYPE while
     *          ensuring proper initialization, destruction, and ownership semantics.
     *          Uses ownerPtr for exclusive ownership and automatic cleanup.
     *
     * Key Features:
     * - Lazy initialization with thread safety
     * - Exclusive ownership using ownerPtr
     * - Controlled initialization and destruction
     * - Prevention of copy and move operations
     * - Exception-safe operations
     *
     * @note This implementation is not thread-safe for simultaneous init()/reset() calls.
     *       For thread-safe usage, ensure proper synchronization at the application level.
     */
    template<typename TYPE>
    class singleton {
        static ownerPtr<TYPE> instance_; ///< Unique instance managed by ownerPtr

        /**
         * @brief Private constructor to prevent external instantiation
         */
        singleton() = default;

        /**
         * @brief Private destructor to prevent external destruction
         */
        ~singleton() = default;

    public:
        /**
         * @brief Deleted copy constructor to prevent copying
         */
        singleton(const singleton&) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying
         */
        singleton& operator=(const singleton&) = delete;

        /**
         * @brief Deleted move constructor to prevent moving
         */
        singleton(singleton&&) = delete;

        /**
         * @brief Deleted move assignment operator to prevent moving
         */
        singleton& operator=(singleton&&) = delete;

        /**
         * @brief Checks if the singleton instance exists
         * @return true if instance is initialized, false otherwise
         */
        static bool exist();

        /**
         * @brief Initializes the singleton instance with provided arguments
         * @tparam Args Argument types for TYPE constructor
         * @param args Arguments to forward to TYPE constructor
         * @throws valueError if instance already exists
         *
         * @code
         * singleton<MyClass>::init(arg1, arg2); // Initialize with constructor arguments
         * @endcode
         */
        template<typename... Args>
        static void init(Args&&... args);

        /**
         * @brief Provides access to the singleton instance
         * @return Reference to the singleton instance of TYPE
         * @throws nullPointerError if instance not initialized
         *
         * @code
         * auto& instance = singleton<MyClass>::instance(); // Access the instance
         * instance.someMethod();
         * @endcode
         */
        static TYPE& instance();

        /**
         * @brief Clears the singleton instance
         * @post instance_ becomes nullptr, existing instance is destroyed
         * @note Safe to call even if instance doesn't exist
         */
        static void clear();

        /**
         * @brief Resets the singleton instance with new arguments
         * @tparam Args Argument types for TYPE constructor
         * @param args Arguments to forward to TYPE constructor
         * @note Destroys existing instance and creates a new one
         *
         * @code
         * singleton<MyClass>::reset(newArg1, newArg2); // Replace existing instance
         * @endcode
         */
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
