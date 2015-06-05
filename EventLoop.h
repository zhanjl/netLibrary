
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Thread.h"
#include <boost/noncopyable.hpp>

#include <vector>
#include <boost/scoped_ptr.hpp>
namespace muduo
{
    class Channel;
    class Poller;
    class EventLoop : boost::noncopyable
    {
        public:
            EventLoop();
            ~EventLoop();

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
            typedef std::vector<Channel*> ChannelList;
            void abortNotInLoopThread();
            bool looping_;
            bool quit_;
            const pid_t threadId_;

            boost::scoped_ptr<Poller> poller_;
            ChannelList activeChannels_;
    };
}

#endif
