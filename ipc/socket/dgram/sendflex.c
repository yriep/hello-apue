
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
    int size;
    int sd;
    struct sockaddr_in sendaddr;
    msg_fst *msg;

    if (strlen(argv[2]) > NAMEMAX) {
        fprintf(stderr, "NAME is  too long!\n");
        exit(EXIT_FAILURE);
    }
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(RECVPORT);
    sendaddr.sin_addr.s_addr = inet_addr(argv[1]);

    size = NAMEMAX+sizeof(msg_fst);
    msg = malloc(size);
    memset(msg, 0, size);

    memcpy(msg->name, argv[2], strlen(argv[2]));
    msg->math = 100;
    msg->chinese = 150;
    sendto(sd, msg, size, 0, (void *)&sendaddr, sizeof(struct sockaddr_in));
    close(sd);
    return 0;
}
