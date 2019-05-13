#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>
#include <unistd.h> 
#include <time.h>
#include "helper.h"

int shm_id, sem_id;
int queue_size, max_weight, truck_max_weight, number_of_loaders, fixed_number=0;
char *number_of_packages_per_loader;
struct queue *queue;
void prepare_queue()
{
    queue->rear = -1;
    queue->itemCount = 0;
    queue->queue_size = queue_size;
    queue->max_weight = max_weight;
    queue->front = 0;
    queue->current_weight = 0;
}

int get_queue_size()
{
    return 6*sizeof(int)+queue_size*sizeof(struct product);
}

void sig_handler(int signo)
{
  if (signo == SIGINT){
    shmctl(shm_id,IPC_RMID,NULL);
    semctl(sem_id, 0, IPC_RMID);
    printf("received SIGINT\n");
    exit(0);
  }
}

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
    shm_id = shmget(key,get_queue_size(),IPC_CREAT | IPC_EXCL | 0666);
    if(shm_id==-1)
    {
        printf("Cannot get shared memory id trucker\n");
        exit(4);
    }
    sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if(sem_id==-1)
    {
        printf("Cannot get semaphore id\n");
        exit(5);
    }
    union semun semun;
    semun.val=0;
    if(semctl(sem_id, 0, SETVAL, semun)==-1)
    {
       printf("Cannot set initial semaphore value\n");
       exit(6); 
    }
    queue = shmat(shm_id,NULL,0);
    if ( queue == (struct queue*)-1 ) {
        printf("*** shmat error (trucker) ***\n");
        exit(7);
    }
}
void spawn_loaders()
{
    int max=max_weight, min = 1, random_weight;
    char *weight = malloc(sizeof(char*));
    for(int i =0;i<number_of_loaders;i++)
    {
        random_weight = rand()%(max-min+1)+min;
        sprintf(weight, "%d", random_weight);
        if(fork()==0)
        {
            if(fixed_number)
                execl("./loader","./loader",weight,number_of_packages_per_loader,NULL);
            else 
                execl("./loader","./loader",weight,NULL);
        }
    }
}
int main(int argc, char **argv)
{
    if(argc!=5 && argc!=6)
    {
        printf("Bad number of arguments. Expected 4 or 5, got %d\n",argc-1);
        printf("Args: truck max weight, products line length, max weight of products on line, number of loaders\n");
        printf("Optional: number of packages to add by one loader\n");
        exit(1);
    }
    srand(time(0)); 
    truck_max_weight = atoi(argv[1]);
    queue_size = atoi(argv[2]);
    max_weight = atoi(argv[3]);
    number_of_loaders = atoi(argv[4]);
    if(argc==6)
    {
        number_of_packages_per_loader = argv[5];
        fixed_number = 1;
    }

    signal(SIGINT, sig_handler);

    prepare_memory();
    prepare_queue();

    int current_truck_weight = 0;
    struct product product;
    time_t rawtime;
    time ( &rawtime );

    printf("Time: %ld ",rawtime);
    printf("New truck arriving!\n");

    spawn_loaders();

    struct sembuf sembuf;
    sembuf.sem_num=0;
    sembuf.sem_op=-1; 
    semop(sem_id,&sembuf,1);    
    
    while(1)
    {  
        if(!isEmpty(queue))
        {
            if(peek(queue).val+current_truck_weight>truck_max_weight)
            {
                time ( &rawtime );
                printf("Time: %ld ",rawtime);
                printf("No more space in truck ");
                queue->current_weight = 0;
                current_truck_weight = 0;
                printf("New truck arriving!\n");
            }
            else
            {
                product = removeData(queue);
                current_truck_weight+=product.val;
                time ( &rawtime );
                printf("Time: %ld ",rawtime);
                printf("Pack is being loaded on truck, loader id: %d ", product.loader_id);
                printf("Weight of product: %d, Free: %d, Taken: %d, Time elapsed: %ld\n", product.val,truck_max_weight-current_truck_weight,current_truck_weight, rawtime-product.date_add_to_queue);
            }
        }
        else
        {
            time ( &rawtime );
            printf("Time: %ld ",rawtime);
            printf("Empty products line, waiting for products\n");
        }

        sembuf.sem_op=1; 
        semop(sem_id,&sembuf,1); 
        sembuf.sem_op=-1; 
        semop(sem_id,&sembuf,1);
    }
    
    sig_handler(SIGINT);
    return 0;
}