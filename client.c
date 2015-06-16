#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
int main()
{
    int sockfd, n;
    char buf[1024];

    struct sockaddr_in  servaddr;

    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9981);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof servaddr);

    n = read(sockfd, buf, sizeof buf);
    buf[n] = '\0';
    printf("%s", buf);
    return 0;
}
