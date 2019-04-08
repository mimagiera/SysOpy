#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv)
{
    srand(time(NULL));
    if(argc==3)
    {
        char *pipe_name = argv[1];
        int count = atoi(argv[2]);
        char date_chars[50];
        char pipe_string[70];

        printf("%d\n", (int) getpid());
        int pipe = open(pipe_name,O_WRONLY);

        for(int i=0;i<count;i++)
        {
            FILE *date = popen("date", "r");
            fgets(date_chars,50,date);
            printf("%s\n",date_chars);
            pclose(date);

            sprintf(pipe_string, "%d, %s", (int) getpid(), date_chars);

            write(pipe,pipe_string,strlen(pipe_string));
            sleep(rand()%7+2);
        }
        close(pipe);
    }
    return 0;
}