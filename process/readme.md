1. fork / wait /waitpid / exec --->   system
    fork.c
        fork例子
    primerN.c 
        创建三个进程，使用交叉分配法获取计算数，来筛质数；
    few.c /exec.c
        exec

    sleep.c / test.c /few.c
        测试exec

    myshell.c
        通过 exec自定义一个终端。

3. 文件权限 u+s  及setsid
    mysu.c  及 mysu的权限修改   修改所有者为root + 修改权限为u+s

4. 守护进程 及 系统日志打印
    deamon
