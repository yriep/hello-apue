# 文件系统

## 1. 目录及文件
1. 文件属性
    1. stat fstat lstat获取文件属性
    2. 文件类型 dcb-lsp
    3. 文件权限 rwx --》相应宏值 S_ISDIR （inode）中
    4. 文件属性设置 chmod fchmode umask
    5. 特殊文件 空洞文件
    6. 注意文件属性中的 blksize blocks（占用磁盘空间-以blksize为单位） size（字节）

    粘住位T，文件系统，链接， utime，mkdir chdir getcwd等。。

2. 目录
    glb.c
        glob函数的使用：根据通配符获取路径信息    

    dir_op.c
        dir相关文件

    mydu.c
    --》根据glob自己实现的disk usage命令。
        用到了glob和递归。

    --> 根据readdir实现disk usage
        用readdir替换glob，循环获取目录项。

## 2. 系统数据文件和信息
1. passwd / group / shadow
    passcheck.c
        密码的输入，加密，获取，校验 crypt加密时 str的设置，即hash的设置。

2. 时间戳
    系统使用 time_t， 函数使用 struct tm，阅读使用string --strftime
    timelog.c
        时间信息的转换
    timeadd.c
        mktime的特殊用法

## 3. 进程环境
1. main exit 环境变量 程序资源布局 
2. 命令行参数解析
    mdate.c --- getopt

3. 跳转函数
4. 程序资源

## c的用法
    数字使用，没有单位的数字没有意义。
    使用lssek创建一个5G空洞文件时，函数参数类型是long，
        但是传参不带单位时，默认为int就会有类型转换问题，导致不能创建5G文件。

