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
    char buf[] = "abcdefg";
    struct sockaddr_in  servaddr;
    char recv[1024];
    bzero(&servaddr, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9981);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    connect(sockfd, (struct sockaddr*)&servaddr, sizeof servaddr);

    while (1)
    {
        write(sockfd, buf, strlen(buf));
        n = read(sockfd, recv, sizeof recv);
        recv[n] = '\0';
        printf("%s", recv);
    }
    return 0;
}
