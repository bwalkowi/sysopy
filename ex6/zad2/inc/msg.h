#ifndef _MSG_H_
#define _MSG_H_

enum    {
                    CLIENT_READY,
                    CLIENT_FINISHED,
                    CHECK_IF_PRIME,
                    PRIME,
                    NOT_PRIME
                };

typedef struct{
    pid_t cid;
    mqd_t cmq;
} client_t;

typedef struct{
    pid_t from;
    uint8_t request;
    uint32_t num;
} msg_t;

#endif // _MSG_H_
