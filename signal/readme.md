# signal

## 1. signal 信号处理；

## 2. 原理：可重入，信号未定义的行为，信号的响应过程;
## 3. 常用函数 alarm kill raise pause（挂起等待）

### slowcat.c
        通过漏桶方法实现流量控制打印：pause signal。。
### slowcat2.c
        令牌桶实现流控
### slowcat3-setitimer.c
        setitimer 精度优化。

### mytbf / anytimer
        令牌桶和定时器实例
        signal / setitimer / 钩子函数。。

---
常用函数
setitimer -- alarm的优化

## 4. sigaddset -》sigprocmask 信号集
    sigprocmask - 修改mask，可以阻塞信号或解除信号的阻塞，决定响应信号的时机。
###  block.c / block1.c
    block和block1的区别就是一个使用了SET_MASK；通过阻塞信号SIG_INT，使得一行数据打印完之后再处理。

## 5. sigsuspend
    pause的优化；也是对sigprocmask多个操作的原子化。
    sigsuspend
        使用的是pause，但是操作是非原子的，那么有可能在pause前，就接受了信号，导致pause后要再接受一次。
    sigsuspend1
        使用的是sigsuspend，替代了sigprocmask和pause。

    sigsuspend2
        使用实时信号，实时信号不会丢失，可以一次性全输入。

## 6. sigaction
    相比于signal优点，
        1.可以阻塞部分信号；
        2.可以获取信号来源。

### 1. mydae_sig.c    阻塞其余信号
### 2. /apue/mytbf/mytbf_siga.c 中使用sigaction获取信号来源，防止是内核时钟导致tbf多加一次令牌
