
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Thread.h"
#include "Timestamp.h"
#include "Callbacks.h"
#include "TimerId.h"
#include "Mutex.h"

#include <boost/noncopyable.hpp>

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <stdio.h>
namespace muduo
{
    class Channel;
    class EPoller;
    class TimerQueue;
    class EventLoop : boost::noncopyable
    {
        public:
            typedef boost::function<void()> Functor;
            EventLoop();
            ~EventLoop();
            Timestamp pollReturnTime() const
            {
                return pollReturnTime_;
            }

            void runInLoop(const Functor& cb);

            void queueInLoop(const Functor& cb);
            
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

            void wakeup();

            void updateChannel(Channel* channel);
            void removeChannel(Channel* channel);
        private:

            void abortNotInLoopThread();
            void handleRead();
            void doPendingFunctors();
            const pid_t threadId_;
            Timestamp pollReturnTime_;
            typedef std::vector<Channel*> ChannelList;
            bool looping_;
            bool quit_;
            bool callingPendingFunctors_;

            boost::scoped_ptr<EPoller> poller_;
            boost::scoped_ptr<TimerQueue> timerQueue_;

            int wakeupFd_;

            boost::scoped_ptr<Channel> wakeupChannel_;
            ChannelList activeChannels_;

            MutexLock mutex_;
            std::vector<Functor> pendingFunctors_;
    };
}

#endif
