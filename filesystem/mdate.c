#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>


// cmd line argv;创建一个自己的date命令
int main(int argc, char *argv[])
{
    int c;
    int fd = STDOUT_FILENO;
    time_t tm;
    struct tm *tm_s;
    char buf_format[BUFSIZ] = {0};
    char buf_str[BUFSIZ] = {0};

    tm = time(NULL);
    if ((time_t)-1 == tm) {
        fprintf(stderr, "time error.");
        exit(EXIT_FAILURE);
    }

    tm_s = localtime(&tm);
    if (NULL == tm_s)
    {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getopt(argc, argv, "-y::mdh::Ms");
        if (c < 0) {
            break;
        }
        switch (c)
        {
            case 1:
                if (fd != STDOUT_FILENO) {
                    break;
                }
                fd = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                // fd = open(argv[optind - 1], ....);
                if (fd < 0) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                break;

            case 'y':
                if (optarg == NULL || strcmp(optarg, "1") == 0)
                    strncat(buf_format, "%y ", BUFSIZ-strlen(buf_format)-1);
                else
                    strncat(buf_format, "%Y ", BUFSIZ-strlen(buf_format)-1);

                break;

            case 'm':
                strncat(buf_format, "%m ", BUFSIZ-strlen(buf_format)-1);
                break;

            case 'd':
                strncat(buf_format, "%d ", BUFSIZ-strlen(buf_format)-1);
                break;

            case 'h':
                if (optarg == NULL || strcmp(optarg, "24"))
                    strncat(buf_format, "%H ", BUFSIZ-strlen(buf_format)-1);
                else
                    strncat(buf_format, "%I ", BUFSIZ-strlen(buf_format)-1);
                break;

            case 'M':
                strncat(buf_format, "%M ", BUFSIZ-strlen(buf_format)-1);
                break;

            case 's':
                strncat(buf_format, "%S ", BUFSIZ-strlen(buf_format)-1);
                break;

            default:
                exit(EXIT_FAILURE);
                break;
        }
    }

    strftime(buf_str, BUFSIZ, buf_format, tm_s);
    write(fd, buf_str, strlen(buf_str) + 1);

    if (fd != STDOUT_FILENO)
        close(fd);
    return 0;
}
