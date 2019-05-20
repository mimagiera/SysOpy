#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main()
{
    srand(time(0));
    int c = 20;
    char *file_name = "filter.txt";
    FILE* fp = fopen(file_name, "w");
    double *random = malloc(c*c*sizeof(double));
    double sum=0;
    for(int i =0;i<c*c;i++)
    {
        random[i] = rand();
        sum+=random[i];
    }
    for(int i =0;i<c*c;i++)
    {
        random[i] = random[i]/sum;
    }

    fprintf(fp, "%d\n", c);
    for(int i =0;i<c;i++)
    {
        for(int j = 0 ;j<c;j++)
        {
            fprintf(fp, "%f ", *(random+(i*c)+j));
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}