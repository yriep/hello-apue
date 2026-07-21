#ifndef _SEM_H__
#define _SEM_H__

typedef void sem_t;
sem_t * sem_init(int val);
int sem_destroy(sem_t *sem);

int sem_add(sem_t*,  int);
int sem_wait(sem_t*, int);

#endif // !_SEM_H__
