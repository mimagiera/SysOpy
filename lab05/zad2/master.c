#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define	LINE_MAX 256

int main(int argc, char **argv)
{
    if(argc==2)
    {
        char *pipe_name = argv[1];
        char line[LINE_MAX];
        mkfifo(pipe_name,S_IRUSR | S_IWUSR);
        FILE *f = fopen(pipe_name,"r");
        while(fgets(line, LINE_MAX, f)!=NULL)
        {
            printf("%s\n",line);
        }
        fclose(f);
    }
    return 0;
}