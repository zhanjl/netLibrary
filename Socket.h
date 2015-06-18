#ifndef SOCKET_H
#define SOCKET_H

#include <boost/noncopyable.hpp>

namespace muduo
{
    class InetAddress;

    class Socket : boost::noncopyable
    {
        public:
            explicit Socket(int sockfd)
                : sockfd_(sockfd)
            {
            }

            ~Socket();

            int fd() const
            {
                return sockfd_;
            }

            void bindAddress(const InetAddress& localAddr);

            void listen();

            int accept(InetAddress* peeraddr);

            void setReuseAddr(bool on);
            void shutdownWrite();

            void setTcpNoDelay(bool on);
        private:
            const int sockfd_;
    };
}

#endif
