#ifndef PROTO1_H__
#define PROTO1_H__

#define KEYPATH     "./proto1.h"
#define KEYPROJ     'a'

#define PATH_LEN    32
#define DATA_SIZ    1024

enum
{
    MSG_REQ = 1,
    MSG_DATA,
    MSG_EOF
};

/* 当server端和client端互相发消息时，根据mtype决定接收哪个包，
 * 但是写程序时，如何判断该用哪个结构体接收呢？
 * 有如下三种方法
 *
 * 第一：最合理的 使用union，可以用在任意程序中
 * 第二/第三类似，只能用在结构体简单且类似的程序中。
 *
**/
typedef struct st_req
{
    int mtype;                  // mtype is E_REQ
    char path[PATH_LEN];
} req_t;


typedef struct st_data
{
    int mtype;     // 第三种方法：只使用这一个结构体，以mtype决定是哪种包。
    /*
     * 第二种方法：
     * datalen > 0 data
     * datalen = 0 eot
     * */
    int datalen;
    char data[DATA_SIZ];
} data_t;

typedef struct st_eof
{
    int mtype;
} eof_t;

// 第一种方法：用union接收server端发送过来的结构体，该结构体实际上只会时data_t or eof_t，但是又可以利用mtype读取前long 大小的字节。
typedef union st_s2c
{
    long mtype;
    data_t data;
    eof_t  eof;
} s2c_t;

#endif // !PROTO1_H__
