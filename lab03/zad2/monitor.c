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
#define TIME_FORMAT_DATE "_%Y-%m-%d_%H-%M-%S"

int number_of_lines = 0;

char *read_file(char *file_name) {
	struct stat stat;
	lstat(file_name, &stat);

	FILE* file = fopen(file_name, "r");

	char *filebuffer = malloc(stat.st_size + 1);
	fread(filebuffer, 1, stat.st_size, file);

	filebuffer[stat.st_size] = '\0';

	fclose(file);

	return filebuffer;
}

char** get_lines_of_file(char *file_name)
{
    FILE *f = fopen(file_name,"r");
    size_t max_number_of_elements=20;
    char **lines = calloc(max_number_of_elements,sizeof(char*));
    char * line = NULL;
    int i = 0;
    size_t len = 0;
    ssize_t read;
    if(f==NULL)
    {
        printf("Could not open file %s\n",file_name);
        exit(1);
    }
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read-1]=='\n')
        {
            line[read-1]=0;
        }
        read=strlen(line);
        lines[i]=calloc(read,sizeof(char));
        strcpy(lines[i],line);
        i++;
    }

    number_of_lines=i;

    fclose(f);

    return lines;
}

void copy_file(char *path,struct stat *stat)
{
    char *file_name_date = malloc(strlen(path) + 30);
    strcpy(file_name_date, path);
    strftime(&file_name_date[strlen(path)], 30, TIME_FORMAT_DATE, localtime(&stat->st_mtime));

    pid_t child_pid = fork();
    if (child_pid==0)
    {
        execlp("cp", "cp", path, file_name_date, NULL);
    }
}
void save_to_file(char *path,struct stat *stat,char *content)
{
    char *file_name_date = malloc(strlen(path) + 30);
    strcpy(file_name_date, path);
    strftime(&file_name_date[strlen(path)], 30, TIME_FORMAT_DATE, localtime(&stat->st_mtime));

    FILE *backup = fopen(file_name_date, "w");
    fwrite(content, 1, strlen(content), backup);
    fclose(backup);

    free(content);
}
int watch_file(char *path,int time_of_sleep,int time_of_total_watch, int mode)
{
    struct stat stat;
    lstat(path, &stat);
    
    long last_modification_time = stat.st_mtime;
    int i =0;
    int counter_of_copies=0;
    char *content;
    if(mode ==0)
    {
        content=read_file(path);
    }
    else if(mode == 1)
    {
        copy_file(path,&stat);
        counter_of_copies++;
    }
    while(i*time_of_sleep<time_of_total_watch)
    {
        sleep(time_of_sleep);
        i++;
        lstat(path, &stat);
        if(stat.st_mtime!=last_modification_time)
        {
            last_modification_time=stat.st_mtime;
            counter_of_copies++;
            if(mode == 0)
            {
                save_to_file(path,&stat,content);
                content=read_file(path);
            }
            else if(mode == 1)
            {
                 copy_file(path,&stat);
            }
        }

    }
    return counter_of_copies;
}

void monitor(char *file_name,int time_of_total_watch,int mode)
{
    char **lines = get_lines_of_file(file_name);
    char *path;
    int time_of_sleep;
    char delim[] = " ";
    char *ptr;
    int number_of_copies=0;
    pid_t child_pid,pid;
    int status;
    for(int i=0;i<number_of_lines;i++)
    {
        ptr = strtok(lines[i], delim);
        path=ptr;
		ptr = strtok(NULL, delim);
        time_of_sleep=atoi(ptr);
        
        child_pid = fork();

        if(child_pid==0)
        {
            number_of_copies = watch_file(path,time_of_sleep,time_of_total_watch,mode);
            exit(number_of_copies);
        }
             
    }
    for(int i=0;i<number_of_lines;i++)
    {
        pid = wait(&status);
        printf("Proces %d utworzył %d kopii pliku\n",(int)pid,status/256);
    }
}

int main(int argc, char **argv)
{
    if(argc==4)
    {
        char *file_name = argv[1];
        int time_of_total_watch = atoi(argv[2]); 
        int mode = atoi(argv[3]);
        monitor(file_name,time_of_total_watch,mode);
    }

    return 0;
}