#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static void *send_msg(void *p);
static void *recv_msg(void *p);
static char name_str[1024];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in sock_addr;
    pthread_t tid_r, tid_w;

    if (argc < 4) {
        fprintf(stderr, "Usage: <%s> <IP> <PORT> <NAME>", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        fprintf(stderr, "socker err\n");
        exit(EXIT_FAILURE);
    }
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sock_addr.sin_port = htons(atoi(argv[2]));
    if (-1 == connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr))) {
        fprintf(stderr, "connect err\n");
        exit(EXIT_FAILURE);
    }
    snprintf(name_str, 1024, "[%s]: ", argv[3]);
    pthread_create(&tid_r, NULL, send_msg, &sock);
    pthread_create(&tid_w, NULL, recv_msg, &sock);

    pthread_join(tid_r, NULL);
    pthread_join(tid_w, NULL);
    close(sock);
    return 0;
}

static void *send_msg(void *p)
{
    char buf[1024];
    char name_msg[1024];
    int len;
    int sock = *(int *)p;
    for (int i = 0; i < 2; i++) {
   // while (1) {
        fputs("input your msg:(q is quit): ",stdout);
        fgets(buf, 1024, stdin);
        if (buf[0] == 'q' || buf[0] == 'Q')
            break;
        snprintf(name_msg, 1024, "%s%s", name_str, buf);
        printf("send msg: %s", name_msg);
        fflush(stdout);
        write(sock, name_msg, strlen(name_msg)+1);
    }
    close(sock);
    //shutdown(sock, SHUT_WR);
    return NULL;
}

static void *recv_msg(void *p)
{
    char buf[1024];
    int sock = *(int *)p;
    int len;

    while (0 < (len = read(sock, buf, 1024))) {
        printf("recv msg: %s", buf);
    }
    fflush(stdout);
    close(sock);
    return NULL;
}


