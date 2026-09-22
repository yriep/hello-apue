#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <unistd.h>
#include <shadow.h>
#include <errno.h>


int main(int argc, char *argv[])
{
    char *pass;
    char *crypt_str;
    struct spwd *pwinfo;

    if (argc < 2) {
        fprintf(stderr, "Usage: <%s> <user>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (NULL == (pass = getpass("Passwd: "))) {
        perror("getpass");
        exit(EXIT_FAILURE);
    }

    pwinfo = getspnam(argv[1]);
    if (pwinfo == NULL) {
        printf("errno: %d\n", errno);
        perror("getspnam");
        exit(EXIT_FAILURE);
    }

    puts(pwinfo->sp_pwdp);
    crypt_str = crypt(pass, pwinfo->sp_pwdp);
    puts(crypt_str);
    if (strcmp(crypt_str, pwinfo->sp_pwdp) == 0) {
        puts("ok");
    } else {
        puts("fail");
    }

    return 0;
}
