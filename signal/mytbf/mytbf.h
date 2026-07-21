#ifndef _MYTBF_H__
#define _MYTBF_H__

#define MAX_TBF_SIZE 1024
typedef     void   mytbf_t;
/*
    隐藏数据结构；头文件给用户，c文件会编译成动态库
*/
mytbf_t * mytbf_init(int cps, int burst);
int tbftoken_fetch(mytbf_t *, int);
int tbftoken_retur(mytbf_t *, int);
int tbf_destroy(mytbf_t *);


#endif // !_MYTBF_H__