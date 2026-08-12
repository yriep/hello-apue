#ifndef __MY_PIPE_H_
#define __MY_PIPE_H_

#define MAX_PIPE 1024
#define READER   0x0001
#define WRITER   0x0002

typedef void pipe;

pipe *pipe_init(void);
int pipe_destroy(pipe *);

int pipe_read(pipe *, void *, int);
int pipe_write(pipe *, const void *, int);

int pipe_register(pipe *, int);
int pipe_unregister(pipe *, int);
#endif // !__MY_PIPE_H_
