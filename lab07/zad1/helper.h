#ifndef TASMA_H
#define TASMA_H
union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };
#define MAX_QUEUE_LENGTH 100
struct product{
    int val;
    long date_add_to_queue;
    pid_t loader_id;
};

struct queue{
   struct product products_line[MAX_QUEUE_LENGTH];
   int front;
   int rear;
   int itemCount;
   int queue_size;
   int max_weight;
   int current_weight;
};

struct product peek(struct queue *queue) {
   return queue->products_line[queue->front];
}

int isEmpty(struct queue *queue) {
   return (queue->itemCount == 0) ? 1 : 0;
}

int isFull(struct queue *queue) {
   return (queue->itemCount == queue->queue_size) ? 1 : 0;
}

int size(struct queue *queue) {
   return queue->itemCount;
}  

int insert(struct product data, struct queue *queue) {

   if(!isFull(queue)) {
	
      if(queue->rear == queue->queue_size-1) {
         queue->rear = -1;            
      }       
      int weight_after = queue->current_weight+data.val;
      if(weight_after>queue->max_weight)
      {
          //printf("Package is too heavy\n");
          return 0;
      }
      queue->products_line[++(queue->rear)] = data;
      queue->itemCount++;
      queue->current_weight = weight_after;
      return 1;
   }
   return 0;
}

struct product removeData(struct queue *queue) {
   struct product data = queue->products_line[queue->front++];
	
   if(queue->front == queue->queue_size) {
      queue->front = 0;
   }
	queue->current_weight-=data.val;
   queue->itemCount--;
   return data;  
}

#endif