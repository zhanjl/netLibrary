#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "Timestamp.h"

namespace muduo
{
    class TcpConnection;
    class Buffer;
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef boost::function<void()> TimerCallback;

    typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef boost::function<void (const TcpConnectionPtr&,
                                Buffer *buf,
                                Timestamp)> MessageCallback;

    typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
}

#endif
