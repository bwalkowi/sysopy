#include "sig_env.h"


bool receive        =       true;
long sig_num       =       0;

sigset_t usr_unblk;

static sigset_t procmask;
static sigset_t oprocmask;

char *set_sig_mask(int sigusr1, int sigusr2){
    if(sigemptyset(&procmask) || sigaddset(&procmask, sigusr1) || sigaddset(&procmask, sigusr2))
        return "creating proc mask";

    if(sigprocmask(SIG_SETMASK, &procmask, &oprocmask))
        return "setting proc mask";

    usr_unblk = procmask;
    if(sigdelset(&usr_unblk, sigusr1) || sigdelset(&usr_unblk, sigusr2))
        return "creating usr_unblk mask";

    return NULL;
}

char *restore_sig_mask(void){
    if(sigprocmask(SIG_SETMASK, &oprocmask, NULL))
        return "restoring proc mask";

    return NULL;
}

static void sig_usr1(int signo){
    ++sig_num;
    return;
}

static void sig_usr2(int signo){
    receive = false;
    return;
}

static struct sigaction sa_usr1;
static struct sigaction sa_usr2;

static struct sigaction osa_usr1;
static struct sigaction osa_usr2;

char *set_sig_han(int sigusr1, int sigusr2){
    sigset_t emptyset;
    if(sigemptyset(&emptyset))
        return "creating emptyset";

    sa_usr1.sa_mask     =   sa_usr2.sa_mask     =   emptyset;
	sa_usr1.sa_flags      =    sa_usr2.sa_flags      =   0;

	sa_usr1.sa_handler = sig_usr1;
    if(sigaction(sigusr1, &sa_usr1, &osa_usr1))
        return "setting sigusr1";

    sa_usr2.sa_handler = sig_usr2;
    if(sigaction(sigusr2, &sa_usr2, &osa_usr2))
        return "setting sigusr2";

    return NULL;
}

char *restore_sig_han(void){
    if(sigaction(SIGUSR1, &osa_usr1, NULL))
        return "restoring sigusr1";

    if(sigaction(SIGUSR2, &osa_usr2, NULL))
        return "restoring sigusr2";

    return NULL;
}

