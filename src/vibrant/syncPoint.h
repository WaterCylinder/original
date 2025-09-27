#ifndef ORIGINAL_SYNCPOINT_H
#define ORIGINAL_SYNCPOINT_H
#include "mutex.h"
#include "condition.h"


namespace original {
    class syncPoint {
        const u_integer max_arrived_;
        u_integer arrived_;
        u_integer round_;
        mutable pMutex mutex_;
        mutable pCondition condition_;
        std::function<void()> complete_func_;
        std::exception_ptr e_;
    public:
        syncPoint();

        explicit syncPoint(u_integer max_arrived, const std::function<void()>& func = {});

        void arrive();

        u_integer maxArrived() const;

        u_integer currentArrived() const;
    };
}

inline original::syncPoint::syncPoint()
    : max_arrived_(0), arrived_(0), round_(0) {}

inline original::syncPoint::syncPoint(const u_integer max_arrived, const std::function<void()>& func)
    : max_arrived_(max_arrived), arrived_(0), round_(0), complete_func_(func) {}

inline void original::syncPoint::arrive() {
    {
        uniqueLock lock{this->mutex_};
        if (max_arrived_ == 0) {
            return;
        }
        u_integer this_round = this->round_;
        this->arrived_ += 1;
        if (this->arrived_ != this->max_arrived_) {
            this->condition_.wait(this->mutex_, [this, this_round]
            {
                return this->round_ != this_round;
            });
        } else {
            this->round_ += 1;
            if (this->complete_func_) {
                try {
                    this->complete_func_();
                }catch (...){
                    this->e_ = std::current_exception();
                }
            }
            this->arrived_ -= this->max_arrived_;
            lock.unlock();
            this->condition_.notifyAll();
            if (this->e_) {
                throw this->e_;
            }
        }
    }
}

inline original::u_integer original::syncPoint::maxArrived() const
{
    return this->max_arrived_;
}

inline original::u_integer original::syncPoint::currentArrived() const
{
    uniqueLock lock{this->mutex_};
    return this->arrived_;
}

#endif //ORIGINAL_SYNCPOINT_H
