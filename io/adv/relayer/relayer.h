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

// 显示状态
typedef struct relay_status
{
    int state;
    int fd1;
    int fd2;
    int64_t cnt12, cnt21;
    //struct timeval start, end;
} relay_st;


int relay_addjob(int s_fd, int d_fd);

int relay_canceljob(int id);

int relay_waitjob(int id);
// int relay_waitjob(int fm, relay_st *st);   // 获取任务状态
// int relay_statjob(int id, relay_st *st);
#endif // !RELAY_H
