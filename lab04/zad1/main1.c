#define _XOPEN_SOURCE 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define TIME_FMT "%Y.%m.%d %H:%M:%S"
int flag=1;
void au(int a)
{
    if (flag) {
		puts("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu");
		flag = 0;
	} else {
		flag = 1;
}
}
void sig_int(int a)
{
    printf("Odebrano sygnał SIGINT\n");
    exit(1);
}

int main()
{
    struct sigaction act; 
    act.sa_handler = au; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 

    sigaction(SIGTSTP, &act, NULL); 

    signal(SIGINT, sig_int);

    while(1)
    {
        if(flag)
        {
            time_t mytime = time(NULL);
            char * time_str = ctime(&mytime);
            time_str[strlen(time_str)-1] = '\0';
            printf("Current Time : %s\n", time_str);
        }
        sleep(1);
    }
    return 0;
}