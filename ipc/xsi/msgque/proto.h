#ifndef PROTO_H__
#define PROTO_H__


#define     namelen     32
#define     pathkey     "./proto.h"
#define     projkey     'A'
#define     MSG_SZ      1024

struct msg_st
{
    long msgtyp;
    char name[namelen];
    int  math;
    int chinese;
};

#endif // !PROTO_H__
