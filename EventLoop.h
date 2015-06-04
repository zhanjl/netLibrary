
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Thread.h"
#include <boost/noncopyable.hpp>

namespace muduo
{
    class EventLoop : boost::noncopyable
    {
        public:
            EventLoop();
            ~EventLoop();

            void loop();

            void assertInLoopThread()
            {
                if (!isInLoopThread())
                {
                    abortNotInLoopThread();
                }
            }
            bool isInLoopThread() const
            {
                return threadId_ == CurrentThread::tid();
            }
        private:
            void abortNotInLoopThread();
            bool looping_;
            const pid_t threadId_;
    };
}

#endif
