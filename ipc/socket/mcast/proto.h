#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>

#define RECVPORT    1999
#define NAMESIZE    11

#define GROUP_ADDR  "224.1.1.3"

struct msg_st
{
    uint8_t     name[NAMESIZE];
    uint32_t    math;
    uint32_t    chinese;
};

#endif // PROTO.H__
