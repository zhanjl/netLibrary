#ifndef TIMERID_H
#define TIMERID_H

#include "copyable.h"

namespace muduo
{

    class Timer;

    class TimerId : public muduo::copyable
    {
        public:
            explicit TimerId(Timer* timer)
                : value_(timer)
            {
            }
        private:
            Timer* value_;
    };

}

#endif
