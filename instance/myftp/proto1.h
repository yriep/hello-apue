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

typedef struct st_req
{
    int mtype;                  // mtype is E_REQ
    char path[PATH_LEN];
} req_t;


typedef struct st_data
{
    int mtype;
    int datalen;
    char data[DATA_SIZ];
} data_t;

typedef struct st_eof
{
    int mtype;
} eof_t;

typedef union st_s2c
{
    long mtype;
    data_t data;
    eof_t  eof;
} s2c_t;

#endif // !PROTO1_H__
