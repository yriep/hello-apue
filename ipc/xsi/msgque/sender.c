#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"


int main(void)
{
    int id;
    key_t key;
    struct msg_st msg;

    key = ftok(pathkey, projkey);
    if (key < 0) {
        perror("ftok()");
        exit(EXIT_FAILURE);
    }

    id = msgget(key, 0);
    if (id < 0) {
        perror("msgget()");
        exit(EXIT_FAILURE);
    }

    printf("get id is %d\n", id);
    msg.msgtyp = 1;
    strcpy(msg.name, "hongye");
    msg.math = 100;
    msg.chinese = 100;
    if (0 != msgsnd(id, &msg, sizeof(struct msg_st) - sizeof(long), 0)) {
        perror("msgsnd()");
        exit(EXIT_FAILURE);
    }

    return 0;
}
