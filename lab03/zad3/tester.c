#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define TIME_FORMAT_DATE "%Y-%m-%d %H-%M-%S"
char *random_word(int length)
{
    char *word = malloc(sizeof(char)*length);
    for(int i=0;i<length;i++)
    {
        word[i]='a'+rand()%26;
    }

    return word;
}
int main(int argc, char **argv)
{
    srand(time(0));
    if(argc==5)
    {
        char *file_name= argv[1];
        int pmin = atoi(argv[2]);
        int pmax = atoi(argv[3]);
        int bytes = atoi(argv[4]);
        int rand_time;
        int ptotal=4;
        int pcurr=0;
        while(pcurr<ptotal)
        {
            rand_time=pmin+rand()%(pmax+1-pmin);
            pcurr+=rand_time;
            time_t czas;
            struct tm * ptr;
            time( & czas );
            ptr = localtime( & czas );
            char * data = asctime( ptr );

            FILE *f = fopen(file_name,"a+");

            fprintf(f,"%d %d %s",(int)getpid(),rand_time,data);
            fprintf(f,"%s",random_word(bytes));

            fclose(f);
            sleep(rand_time);

        }

    }
}