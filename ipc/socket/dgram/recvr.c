#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "proto.h"


int main(void)
{
    int sd;
    struct sockaddr_in recvaddr, fromaddr;
    socklen_t from_len;
    struct msg_st msg;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    recvaddr.sin_family = AF_INET;
    recvaddr.sin_port = htons(RECVPORT);
    inet_pton(sd, "0.0.0.0", &recvaddr.sin_addr.s_addr);
    if (0 > bind(sd, (void *)&recvaddr, sizeof(struct sockaddr_in))) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    from_len = sizeof(fromaddr);
    while (1) {
        recvfrom(sd, &msg, sizeof(struct msg_st), 0, (void *)&fromaddr, &from_len);
        printf("----recv msg from: %s.%d\n", inet_ntoa(fromaddr.sin_addr), fromaddr.sin_port);
        printf("name is %s\n", msg.name);
        printf("math is %d\n", msg.math);
        printf("chinese is %d\n", msg.chinese);
    }
    close(sd);
    return 0;
}
