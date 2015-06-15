#include "EventLoop.h"
#include "Logging.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"

#include <assert.h>
#include <poll.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <boost/bind.hpp>

using namespace muduo;

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

static int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }

    return evtfd;
}

EventLoop::EventLoop()
      :threadId_(CurrentThread::tid()),
      looping_(false),
      quit_(false),
      callingPendingFunctors_(false),
      poller_(new Poller(this)),
      timerQueue_(new TimerQueue(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_))
{
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;

    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
            << " exists in this thread " << threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback(
            boost::bind(&EventLoop::handleRead, this));

    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();

    looping_ = true;
    quit_ = false;

    while (!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (ChannelList::iterator it = activeChannels_.begin();
                it != activeChannels_.end(); it++)
        {
            (*it)->handleEvent();
        }

        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop " << this 
        << " was created in threadId_= " << threadId_
        << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::quit()
{
    quit_ = true;

    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}
void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}


TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleRead()";
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;

    ssize_t n = ::read(wakeupFd_, &one, sizeof one);

    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleRead()";
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < functors.size(); i++)
    {
        functors[i]();
    }

    callingPendingFunctors_ = false;
}
