#ifndef ORIGINAL_SYNCPOINT_H
#define ORIGINAL_SYNCPOINT_H
#include "mutex.h"
#include "condition.h"

// todo
namespace original {
    class syncPoint {
        const u_integer max_arrived_;
        u_integer arrived_;
        u_integer round_;
        pMutex mutex_;
        pCondition condition_;
        std::function<void()> complete_func_;
    public:
        syncPoint();

        explicit syncPoint(u_integer max_arrived, const std::function<void()>& func = {});

        void arrive();
    };
}

original::syncPoint::syncPoint()
    : max_arrived_(0), arrived_(0), round_(0) {}

original::syncPoint::syncPoint(original::u_integer max_arrived, const std::function<void()>& func)
    : max_arrived_(max_arrived), arrived_(0), round_(0), complete_func_(func) {}

void original::syncPoint::arrive() {
    {
        uniqueLock lock{this->mutex_};
        u_integer this_round = this->round_;
        this->arrived_ += 1;
        if (this->arrived_ < this->max_arrived_) {
            this->condition_.wait(this->mutex_);
        }
    }
}

#endif //ORIGINAL_SYNCPOINT_H
