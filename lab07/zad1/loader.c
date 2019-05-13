#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>
#include <time.h>
#include "helper.h"

int shm_id, sem_id;
int weight_of_product;
struct queue *queue;
void prepare_memory()
{
    char *home_dir = getenv("HOME");
    if(!home_dir)
    {
        printf("Cannot get home dir path\n");
        exit(2);
    }
    key_t key = ftok(home_dir,2137);
    if((int)key == -1)
    {
        printf("Cannot get key\n");
        exit(3);
    }

    shm_id = shmget(key,0,0);
    if(shm_id==-1)
    {
        printf("Cannot get shared memory id trucker\n");
        exit(4);
    }
    sem_id = semget(key, 0,0);
    if(sem_id==-1)
    {
        printf("Cannot get semaphore id\n");
        exit(5);
    }
    queue = shmat(shm_id,NULL,0);
    if ( queue == (struct queue*)-1 ) {
        printf("*** shmat error (loader) ***\n");
        exit(6);
    }

}
int main(int argc, char **argv)
{
    if(argc!=2 && argc!=3)
    {
        printf("Bad number of arguments\n");
        exit(1);
    }
    int number_of_packages_per_loader = 1;
    weight_of_product = atoi(argv[1]);
    if(argc==3)
    {
        number_of_packages_per_loader = atoi(argv[2]);
    }
    prepare_memory();

    struct product product;
    product.val = weight_of_product;
    struct sembuf sembuf;
    sembuf.sem_num=0;
    sembuf.sem_op=1;
    time_t rawtime;
    int packages_loaded=0;
    while(packages_loaded < number_of_packages_per_loader)
    {
        if(!isFull(queue))
        {
            time ( &rawtime );
            printf("Time: %ld ",rawtime);
            product.date_add_to_queue = rawtime;
            product.loader_id = getpid();                
            if(insert(product, queue))
            {
                printf("Insert to queue, weight: %d, loader id: %d\n", weight_of_product, getpid());
                if(argc==3)
                {
                    packages_loaded++;
                }
            }
            else
            {
                printf("Package too heavy, Weight: %d, weight left: %d\n", product.val, queue->max_weight-queue->current_weight);
            }         
        }
        else
        {
            time ( &rawtime );
            printf("Time: %ld ",rawtime);
            printf("Waiting for space in queue, loader id: %d\n", getpid());
        }
        sembuf.sem_op=1; 
        semop(sem_id,&sembuf,1); 
        sembuf.sem_op=-1; 
        semop(sem_id,&sembuf,1);
    }
    printf("Loader with id: %d finished. Loaded %d packages\n", getpid(), number_of_packages_per_loader);
    sembuf.sem_op=1; 
    semop(sem_id,&sembuf,1);
    return 0;
}