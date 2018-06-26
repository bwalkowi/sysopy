#ifndef _MSG_H_
#define _MSG_H_

enum    {
                    CLIENT_READY            =           1,
                    CLIENT_FINISHED,
                    CHECK_IF_PRIME,
                    PRIME,
                    NOT_PRIME
                };

typedef struct{
    pid_t cid;
    int cmq;
} client_t;

typedef struct{
    long request;
    pid_t from;
    int num;
} msg_t;

#endif // _MSG_H_
