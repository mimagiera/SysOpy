#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

char *givenOp;
time_t givenTimeG;

int timeValid(time_t fileTime, char* operator, time_t time) {
	if(strcmp(operator, "=") == 0) {
		return fileTime == time;
	} else if (strcmp(operator, ">") == 0) {
		return fileTime > time;
	} else if (strcmp(operator, "<") == 0) {
		return fileTime < time;
	}
	return 0;
}
void showFile(const char* path, const struct stat* stat)
{
    printf("%s\t", path);
    if (S_ISREG(stat->st_mode))
		printf("zwykły plik\t");
	else if (S_ISDIR(stat->st_mode))
		printf("katalog\t");
	else if (S_ISCHR(stat->st_mode))
		printf("urządzenie znakowe\t");
	else if (S_ISBLK(stat->st_mode))
		printf("urzączenie blokowe\t");
	else if (S_ISFIFO(stat->st_mode))
		printf("potok nazwany\t");
	else if (S_ISLNK(stat->st_mode))
		printf("link symboliczny\t");
	else
		printf("soket\t");

	printf("%ld\t", stat->st_size);
	printf("%s\t",ctime(&stat->st_atime));
	printf("%s\n",ctime(&stat->st_mtime));
}
void find(char *path, char *operator, time_t time)
{
    DIR *dir;
    struct dirent *dirent;
    struct stat stat;
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
        sprintf(new_path, "%s/%s", path, dirent->d_name);
        if (lstat(new_path, &stat) == -1)
        {
            return;
        }
        if (timeValid(stat.st_mtime, operator, time))
        {
            showFile(new_path, &stat);
        }
        if(S_ISDIR(stat.st_mode))
        {
            find(new_path,operator,time);
        }
    }
}
static int findNftw(const char *path, const struct stat *stat, int typeflag, struct FTW *ftwbuf) {
	if (ftwbuf->level == 0) return 0;
	if (!timeValid(stat->st_mtime, givenOp, givenTimeG)) return 0;

	showFile(path, stat);

	return 0;
}
//4 argument okresla rodzaj wyszukiwania
//0 - Korzystajc z funkcji opendir, readdir oraz funkcji z rodziny stat 
//1 - Korzystajac z funkcji nftw
//domyslnie 0
int main(int argc, char **argv)
{    
    char *mode ="0";
    if(argc>3)
    { 
        char *path = argv[1];
        char *operator = argv[2];
        char *givenTime = argv[3];
        time_t time;
        struct tm tm;
        strptime(givenTime,"%Y-%m-%d %H:%M:%S",&tm);
        time = mktime(&tm);

        if(argc>4)
        {
            mode = argv[4];
        }
        if(strcmp(mode,"0")==0)
        {
            find(path,operator,time);
        }
        else
        {
            givenOp=operator;
            givenTimeG=time;
            nftw(path, findNftw, 15, FTW_PHYS);
        }
        
    }
    return 0;
}