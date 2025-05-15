#ifndef THREAD_H
#define THREAD_H

#include "error.h"
#include "functional"
#include "thread"
#include "ownerPtr.h"


namespace original {
    template<typename Callback>
    class threadBase {
    protected:
        class threadData {
        public:
            std::function<Callback> func;

            explicit threadData(Callback c);

            template<typename... ARGS>
            void run(ARGS&&... args);
        };

        ownerPtr<threadData> thread_data;

        explicit threadBase(Callback c);

    public:
        threadBase(const threadBase&) = delete;

        threadBase& operator=(const threadBase&) = delete;
    };

    template<typename Callback>
    class pThread : public threadBase<Callback> {
        pthread_t handle;

        static void* pThreadProxy(void* arg);

        void handleInit();

        explicit pThread(Callback c);
    };
}

template<typename Callback>
original::threadBase<Callback>::threadData::threadData(Callback c)
    : func(std::move(c)) {}

template<typename Callback>
template<typename... ARGS>
void original::threadBase<Callback>::threadData::run(ARGS &&... args) {
    this->func(std::forward<ARGS>(args)...);
}

template<typename Callback>
original::threadBase<Callback>::threadBase(Callback c)
    : thread_data(makeOwnerPtr(std::move(c))) {}

template<typename Callback>
void* original::pThread<Callback>::pThreadProxy(void* arg) {
    auto self = static_cast<pThread*>(arg);
    try {
        self->thread_data.run(arg);
    } catch (const error& e) {
        throw sysError();
    }
    return nullptr;
}

template<typename Callback>
void original::pThread<Callback>::handleInit() {
    pthread_create(&this->handle, nullptr, &pThreadProxy, nullptr);
}

template<typename Callback>
original::pThread<Callback>::pThread(Callback c) : threadBase<Callback>(c), handle() {
    this->handleInit();
}

#endif //THREAD_H
