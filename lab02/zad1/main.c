#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
FILE *f;
double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}
char *randomWord(int length)
{
    char *word = malloc(sizeof(char)*length);
    for(int i=0;i<length;i++)
    {
        word[i]='a'+rand()%26;
    }
    return word;
}
void saveTimesToFile(char *additionalInfo,clock_t startTime,clock_t endTime, struct tms *startTms, struct tms *endTms)
{
    fprintf(f,"%s\n",additionalInfo);
    fprintf(f,"%lf   ", calculate_time(startTime, endTime));
    fprintf(f,"%lf   ", calculate_time(startTms->tms_utime, endTms->tms_utime));
    fprintf(f,"%lf ", calculate_time(startTms->tms_stime, endTms->tms_stime));
	fprintf(f,"\n");
    fprintf(f,"\n");
}
void generate(char *fileName, int numberOfRecords, int sizeOfRecord)
{
    FILE *file=fopen(fileName,"w");
    char *word;
    for(int i=0;i<numberOfRecords;i++)
    {
        word=randomWord(sizeOfRecord);
        fprintf(file,"%s",word);
    }
    free(word);
    fclose(file);
}

void sortLib(char *fileName, int numberOfRecords, int sizeOfRecord)
{
    FILE *file=fopen(fileName,"r+");
    fpos_t pos;
    fpos_t newFirstPos;
    fpos_t minPos;

    char *a=calloc(sizeOfRecord,sizeof(char));
    char *newFirstElem=calloc(sizeOfRecord,sizeof(char));
    char *min=calloc(sizeOfRecord,sizeof(char));

    struct tms tmsStart;
    clock_t start = times(&tmsStart);

    for(int i=0;i<numberOfRecords;i++)
    {
        fseek(file,i*(sizeOfRecord),0);     
        fgetpos(file,&minPos);
        fread(min,sizeof(char),sizeOfRecord,file); 

        newFirstPos=minPos;
        strcpy(newFirstElem,min);

        for(int k=i+1;k<numberOfRecords;k++)
        {     
            fgetpos(file,&pos);
            fread(a,sizeof(char),sizeOfRecord,file);
            if(a[0]<min[0])
            {
                
                minPos=pos;
                strcpy(min,a);
            }
        }

        fsetpos(file,&minPos);
        fwrite(newFirstElem,sizeof(char),sizeOfRecord,file);

        fsetpos(file,&newFirstPos);
        fwrite(min,sizeof(char),sizeOfRecord,file);
    }
    struct tms tmsEnd;
    clock_t end = times(&tmsEnd);
    char message[100];
    snprintf(message,sizeof(message),"Sorted %d records of size %d by lib functions",numberOfRecords,sizeOfRecord);

    saveTimesToFile(message,start,end,&tmsStart,&tmsEnd);

    fclose(file);
    free(a);
    free(newFirstElem);
    free(min);
}

void sortSys(char *fileName, int numberOfRecords, int sizeOfRecord)
{
    int file =open(fileName,O_RDWR);
    char *a=calloc(sizeOfRecord,sizeof(char));
    char *newFirstElem=calloc(sizeOfRecord,sizeof(char));
    char *min=calloc(sizeOfRecord,sizeof(char));

    long minPos,newFirstPos,pos;

    struct tms tmsStart;
    clock_t start = times(&tmsStart);

    for(int i=0;i<numberOfRecords;i++)
    {
        minPos=i*(sizeOfRecord);
        lseek(file,minPos,SEEK_SET);
        read(file,min,sizeOfRecord);
        newFirstPos=minPos;
        strcpy(newFirstElem,min);
                
        for(int k=i+1;k<numberOfRecords;k++)
        {
            pos=k*sizeOfRecord;
            read(file,a,sizeOfRecord);
            if(a[0]<min[0])
            {
                minPos=pos;
                strcpy(min,a);
            }
        }
         
        lseek(file,minPos,SEEK_SET);
        write(file,newFirstElem,sizeOfRecord);

        lseek(file,newFirstPos,SEEK_SET);
        write(file,min,sizeOfRecord); 
    }

    struct tms tmsEnd;
    clock_t end = times(&tmsEnd);

    char message[100];
    snprintf(message,sizeof(message),"Sorted %d records of size %d by sys functions",numberOfRecords,sizeOfRecord);
    saveTimesToFile(message,start,end,&tmsStart,&tmsEnd);

    free(a);
    free(newFirstElem);
    free(min);
    close(file);
}

void sort(char *fileName, int numberOfRecords, int sizeOfRecord, char *typeOfFunctions)
{
    if(strcmp(typeOfFunctions,"lib")==0)
    {
        sortLib(fileName,numberOfRecords,sizeOfRecord);
    }
    else if (strcmp(typeOfFunctions,"sys")==0)
    {
        sortSys(fileName,numberOfRecords,sizeOfRecord);
    }
}
void copy(char *fileNameFrom, char *fileNameTo, int numberOfRecords, int sizeOfBuffer,char *typeOfFunctions)
{
    if(strcmp(typeOfFunctions,"lib")==0)
    {
        char *a = calloc(sizeOfBuffer,sizeof(char));
        size_t numberOfCopied;

        struct tms tmsStart;
        clock_t start = times(&tmsStart);

        FILE *fileFrom = fopen(fileNameFrom,"r");
        FILE *fileTo = fopen(fileNameTo,"w");
               
        for(int i=0;i<numberOfRecords;i++)
        {
            numberOfCopied = fread(a,sizeof(char),sizeOfBuffer,fileFrom);
            fwrite(a,sizeof(char),numberOfCopied,fileTo);
        }

        struct tms tmsEnd;
        clock_t end = times(&tmsEnd);

        char message[100];
        snprintf(message,sizeof(message),"Copied %d records with buffer of size %d by lib functions",numberOfRecords,sizeOfBuffer);
        saveTimesToFile(message,start,end,&tmsStart,&tmsEnd);

        fclose(fileFrom);
        fclose(fileTo);
        free(a);
    }
    else if(strcmp(typeOfFunctions,"sys")==0)
    {
        int from,to;
        int bytesRead;
        char *a = calloc(sizeOfBuffer,sizeof(char));

        from = open(fileNameFrom, O_RDONLY);
        to = open(fileNameTo,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);

        struct tms tmsStart;
        clock_t start = times(&tmsStart);

        for(int i=0;i<numberOfRecords;i++)
        {
            bytesRead = read(from,a,sizeOfBuffer);
            write(to,a,bytesRead);
        }

        struct tms tmsEnd;
        clock_t end = times(&tmsEnd);

        char message[100];
        snprintf(message,sizeof(message),"Copied %d records with buffer of size %d by sys functions",numberOfRecords,sizeOfBuffer);
        saveTimesToFile(message,start,end,&tmsStart,&tmsEnd);

        free(a);
        close(from);
        close(to);
    }
}

int main(int argc, char **argv){
    srand(time(NULL));
    int numberOfRecords;
    int sizeOfRecord;
    char *fileNameFrom;
    char *fileNameTo;
    char *typeOfFunctions;
    

    f = fopen("wyniki.txt","a");
    
    if(argc>1)
    {
        if(strcmp(argv[1],"generate")==0)
        {
            if(argc>4)
            {
                fileNameTo = argv[2];
                numberOfRecords = atoi(argv[3]);
                sizeOfRecord = atoi(argv[4]);
                generate(fileNameTo,numberOfRecords,sizeOfRecord);
            }
        }
        else if(strcmp(argv[1],"sort")==0)
        {
            if(argc>5)
            {
                fileNameFrom = argv[2];
                numberOfRecords = atoi(argv[3]);
                sizeOfRecord = atoi(argv[4]);
                typeOfFunctions=argv[5];
                sort(fileNameFrom,numberOfRecords,sizeOfRecord,typeOfFunctions);
            }
        }
        else if(strcmp(argv[1],"copy")==0)
        {
            if(argc>6)
            {
                fileNameFrom = argv[2];
                fileNameTo = argv[3];
                numberOfRecords = atoi(argv[4]);
                sizeOfRecord = atoi(argv[5]);
                typeOfFunctions=argv[6];
                copy(fileNameFrom,fileNameTo,numberOfRecords,sizeOfRecord,typeOfFunctions);
            }
        }
    }
    fclose(f);
    return 0;
}