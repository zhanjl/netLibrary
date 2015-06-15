
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Thread.h"
#include "Timestamp.h"
#include "Callbacks.h"
#include "TimerId.h"

#include <boost/noncopyable.hpp>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <stdio.h>
namespace muduo
{
    class Channel;
    class Poller;
    class TimerQueue;
    class EventLoop : boost::noncopyable
    {
        public:
            EventLoop();
            ~EventLoop();
            Timestamp pollReturnTime() const
            {
                return pollReturnTime_;
            }
            
            TimerId runAt(const Timestamp& time, const TimerCallback& cb);
            TimerId runAfter(double delay, const TimerCallback& cb);
            TimerId runEvery(double interval, const TimerCallback& cb);
            void loop();
            void quit();
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

            void updateChannel(Channel* channel);
        private:

            void abortNotInLoopThread();

            const pid_t threadId_;
            Timestamp pollReturnTime_;
            typedef std::vector<Channel*> ChannelList;
            bool looping_;
            bool quit_;

            boost::scoped_ptr<Poller> poller_;
            boost::scoped_ptr<TimerQueue> timerQueue_;
            ChannelList activeChannels_;
    };
}

#endif
