#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"


int main(void)
{
    int msgid;
    int len = 0;
    key_t key;
    struct msg_st grade;

    key = ftok(pathkey, projkey);
    if (key < 0) {
        perror("ftok()");
        exit(EXIT_FAILURE);
    }

    msgid = msgget(key, IPC_CREAT | 0600);
    if (msgid < 0) {
        perror("msgget()");
        exit(EXIT_FAILURE);
    }

    printf("get id is %d\n", msgid);
    while (1) {
        len = msgrcv(msgid, &grade, sizeof(struct msg_st) - sizeof(long), 0, 0);
        if (len < 0) {
            perror("msgrcv()");
            break;
        }
        printf("name is %s\n", grade.name);
        printf("math is %d\n", grade.math);
        printf("chinese os %d\n", grade.chinese);
    }

    if (msgctl(msgid, IPC_RMID, NULL) != 0) {
        perror("close msg");
        exit(EXIT_FAILURE);
    }
    return 0;
}
