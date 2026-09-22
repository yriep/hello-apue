#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "proto.h"


int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in sendaddr;
    struct msg_st msg;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(RECVPORT);
    inet_pton(sd, GROUP_ADDR, &sendaddr.sin_addr);

    memcpy(msg.name, "ludel", 6);
    msg.math = 100;
    msg.chinese = 150;
    sendto(sd, &msg, sizeof(struct msg_st), 0, (void *)&sendaddr, sizeof(struct sockaddr_in));
    close(sd);
    return 0;
}
