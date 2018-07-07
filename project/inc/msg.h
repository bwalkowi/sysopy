#ifndef _MSG_H_
#define _MSG_H_

#define START 1
#define STOP 2
#define MSG_LEN 256

typedef struct {
    char request;
    char msg[MSG_LEN];
} msg_t;

#endif // _MSG_H_

