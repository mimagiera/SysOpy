#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
int number_of_threads, width, height, filter_size, max_gray;
int **file_to_filter;
int **new_file;
double **filter_matrix;
char *filter_type;
char magic_number_xd[10];

void intervealed_filter(int thread_number);
void block_filter(int x_from, int x_to);

double max(double a, double b)
{
    return a>b ? a : b;
}
int filter_function(int x, int y)
{
    double new_value = 0;
    int max_x, max_y;
    for(int i=0;i<filter_size;i++)
    {
        for(int j=0;j<filter_size;j++)
        {
            max_x = max(1, x - (int)ceil(filter_size/2) + i);
            max_y = max(1, y - (int)ceil(filter_size/2) + j);
            if(max_x >= width) max_x = width-1;
            if(max_y >= height) max_y = height-1;
            new_value+=(file_to_filter[max_x][max_y]*filter_matrix[i][j]);
        }
    }
    return (int)round(new_value);
}

void *filter(void *x_void_ptr)
{
    int thread_number= *((int *)x_void_ptr);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    if(strcmp(filter_type, "block")==0)
    {
        double from_ceil = (double)(thread_number*width)/(double)number_of_threads;
        double to_ceil = (double)((thread_number+1)*width)/(double)number_of_threads-1;
        int from = (int) ceil(from_ceil);
        int to = (int) ceil(to_ceil);
        block_filter(from, to);
    }
    else if(strcmp(filter_type, "interleaved")==0)
    {
        intervealed_filter(thread_number);
    }

    gettimeofday(&end_time, NULL);
    unsigned long elapsed = (end_time.tv_sec*1000000 + end_time.tv_usec) - (start_time.tv_sec*1000000 + start_time.tv_usec);

    pthread_exit((void *)elapsed);
}

void intervealed_filter(int thread_number)
{
    for(int i = thread_number; i<width;i+=number_of_threads)
    {
        for(int j=0;j<height;j++)
        {
            new_file[i][j] = filter_function(i, j);
        }
    }
}

void block_filter(int x_from, int x_to){
    for(int i = x_from;i<=x_to;i++)
    {
        for(int j=0;j<height;j++)
        {
            new_file[i][j] = filter_function(i, j);
        }
    }
}

void read_filter(char *filter_name)
{
    FILE* fp = fopen(filter_name, "r");
    fscanf(fp, "%d", &filter_size);

    filter_matrix = (double **)malloc(filter_size * sizeof(double *));
    for(int i =0 ;i<filter_size;i++)
    {
        filter_matrix[i] = (double *) malloc(filter_size * sizeof(double));
    }

    for(int i =0;i<filter_size;i++)
    {
        for(int j = 0 ;j<filter_size;j++)
        {
            fscanf(fp, "%lf", &filter_matrix[i][j]);
        }
    }
    fclose(fp);
}

void read_from_file(char *file_name)
{
    FILE* fp = fopen(file_name, "r");
    fscanf(fp, "%s", magic_number_xd);
    fscanf(fp, "%d %d", &width, &height);
    fscanf(fp, "%d", &max_gray);
    printf("%d\n",width);
    
    new_file = (int **)malloc(width * sizeof(int *));
    file_to_filter = (int **)malloc(width * sizeof(int *));

    for(int i =0 ;i<width;i++)
    {
        new_file[i] = (int *) malloc(height * sizeof(int));
        file_to_filter[i] = (int *) malloc(height * sizeof(int));
    }
    for(int i =0;i<width;i++)
    {
        for(int j = 0 ;j<height;j++)
        {
            fscanf(fp, "%d", &file_to_filter[i][j]);
        }
    }
    fclose(fp);
}
void save_new_file(char *file_name)
{
    FILE* fp = fopen(file_name, "w");
    fprintf(fp, "%s\n", magic_number_xd);
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", max_gray);
    for(int i =0;i<width;i++)
    {
        for(int j = 0 ;j<height;j++)
        {
            fprintf(fp, "%d ", new_file[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(int argc, char **argv)
{
    if(argc!=6)
    {
        printf("Not enough arguments! Expected 5, got %d\n", argc-1);
        exit(1);
    }
    number_of_threads = atoi(argv[1]);
    filter_type = argv[2];
    char *file_name_to_filter = argv[3];
    char *filter_name = argv[4];
    char *file_name_to_save = argv[5];

    read_from_file(file_name_to_filter);
    read_filter(filter_name);

    pthread_t *thread_ids = malloc(number_of_threads*sizeof(pthread_t));
    int *values = malloc(number_of_threads*sizeof(pthread_t));
    unsigned long *times = malloc(number_of_threads*sizeof(unsigned long));

    printf("Number of threads: %d\n", number_of_threads);
    printf("Type of filtering: %s\n", filter_type);
    printf("Filter size: %d\n", filter_size);
    printf("Size of image: %d %d\n", width, height);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    for(int i =0;i<number_of_threads;i++)
    {
        values[i]=i;
        pthread_create(&thread_ids[i], NULL, filter, &values[i]);
    }
    void *elapsed;
    for(int i =0;i<number_of_threads;i++)
    {
        pthread_join(thread_ids[i], &elapsed);
        times[i]=(unsigned long)elapsed;
    } 
    gettimeofday(&end_time, NULL);
    for(int i =0;i<number_of_threads;i++)
    {
        printf("Thread: %d time elapsed: %lu\n", i, times[i]);
    } 
    unsigned long elapsed_total = (end_time.tv_sec*1000000 + end_time.tv_usec) - (start_time.tv_sec*1000000 + start_time.tv_usec);
    printf("Total time: %lu\n", elapsed_total);

    save_new_file(file_name_to_save);

    //free memory
    for(int i =0 ;i<width;i++)
    {
        free(new_file[i]);
        free(file_to_filter[i]);
    }
    free(new_file);
    free(file_to_filter);
    for(int i =0 ;i<filter_size;i++)
    {
        free(filter_matrix[i]);
    }
    free(filter_matrix);
    free(thread_ids);
    free(values);
    free(times);

    return 0;
}