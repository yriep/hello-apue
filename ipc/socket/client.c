#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    int sd;
    FILE *fp;
    int len = 0;
    char buf[512] = {0};

    sd = socket(AF_INET, SOCK_STREAM, 0);

    // bind()
    //

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret = connect(sd, (void *)&serv_addr, sizeof(serv_addr));

    fp = fdopen(sd, "r");
    while (1) {
        len = fread(buf, 512, 1, fp);
        if (len <= 0) {
            break;
        }
        fwrite(buf, 512, 1, fp);
    }
    return 0;
}
