#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

#include "proto.h"
// 静态进程池 
static void serv_job(int socket)
{
    time_t tm;
    char buf[512];

    tm = time(NULL);
    snprintf(buf, 512, FMT_STAMP, (long long)tm);

    if (0 > send(socket, buf, strlen(buf) + 1, 0)) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    int sd, clnt_sd;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t sock_len;
    pid_t pid;
    time_t tm;
    char buf[512];

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /*IPPROTO_SCTP*/
    if (sd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(SERV_PORT));
    inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr);
    if (0 > bind(sd, (void *)&serv_addr, sizeof(struct sockaddr_in))) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (0 > listen(sd, 5)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    sock_len = sizeof(struct sockaddr_in);
    while (1) {
        clnt_sd = accept(sd, (void *)&clnt_addr, &sock_len);
        if (0 > clnt_sd) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
//            close(clnt_sd);
            wait(NULL);
        } else {
            close(sd);
            serv_job(clnt_sd);
            close(clnt_sd);
            exit(EXIT_SUCCESS);
        }
    }

    close(sd);
    return 0;
}
