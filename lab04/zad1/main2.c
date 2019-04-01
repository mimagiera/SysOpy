#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
int flag=1;
pid_t child_pid;

void spawn_child() {
	pid_t child = vfork();
	if (child == 0) {
		execl("./date.sh", "./date.sh", NULL);
	} else {
		child_pid = child;
	}
}

static void sig_tstp(int _) {
	if (flag) {
		puts("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu");
		flag = 0;
		kill(child_pid, SIGINT);
	} else {
		spawn_child();
		flag = 1;
	}
}

static void sig_int(int _) {
	puts("Odebrano sygnał SIGINT");
	if (flag)
		kill(child_pid, SIGINT);
	exit(1);
}

int main()
{
    struct sigaction act; 
    act.sa_handler = sig_tstp; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 

    sigaction(SIGTSTP, &act, NULL);

    signal(SIGINT, sig_int);

    spawn_child();

    while(1){
        pause();
    }
    return 0;
}