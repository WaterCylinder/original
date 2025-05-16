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

        bool is_joinable;
        ownerPtr<threadData> thread_data;

        explicit threadBase(Callback c);

        virtual void join() = 0;

        virtual void detach() = 0;
    public:
        threadBase(const threadBase&) = delete;

        threadBase& operator=(const threadBase&) = delete;
    };

    template<typename Callback>
    class pThread : public threadBase<Callback> {
        pthread_t handle;

        static void* pThreadProxy(void* arg);

        void handleInit();

    public:
        explicit pThread(Callback c);

        void join() override;

        void detach() override;

        ~pThread();
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
    : thread_data(makeOwnerPtr(std::move(c))), is_joinable(true) {}

template<typename Callback>
void* original::pThread<Callback>::pThreadProxy(void* arg) {
    auto self = static_cast<pThread*>(arg);
    try {
        self->thread_data->run(arg);
    } catch (const error& e) {
        throw sysError();
    }
    return nullptr;
}

template<typename Callback>
void original::pThread<Callback>::handleInit() {
    pthread_create(&this->handle, nullptr, &pThreadProxy, this);
}

template<typename Callback>
original::pThread<Callback>::pThread(Callback c) : threadBase<Callback>(c), handle() {
    this->handleInit();
}

template<typename Callback>
void original::pThread<Callback>::join() {
    if (this->is_joinable){
        int code = pthread_join(this->handle, nullptr);
        if (code != 0){
            throw sysError();
        }
        this->is_joinable = false;
    }
}

template<typename Callback>
void original::pThread<Callback>::detach() {
    if (this->is_joinable){
        int code = pthread_detach(this->handle);
        if (code != 0){
            throw sysError();
        }
        this->is_joinable = false;
    }
}

template<typename Callback>
original::pThread<Callback>::~pThread() {
    this->detach();
}

#endif //THREAD_H
