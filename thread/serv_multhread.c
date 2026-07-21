/*多个客户端之间可以交换信息的聊天程序*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

static void *clnt_handler(void *p);
static int clnt_num;
static int clnt_socks[1024];
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_sz;
    int err, i;
    pthread_t tid;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <IP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if (-1 == bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(serv_sock, 5)) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    while (1) {
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_sz);
        if (clnt_sock == -1) {
            continue;
        }
        printf("connect client: %d\n", clnt_sock);
        pthread_create(&tid, NULL, clnt_handler, &clnt_sock);
        pthread_detach(tid);

        pthread_mutex_lock(&mut);
        clnt_socks[clnt_num] = clnt_sock;
        clnt_num++;
        pthread_mutex_unlock(&mut);
    }
    close(serv_sock);
    return 0;
}

#define OUT "I OUT\n"
static void *clnt_handler(void *p)
{
    int i = 0;
    int len = 0;
    char buf[1024];
    int sock = *(int *)p;

    int j = 2;
    //while (0 < (len = read(sock, buf, 1024))) {
    for (int j = 0; j < 2; j++) {
        len = read(sock, buf, 1024);
        printf("get msg is %s\n", buf);

        /*
        pthread_mutex_unlockhread_mutex_lock(&mut);
        for (i = 0; i < clnt_num; i++) {
            write(clnt_socks[i], buf, len);
        }

        pthread_mutex_unlock(&mut);
        */
    }
    pthread_mutex_lock(&mut);
    puts("end!");
    sleep(1);
    write(sock, OUT, strlen(OUT)+1);

    for (i = 0; i < clnt_num; i++) {
        if (sock == clnt_socks[i]) {
            while (i < clnt_num) {
                clnt_socks[i] = clnt_socks[i + 1];
                i++;
            }
            break;
        }
    }
    clnt_num--;
    pthread_mutex_unlock(&mut);
    close(sock);
    return NULL;
}
