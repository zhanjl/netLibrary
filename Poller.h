#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <vector>

#include "Timestamp.h"
#include "EventLoop.h"

struct pollfd;

namespace muduo
{

    class Channel;

    class Poller : boost::noncopyable
    {
        public:
            typedef std::vector<Channel*> ChannelList;
            Poller(EventLoop *loop);
            ~Poller();

            Timestamp poll(int timeoutMs, ChannelList* activeChannels);
            void updateChannel(Channel* channel);
            void removeChannel(Channel* channel);

            void assertInLoopThread()
            {
                ownerLoop_->assertInLoopThread();
            }

        private:
            void fillActiveChannels(int numEvents,
                                    ChannelList *activeChannels) const;
            typedef std::vector<struct pollfd> PollFdList;
            typedef std::map<int, Channel*> ChannelMap;

            EventLoop* ownerLoop_;
            PollFdList pollfds_;    //要监控的文件描述符
            ChannelMap channels_;
    };

}

#endif
