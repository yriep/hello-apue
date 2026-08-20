#ifndef _MYTBF_H__
#define _MYTBF_H__

#define MAXSIZE_TBF     1024

int mytbf_init(int, int);
int tbf_destroy(int);
int tbf_fetchtoken(int, int);
int tbf_returtoken(int, int);

#endif
