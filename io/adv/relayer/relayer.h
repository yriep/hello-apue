#ifndef RELAY_H
#define RELAY_H

#define MAX_JOB 10000
#define BUFSIZE 1024
enum
{
    RUNNING,
    CANCELED,
    STOPED,
};

typedef struct relay_status
{
    int d_fd;
} relay_st;


int relay_addjob(int s_fd, int d_fd);

int relay_canceljob(int fm);

int relay_waitjob(int fm);

#endif // !RELAY_H
