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
int finish =0;
int stopped=0;

typedef struct {
	char *file_name;
	pid_t pid;
	int interval;
	int stopped;
} child;

typedef struct {
	child *list;
	int count;
} children;

static void sig_child_handler(int signo) {
	if (signo == SIGUSR1 || signo == SIGINT) {
        finish=1;
	}
    else if(signo == SIGUSR2)
    {
        if(stopped==1) stopped=0;
        else stopped=1;
    }
}

void list(children *children) {

	for (int i = 0; i < children->count; i++) {
		printf("Process %d is monitoring '%s' every %d seconds\n",
			   children->list[i].pid,
			   children->list[i].file_name,
			   children->list[i].interval
			);
	}
}

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
int watch_file(char *path,int time_of_sleep)
{
    struct stat stat;
    lstat(path, &stat);
    
    long last_modification_time = stat.st_mtime;
    int counter_of_copies=0;
    char *content;
    content=read_file(path);

    while(1)
    {
        sleep(time_of_sleep);
        if(finish) break;
        if (stopped) continue;
        lstat(path, &stat);
        if(stat.st_mtime!=last_modification_time)
        {
            last_modification_time=stat.st_mtime;
            counter_of_copies++;
            save_to_file(path,&stat,content);
            content=read_file(path);
        }
    }
    return counter_of_copies;
}

child *find_child(children *children, pid_t child_pid)
{
    for (int i = 0; i < children->count; i++)
		if (children->list[i].pid == child_pid)
        return &children->list[i];
        
    return NULL;
}

void stop_pid(children *children, pid_t child_pid)
{
    child *child = find_child(children,child_pid);
    if(child->stopped)
    {
        printf("Child with PID %d was alreadry stopped\n", child->pid);
        return;
    }
    else
    {
        printf("Child with PID %d stopped\n", child->pid);
        child->stopped = 1;
        kill(child->pid, SIGUSR2);
    }
}
void start_pid(children *children, pid_t child_pid) {
	child *child = find_child(children, child_pid);
	if (!child->stopped) 
    {
		printf("Child with PID %d is already running\n", child->pid);
		return;
    }
    else
    {
        printf("Child with PID %d restarted\n", child->pid);
        child->stopped = 0;
        kill(child->pid, SIGUSR2);
    }
    
}

void stop_all(children *children) {
    for (int i = 0; i < children->count; i++)
    {
        if(!children->list[i].stopped)
        {
            printf("Child with PID %d stopped\n", children->list[i].pid);
            kill(children->list[i].pid, SIGUSR2);
            children->list[i].stopped=1;
        }
    }
}
void start_all(children *children) {
    for (int i = 0; i < children->count; i++)
    {
        if(children->list[i].stopped)
        {
            printf("Child with PID %d started\n", children->list[i].pid);
            kill(children->list[i].pid, SIGUSR2);
            children->list[i].stopped=0;
        }
    }
}

void monitor(char *file_name)
{
    char **lines = get_lines_of_file(file_name);
    char *path;
    int time_of_sleep;
    char delim[] = " ";
    char *ptr;
    int number_of_copies=0;
    pid_t child_pid;

    child *children_list = calloc(number_of_lines, sizeof(child));
    int child_count=0;


    for(int i=0;i<number_of_lines;i++)
    {

        ptr = strtok(lines[i], delim);
        path=ptr;
		ptr = strtok(NULL, delim);
        time_of_sleep=atoi(ptr);
        
        child_pid = fork();

        if(child_pid==0)
        {
            struct sigaction act;
            act.sa_handler = sig_child_handler;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;

            sigaction(SIGUSR2, &act, NULL);
            sigaction(SIGUSR1, &act, NULL); 

            number_of_copies = watch_file(path,time_of_sleep);
            exit(number_of_copies);
        }
        else
        {
            child *child = &children_list[child_count];
            child->file_name = malloc(strlen(path) + 1);
            strcpy(child->file_name, file_name);
            child->pid = child_pid;
			child->interval = time_of_sleep;
            child_count++;
        }
        
             
    }

    children children;
    children.list = children_list;
    children.count = child_count;

    char command[15];
    list(&children);
    while(1)
    {
        fgets(command, 15, stdin);
        if (strcmp(command, "LIST\n") == 0) {
            list(&children);
        }
        else if (strcmp(command, "END\n") == 0 || finish) {
			break;
        }
        else if (strcmp(command, "STOP ALL\n") == 0) {
			stop_all(&children);
        } 
        else if (strcmp(command, "START ALL\n") == 0) {
			start_all(&children);
        }
        else if (strncmp(command, "STOP ", 5) == 0) {
			int pid = atoi(command+5);
			if (pid > 0) stop_pid(&children, pid);
            else printf("Invalid pid '%s'", command+5);
        } 
        else if (strncmp(command, "START ", 6) == 0) {
			int pid = atoi(command+6);
			if (pid > 0) start_pid(&children, pid);
			else printf("Invalid pid '%s'", command+6);
        }
        
         
    }

    for(int i=0;i<child_count;i++)
    {
        int status;
		kill(children.list[i].pid, SIGUSR1);
		pid_t child_pid = waitpid(children.list[i].pid, &status, 0);	
        printf("Process %d has made %d backups\n", (int) child_pid, WEXITSTATUS(status));
        
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, sig_child_handler);
    if(argc==2)
    {
        char *file_name = argv[1];
        monitor(file_name);
    }

    return 0;
}