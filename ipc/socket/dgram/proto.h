#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>

#define RECVPORT    1999
#define NAMESIZE    11
#define NAMEMAX     (512 - 8 - 8)

struct msg_st
{
    uint8_t     name[NAMESIZE];
    uint32_t    math;
    uint32_t    chinese;
} __attribute__((packed));

typedef struct flex_msg_t
{
    uint32_t math;
    uint32_t chinese;
    uint8_t  name[];
} __attribute__((packed)) msg_fst;

#endif // PROTO.H__
