#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>


char *givenOp;
time_t givenTimeG;

void find(char *path,char *relative_path)
{
    DIR *dir;
    struct dirent *dirent;
    struct stat stat;
    pid_t child_pid;
    if(!(dir=opendir(path)))
    {
        return;
    }
    while((dirent = readdir(dir)))
    {
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
        {
            continue;
        }
        char* new_path = malloc(strlen(path) + strlen(dirent->d_name) + 2);
        char* new_relative_path = malloc(strlen(relative_path) + strlen(dirent->d_name) + 2);
        sprintf(new_path, "%s/%s", path, dirent->d_name);
        sprintf(new_relative_path, "%s/%s", relative_path, dirent->d_name);
        if (lstat(new_path, &stat) == -1)
        {
            return;
        }
        if(S_ISDIR(stat.st_mode))
        {
            child_pid = fork();
            if(child_pid!=0)
            {
                wait(0);
                find(new_path,new_relative_path);
            }
            else
            {
                printf("%s\t%d\n","PID:",(int)getpid());
                printf("%s\t%s\n","Relative path:",new_relative_path);
                execl("/bin/ls", "ls", "-l",new_path,NULL);
                exit(0);
            } 
            
        }
    }
}

int main(int argc, char **argv)
{    
    char *path;
    char *relative_path="..";
     if(argc>1)
    { 
        path=argv[1];
        find(path,relative_path);
    } 
    
    return 0;
}