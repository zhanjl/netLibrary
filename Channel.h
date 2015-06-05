#ifndef CHANNEL_H
#define CHANNEL_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace muduo
{

    class EventLoop;
    
    class Channel : boost::noncopyable
    {
        public:
            typedef boost::function<void()> EventCallback;

            Channel(EventLoop *loop, int fd);

            void handleEvent();
            void setReadCallback(const EventCallback& cb)
            {
                readCallback_ = cb;
            }

            void setWriteCallback(const EventCallback& cb)
            {
                writeCallback_ = cb;
            }

            void setErrorCallback(const EventCallback& cb)
            {
                errorCallback_ = cb;
            }

            int fd() const 
            {
                return fd;
            }

            int events() const
            {
                return events_;
            }

            void set_revents(int revt)
            {
                revents_ = revt;
            }

            bool isNoneEvent()
            {
                events_ |= kReadEvent;
                update();
            }

            int index()
            {
                return index_;
            }

            void set_index(int idx)
            {
                index_ = idx;
            }

            EventLoop* ownerLoop()
            {
                return loop_;
            }
        private:
            void update();
            //事件标志
            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            EventLoop *loop_;
            const int fd_;  //监控的文件描述符
            int events_;    //要监控的事件
            int revents_;   //返回的事件
            int index_;

            EventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback errorCallback_;
    };

}

#endif
