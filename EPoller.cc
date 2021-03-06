#include "EPoller.h"

#include "Channel.h"
#include "Logging.h"

#include <boost/static_assert.hpp>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>

using namespace muduo;

BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);

namespace
{
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

EPoller::EPoller(EventLoop *loop)
    : ownerLoop_(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_SYSFATAL << "EPoller::EPoller";
    }
}

EPoller::~EPoller()
{
    close(epollfd_);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = epoll_wait(epollfd_,
                        &*events_.begin(),
                        static_cast<int>(events_.size()),
                        timeoutMs);
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);

        if (implicit_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_TRACE << "nothing happended";
    }
    else
    {
        LOG_SYSERR << "EPoller::poll()";
    }

    return now;
}

void EPoller::fillActiveChannels(int numEvents,
                        ChannelList* activeChannels) const
{
    assert(implicit_cast<size_t>(numEvents) <= events_.size());
    for (int i = 0; i < numEvents; i++)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoller::updateChannel(Channel* channel)
{
    const int index = channel->index();

    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }

    else
    {
        int fd = channel->fd();
        (void)fd;

        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void)n;
    assert(n == 1);

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);        
    }
    channel->set_index(kNew);
}

void EPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {   
            LOG_SYSERR << "epoll_ctl op = " << operation << "fd = " << fd;
        }
        else
        {
            LOG_SYSFATAL << "epoll_ctl op = " << operation << "fd = " << fd;
        }
    }
}
