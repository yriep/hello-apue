#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int sd;
    FILE *fp;
    long long stamp;

    sd = socket(AF_INET, SOCK_STREAM, 0);

    // bind()
    //

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(SERV_PORT));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sd, (void *)&serv_addr, sizeof(serv_addr));

    fp = fdopen(sd, "r");
    fscanf(fp, FMT_STAMP, &stamp);
    fprintf(stdout, FMT_STAMP, stamp);
    return 0;
}
