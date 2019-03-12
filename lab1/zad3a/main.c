#include "lib.h"
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

FILE *f;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}
char* generateString(int size)
{
    char *a = calloc(size,sizeof(char));
    for(int i = 0;i<size;i++)
    {
        a[i]='a';
    }
    return a;
}
void saveTimesToFile(clock_t startTime,clock_t endTime, struct tms *startTms, struct tms *endTms)
{
    fprintf(f,"%lf   ", calculate_time(startTime, endTime));
    fprintf(f,"%lf   ", calculate_time(startTms->tms_utime, endTms->tms_utime));
    fprintf(f,"%lf ", calculate_time(startTms->tms_stime, endTms->tms_stime));
	fprintf(f,"\n");
    fprintf(f,"\n");
}

int main(int argc, char **argv)
{
    if(argc<2)
    {
        printf("%s\n","Not enough number of arguments");
        return 1;
    }

    f = fopen("raport3a_part.txt", "a");

    createArrayOfPointers(atoi(argv[1]));

    for(int i=2;i<argc;i++)
    {
        if(strcmp(argv[i],"search")==0)
        {
            if(i+3<argc)
            {
                char* dir = argv[i+1];
                char* fileName = argv[i+2];
                char* tmpFileName = argv[i+3];

                struct tms tmsStart;
                clock_t start = times(&tmsStart);

                setCurrentPathAndFileName(dir,fileName,tmpFileName);
                findFileAndSaveToTmp();
                saveTmpToBlock();

                struct tms tmsEnd;
                clock_t end = times(&tmsEnd);

                fprintf(f,"%s\n","Search:");
                saveTimesToFile(start,end,&tmsStart,&tmsEnd);

                deleteBlockAtIndex(0);
                i+=3;
            }
        }
        else if(strcmp(argv[i],"save")==0)
        {
            if(i+1<argc)
            {
                int size = atoi(argv[i+1]);

                struct tms tmsStart;
                clock_t start = times(&tmsStart);

                allocateAtIndex(0,generateString(size));

                struct tms tmsEnd;
                clock_t end = times(&tmsEnd);

                fprintf(f,"%s%d%s\n","Allocate: (",size," block size)");
                saveTimesToFile(start,end,&tmsStart,&tmsEnd);

                deleteBlockAtIndex(0);
                i++;
            }
        }
        else if(strcmp(argv[i],"delete")==0)
        {
            if(i+1<argc)
            {
                int size = atoi(argv[i+1]);
                allocateAtIndex(0,generateString(size));

                struct tms tmsStart;
                clock_t start = times(&tmsStart);

                deleteBlockAtIndex(0);

                struct tms tmsEnd;
                clock_t end = times(&tmsEnd);

                fprintf(f,"%s%d%s\n","Delete: (",size," block size)");
                saveTimesToFile(start,end,&tmsStart,&tmsEnd);
                i++;
            }
        }
        else if(strcmp(argv[i],"add_and_delete")==0)
        {
            if(i+2<argc)
            {
                int numberOfBlocks = atoi(argv[i+1]);
                int sizeOfOneBlock = atoi(argv[i+2]);
                int numberOfIterations = 3;

                struct tms tmsStart;
                clock_t start = times(&tmsStart);

                for(int k=0;k<numberOfIterations;k++)
                {
                    for(int j=0;j<numberOfBlocks;j++)
                    {
                        allocateAtIndex(j,generateString(sizeOfOneBlock));
                    }
                    for(int j=0;j<numberOfBlocks;j++)
                    {
                        deleteBlockAtIndex(j);
                    }
                }

                struct tms tmsEnd;
                clock_t end = times(&tmsEnd);

                fprintf(f,"%s%d%s%d%s%d\n","Allocate and delete ",numberOfIterations," times ",numberOfBlocks," blocks of size ",sizeOfOneBlock);
                saveTimesToFile(start,end,&tmsStart,&tmsEnd);

                i+=2;
            }
        }

    }
    fclose(f);

    deleteArray();

    return 0;
}