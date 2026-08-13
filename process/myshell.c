#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <glob.h>

#define DELIMS  " \t\n"

typedef struct
{
    glob_t globres;
} cmd_st;


static void prompt(char *str)
{
    printf("%smysh-0.1$ ", str);
}

static void parse(char *line, cmd_st *res)
{
    char *tok;
    int i = 0;

    while (1) {
        tok = strsep(&line, DELIMS);
        if (tok == NULL)
            break;
        if (tok[0] == '\0')
            continue;

        glob(tok, GLOB_NOCHECK | GLOB_APPEND * i, NULL, &res->globres);
        i = 1;
    }
}

int main(int argc, char *argv[])
{
    pid_t pid;
    char *linebuf = NULL;
    size_t linebuf_size = 0;
    cmd_st cmd;

    while (1) {
        prompt(argv[1]);
        if (getline(&linebuf, &linebuf_size, stdin) < 0) {
            break;
        }
        parse(linebuf, &cmd);

        if (0) {    //区分内部，外部命令
            //dothing
        } else {
            pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            }
            if (pid == 0) {
                execvp(cmd.globres.gl_pathv[0], cmd.globres.gl_pathv);
                perror("execvp");
                exit(1);
            } else {
                wait(NULL);
            }
        }
    }
    return 0;



}
