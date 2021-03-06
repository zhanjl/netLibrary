#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Callbacks.h"
#include "TcpConnection.h"

#include <map>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace muduo
{
    class Acceptor;
    class EventLoop;

    class TcpServer : boost::noncopyable
    {
        public:
            TcpServer(EventLoop* loop, const InetAddress& listenAddr);
            ~TcpServer();

            void start();

            void setConnectionCallback(const ConnectionCallback& cb)
            {
                connectionCallback_ = cb;
            }

            void setMessageCallback(const MessageCallback& cb)
            {
                messageCallback_ = cb;
            }
        private:
            void newConnection(int sockfd, const InetAddress& peerAddr);
            void removeConnection(const TcpConnectionPtr& conn);
            typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

            EventLoop* loop_;
            const std::string name_;

            boost::scoped_ptr<Acceptor> acceptor_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;

            bool started_;
            int nextConnId_;
            ConnectionMap connections_;
    };
}
#endif
