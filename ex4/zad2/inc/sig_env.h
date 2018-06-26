#ifndef _SIG_ENV_H_
#define _SIG_ENV_H_

#define _XOPEN_SOURCE 500

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

#define RTSIG1 SIGRTMIN
#define RTSIG2 SIGRTMIN + 1


extern bool receive;
extern long sig_num;

extern sigset_t usr_unblk;

char *set_sig_mask(int sigusr1, int sigusr2);
char *set_sig_han(int sigusr1, int sigusr2);
char *restore_sig_mask(void);
char *restore_sig_han(void);


#endif /* _SIG_ENV_H_ */
