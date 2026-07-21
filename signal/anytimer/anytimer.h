#ifndef ANYTIMER_H__
#define ANYTIMER_H__

#define MAX_TIMER   1024
typedef void (* at_jobfunc_t)(void *);

/*
 *  return value
 *  >= 0   succ
 *  = -EINVAL invalid argument
 *  = -ENOSPC no space
 *  = -ENOMEM out of memory
 */
int at_addjob(int sec, at_jobfunc_t jobp, void *argv);

/*
 *  return
 *  0 succ
 *  EINVAL
 *  EBUSY  task finish
 *  ECANCELED   operate cancel
 * */
int at_canceljob(int id);

/*
 * 0
 * EINVAL
 * 
!! 类似于进程的wait，调用wait获取job结束，也就是将异步操作同步化，获取异步操作的结果。
 */
int at_waitjob(int id);




/*
at_pausejob
at_resumjob
*/

#endif // !ANYTIMER_H__
