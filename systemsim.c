#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>


// list implementation start
typedef struct pcb{
pid_t pid;
pthread_t threadId;
int state;
int nextCPUBurst;
int remainingCPUBurst;
int numberOfCPUExecuted;
int timeSpentinReadyList;
int IOdone1;
int IOdone2;
int startTime;
int finishTime;
int totalCPUExecution;

}PCB;





typedef struct Node{
    struct Node *next;
    PCB data;
}Node;

typedef struct List{
    Node *head;
    Node *tail;
}List;



PCB emptyPCB()
{
     PCB pcb;
     pcb.pid =-1;
     pcb.nextCPUBurst =-1;
     return pcb;
}


List* createList(){
    List* list = malloc(sizeof(List));
    list->head=NULL;
    list->tail=NULL;

    return list;
}

void addToHead(List** list, PCB data){
    Node* node = malloc(sizeof(Node));
    node->data=data;
    node->next=NULL;

    if((*list)->head==NULL){
        (*list)->head= node;
        (*list)->tail=node;
    }else{
        node->next= (*list)->head;
        (*list)->head=node;
    }

}

void addToTail(List** list, PCB data){
    Node* node = malloc(sizeof(Node));
    node->data=data;
    node->next=NULL;

    if((*list)->head==NULL){
        (*list)->head= node;
        (*list)->tail=node;
    }else{
        (*list)->tail->next=node;
        (*list)->tail=node;
    }

}
int getNodeNumber(List* list)
{
    if(list->head==NULL){
        return 0;
    }
    int result = 0;
    Node* node = list->head;
    while(node!=NULL){
        
            result++;
            node = node->next;
            
        }
        return result;
}
int getSleepingNodeNumber(List* list)
{
    if(list->head==NULL){
        return 0;
    }
    int result = 0;
    Node* node = list->head;
    while(node!=NULL){
            if( node->data.state == 3)
            {
            result++;
            }
            node = node->next;
            
        }
        return result;
}

int findIndexFromPCB ( List* list, PCB data )
{
    
    Node* node = list->head;
    if ( node == NULL)
    {
        return -1;
    }
    int index = 1;
    while( node != NULL )
    {
        if( node->data.pid == data.pid)
        {
            return index;
        }

        node = node->next;
        index++;
    }
    return -1;


}

PCB searchMin(List* list, int* minIndexPtr){
    if(list->head==NULL){
        *minIndexPtr=-1;
        return emptyPCB();
    }

    Node* node = list->head;
    PCB min = list->head->data;
    int minIndex = 1;
    int index = 1;
    while(node!=NULL){
        if(min.nextCPUBurst >node->data.nextCPUBurst){
            min = node->data;
            minIndex = index;
        }

        index++;
        node = node->next;

    }
    *minIndexPtr=minIndex;
    return min;

}

PCB removeTail(List** list){
    PCB result ;
    if((*list)->head==NULL){
         result=emptyPCB();
    }else if((*list)->head==(*list)->tail){
        Node* temp = (*list)->head;
        (*list)->head=NULL;
        (*list)->tail=NULL;
        result=temp->data;
        free(temp);

    }else{
        Node* temp = (*list)->tail;

        Node* cur = (*list)->head;
        Node* back = NULL;

        while(cur->next!=NULL){
            back=cur;
            cur=cur->next;
        }
        back->next=NULL;

        (*list)->tail=back;
        result=temp->data;
        free(temp);
    }
    return result;
}

void printList(List* list){
    Node* node= list->head;
    if(node==NULL){
        printf("List is empty ");
    }
    while(node!=NULL){
        printf("%d -> ",node->data.pid);
        node=node->next;
    }

    printf("\n");

}

int listSize(List* list){
    Node* node= list->head;
    if(node==NULL){
        return 0;
    }
    int counter = 0;
    while(node!=NULL){
        counter++;
        node=node->next;
    }
    
    return counter;
}

PCB getData(List* list,int index ){
    Node* node = list->head;
    int size = listSize(list);
    if(node==NULL|| index<1||size<index){
        return emptyPCB();
    }
    
    
    for(int i=1;i<index;i++){
         node=node->next;
    }
    
    return node->data;
 
}

PCB getTailData(List* list){
    Node* node= list->tail;
    if(node==NULL){
        return emptyPCB();
    }
    return node->data;

}

void setPCBState(List** list,int index,int state ){
    Node* node = (*list)->head;
    int size = listSize((*list));
    if(node==NULL|| index<1||size<index){
        return;
    }
    
    
    for(int i=1;i<index;i++){
         node=node->next;
    }
    
    node->data.state = state;
 
}

PCB getHeadData(List* list){
    Node* node= list->head;
    if(node==NULL){
        return emptyPCB();
    }
    return node->data;
}

PCB removeHead(List** list){
    Node* del =(*list)->head;
    if(del==NULL){
        return emptyPCB();
    }else if((*list)->head==(*list)->tail){
            (*list)->head=NULL;
            (*list)->tail=NULL;
    }else{
        (*list)->head=del->next;
    }
    PCB result = del->data;
    free(del);    
    return result;
}

PCB deleteWithIndex(List** list, int index){
    int size = listSize(*list);
    if(index<1 || (*list)->head==NULL||size<index){
            return emptyPCB();
    }
    Node* del = (*list)->head;
    Node* back = NULL;

    for (int i=1;i<index;i++){
        back = del;
        del = del->next;
    }

    if(del==(*list)->head){
        return removeHead(list);

    }else if(del==(*list)->tail){
        return removeTail(list);

    }else{
        back->next = del->next;
        PCB result = del->data;
        free(del);
        return result;
    }

}

void enqueue(List** list, PCB data){
    addToTail(list,data);
}
PCB dequeue(List** list){
    
    return removeHead(list);
    
}

PCB getAndDeleteSmallest(List** list){
     int index = -1;
     searchMin(*list,&index);
     return deleteWithIndex(list,index);
}

void deleteList(List** listptr){
    
     while((*listptr)->head != NULL){
        deleteWithIndex(listptr,1);
    }
    free((*listptr));
    
}

// list implementation end




// main code start
pthread_mutex_t mutexAkin;
pthread_mutex_t mutexHakan;
pthread_mutex_t mutexUyan;
pthread_mutex_t mutexRQ;
pthread_mutex_t mutexThreadNumber;
pthread_mutex_t mutexNodeForCPU;
pthread_mutex_t mutexSelectedPid;

pthread_mutex_t mutexIO1;

pthread_mutex_t mutexIO2;

pthread_cond_t condQueue;
pthread_cond_t device1Cond;
pthread_cond_t device2Cond;

pthread_cond_t schedulerWakeUp;
pthread_cond_t    rrCond;
pthread_cond_t nodeSelectedForCPU;


int threadsInTheSystem = 0;
int generatedThreadNumber = 0;
int isRecentlyAddedtoRQ = 0;

int selectedToCPU = -1;
int finishCPU = 0;

List *readyQueue;
int sleepingNumber = 0;
PCB cpu;
int selectedPid = -1;
int threadsSleepingOnIO1 = 0;
int threadsSleepingOnIO2 = 0;

char *ALG;
char *Q;
int Qint;
int T1;
int T2;
char *burst_dist;
int burstlen;
int min_burst;
int max_burst;
float p0;
float p1;
float p2;
float pg;
int MAXP;
int ALLP;
int OUTMODE;

int fixedCPUBurst;

int displayingArr[1000][8];

struct timeval startTime;


void *processThreads(void *arg)
{

  // Initialize
  struct timeval currentTime;
int rqWait = 0;
int waitR = 0;
  while (1)
  {
    
    pthread_mutex_lock(&mutexRQ);

    
    
    gettimeofday(&currentTime, NULL);
    if (displayingArr[((PCB *)arg)->pid - 1][0] == -1)
    {
      displayingArr[((PCB *)arg)->pid - 1][0] = (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
    }
    waitR = (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
    

    
    enqueue(&readyQueue, *(PCB *)arg);
    if (OUTMODE == 3)
    {
      printf("Process Thread %d added to RQ \n", ((PCB *)arg)->pid);
      printList(readyQueue);
    }
    pthread_mutex_unlock(&mutexRQ);
   
    
    pthread_cond_signal(&schedulerWakeUp);
    pthread_mutex_lock(&mutexHakan);

   
    while (selectedPid != ((PCB *)arg)->pid)
    {
   
      pthread_cond_wait(&nodeSelectedForCPU, &mutexHakan);
      

      int ind;

      ind = findIndexFromPCB(readyQueue,(*(PCB *)arg) );
     
      if( getData(readyQueue,ind).state == 1)
      {
        break;
      }

      
    }

    pthread_mutex_unlock(&mutexHakan);
  
      pthread_mutex_lock(&mutexRQ);
     
      int index;

      if ((strcmp(ALG, "FCFS") == 0) || (strcmp(ALG, "RR") == 0))
      {
        dequeue(&readyQueue);
        if (OUTMODE == 3)
        {
          printf("Process Thread at the head removed\n");
        }
      }

      else if ((strcmp(ALG, "SJF") == 0))
      {
        index = findIndexFromPCB(readyQueue, *((PCB *)arg));

        if (OUTMODE == 3)
        {
          printf("Process Thread %d removed\n", ((PCB *)arg)->pid);
        }
        deleteWithIndex(&readyQueue, index);
      }

      if (OUTMODE == 3)
      {
        printList(readyQueue);
      }
      pthread_mutex_unlock(&mutexRQ);


      double rndm;
      if (strcmp(ALG, "FCFS") == 0)
      {

        pthread_mutex_lock(&mutexNodeForCPU);

        cpu = (*(PCB *)arg);

        if (OUTMODE == 3)
        {
          printf("Process Thread  %d is running  on CPU\n", (*(PCB *)arg).pid);
        }
        gettimeofday(&currentTime, NULL);
        rqWait = rqWait -waitR + (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
        if (OUTMODE == 2)
        {
          printf("%d  %d RUNNING \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
        }

     
        usleep(cpu.nextCPUBurst * 1000);


        displayingArr[((PCB *)arg)->pid - 1][5]++;
        displayingArr[((PCB *)arg)->pid - 1][2]+=cpu.nextCPUBurst ;

        cpu = emptyPCB();

        if (OUTMODE == 3)
        {
          printf("Process Thread  %d left CPU \n", (*(PCB *)arg).pid);
        }
        pthread_mutex_unlock(&mutexNodeForCPU);
       
        pthread_cond_signal(&schedulerWakeUp);
        

        rndm = (float)rand() / (float)RAND_MAX;
        
        if (rndm < 1.0 - p2 - p1)
        {
          pthread_mutex_lock(&mutexThreadNumber);
          threadsInTheSystem--;

          pthread_mutex_unlock(&mutexThreadNumber);
          if (OUTMODE == 3)
          {
            printf("Process thread %d is being terminated\n", ((PCB *)arg)->pid);
          }
          if (threadsInTheSystem == 0)
          {
            pthread_cond_signal(&schedulerWakeUp);
          }

          gettimeofday(&currentTime, NULL);
          displayingArr[((PCB *)arg)->pid - 1][1] = (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
          displayingArr[((PCB *)arg)->pid - 1][4] = displayingArr[((PCB *)arg)->pid - 1][1] - displayingArr[((PCB *)arg)->pid - 1][0];
          displayingArr[((PCB *)arg)->pid - 1][3] = displayingArr[((PCB *)arg)->pid - 1][3] + rqWait;
          pthread_exit(NULL);
        }
        else if (rndm >= 1.0 - p1 - p2 && rndm < 1.0 - p2)
        {
      
          displayingArr[((PCB *)arg)->pid - 1][6]++;
          if (OUTMODE == 3)
          {
            printf("Process thread %d is going IO1\n", ((PCB *)arg)->pid);
          }
          pthread_mutex_lock(&mutexIO1);
          while (threadsSleepingOnIO1 > 0)
          {

            pthread_cond_wait(&device1Cond, &mutexIO1);
          }

          threadsSleepingOnIO1++;
          pthread_mutex_unlock(&mutexIO1);

          if (OUTMODE == 3)
          {
            printf("Process Thread  %d is using device 1\n", (*(PCB *)arg).pid);
          }
          gettimeofday(&currentTime, NULL);
          if (OUTMODE == 2)
          {
            printf("%d  %d USING DEVICE1 \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
          }
          usleep(T1 * 1000);

          pthread_mutex_lock(&mutexIO1);
          threadsSleepingOnIO1--;
          pthread_mutex_unlock(&mutexIO1);

          pthread_cond_signal(&device1Cond);
          if (OUTMODE == 3)
          {
            printf("Process thread %d is coming from IO1\n", ((PCB *)arg)->pid);
          }
        }

        else if (1.0 > rndm && rndm >= 1.0 - p2)
        {
          displayingArr[((PCB *)arg)->pid - 1][7]++;
          if (OUTMODE == 3)
          {
            printf("Process thread %d is going IO2\n", ((PCB *)arg)->pid);
          }
          pthread_mutex_lock(&mutexIO2);
          while (threadsSleepingOnIO2 > 0)
          {

            pthread_cond_wait(&device2Cond, &mutexIO2);
          }

          threadsSleepingOnIO2++;
          pthread_mutex_unlock(&mutexIO2);
  
          if (OUTMODE == 3)
          {
            printf("Process Thread  %d is using device 2\n", (*(PCB *)arg).pid);
          }

          gettimeofday(&currentTime, NULL);
          if (OUTMODE == 2)
          {
            printf("%d  %d USING DEVICE2 \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
          }
          usleep(T2 * 1000);

          pthread_mutex_lock(&mutexIO2);
          threadsSleepingOnIO2--;
          pthread_mutex_unlock(&mutexIO2);

          pthread_cond_signal(&device2Cond);
          if (OUTMODE == 3)
          {
            printf("Process thread %d is coming from IO2\n", ((PCB *)arg)->pid);
          }
        }
      }
      // break;

      else if (strcmp(ALG, "SJF") == 0) 
      {
        pthread_mutex_lock(&mutexNodeForCPU);
        cpu = (*(PCB *)arg);

        if (OUTMODE == 3)
        {
          printf("Process Thread  %d is running  on CPU\n", (*(PCB *)arg).pid);
        }
        gettimeofday(&currentTime, NULL);
        if (OUTMODE == 2)
        {
          printf("%d  %d RUNNING \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
        }
        rqWait = rqWait -waitR + (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
        usleep(cpu.nextCPUBurst * 1000);
 

        displayingArr[((PCB *)arg)->pid - 1][5]++;
        displayingArr[((PCB *)arg)->pid - 1][2]+=cpu.nextCPUBurst ;
        
        cpu = emptyPCB();

   
        pthread_mutex_unlock(&mutexNodeForCPU);
      

        pthread_cond_signal(&schedulerWakeUp);
       

        rndm = (float)rand() / (float)RAND_MAX;
      
        if (rndm < 1.0 - p2 - p1)
        {
          pthread_mutex_lock(&mutexThreadNumber);
          threadsInTheSystem--;

          pthread_mutex_unlock(&mutexThreadNumber);
          if (OUTMODE == 3)
          {
            printf("Process thread %d is being terminated\n", ((PCB *)arg)->pid);
          }
          if (threadsInTheSystem == 0)
          {
            pthread_cond_signal(&schedulerWakeUp);
          }
          gettimeofday(&currentTime, NULL);

          displayingArr[((PCB *)arg)->pid - 1][1] = (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
          displayingArr[((PCB *)arg)->pid - 1][4] = displayingArr[((PCB *)arg)->pid - 1][1] - displayingArr[((PCB *)arg)->pid - 1][0];
          displayingArr[((PCB *)arg)->pid - 1][3] = displayingArr[((PCB *)arg)->pid - 1][3] + rqWait;
          pthread_exit(NULL);
        }
        else if (rndm >= 1.0 - p2 - p1 && rndm <= 1.0 - p2)
        {
          displayingArr[((PCB *)arg)->pid - 1][6]++;
          
          if (OUTMODE == 3)
          {
            printf("Process thread %d is going IO1\n", ((PCB *)arg)->pid);
          }
          pthread_mutex_lock(&mutexIO1);
          while (threadsSleepingOnIO1 > 0)
          {

            pthread_cond_wait(&device1Cond, &mutexIO1);
          }

          threadsSleepingOnIO1++;
          pthread_mutex_unlock(&mutexIO1);
        
          if (OUTMODE == 3)
          {
            printf("Process Thread  %d is using device 1\n", (*(PCB *)arg).pid);
          }
          gettimeofday(&currentTime, NULL);
          if (OUTMODE == 2)
          {
            printf("%d  %d USING DEVICE1 \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
          }
          usleep(T1 * 1000);

          pthread_mutex_lock(&mutexIO1);
          threadsSleepingOnIO1--;
          pthread_mutex_unlock(&mutexIO1);

          pthread_cond_signal(&device1Cond);
          if (OUTMODE == 3)
          {
            printf("Process thread %d is coming from IO1\n", ((PCB *)arg)->pid);
          }
        }

        else if (1.0 > rndm && rndm >= 1.0 - p2)
        {
          displayingArr[((PCB *)arg)->pid - 1][7]++;
          if (OUTMODE == 3)
          {
            printf("Process thread %d is going IO2\n", ((PCB *)arg)->pid);
          }
          pthread_mutex_lock(&mutexIO2);
          while (threadsSleepingOnIO2 > 0)
          {

            pthread_cond_wait(&device2Cond, &mutexIO2);
          }

          threadsSleepingOnIO2++;
          pthread_mutex_unlock(&mutexIO2);
      
          if (OUTMODE == 3)
          {
            printf("Process Thread  %d is using device 2\n", (*(PCB *)arg).pid);
          }
          gettimeofday(&currentTime, NULL);
          if (OUTMODE == 2)
          {
            printf("%d  %d USING DEVICE2 \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
          }
          usleep(T2 * 1000);

          pthread_mutex_lock(&mutexIO2);
          threadsSleepingOnIO2--;
          pthread_mutex_unlock(&mutexIO2);

          pthread_cond_signal(&device2Cond);
          if (OUTMODE == 3)
          {
            printf("Process thread %d is coming from IO2\n", ((PCB *)arg)->pid);
          }
        }
      }

      else if (strcmp(ALG, "RR") == 0) 
      {
        pthread_mutex_lock(&mutexNodeForCPU);
        cpu = (*(PCB *)arg);

        if (OUTMODE == 3)
        {
          printf("Process Thread  %d is running  on CPU\n", (*(PCB *)arg).pid);
        }
        gettimeofday(&currentTime, NULL);
        if (OUTMODE == 2)
        {
          printf("%d  %d RUNNING \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
        }

        rqWait = rqWait -waitR + (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;


        if ( Qint > (*(PCB *)arg).remainingCPUBurst )
        {
          usleep( (*(PCB *)arg).remainingCPUBurst * 1000);
          displayingArr[((PCB *)arg)->pid - 1][2]+= (*(PCB *)arg).remainingCPUBurst ;
          (*(PCB *)arg).remainingCPUBurst = 0;
          

        }
        else
        {
          usleep(Qint * 1000); 
          (*(PCB *)arg).remainingCPUBurst = (*(PCB *)arg).remainingCPUBurst - Qint;
          displayingArr[((PCB *)arg)->pid - 1][2]+=Qint ;
        }
        
        

        cpu = emptyPCB();

        pthread_mutex_unlock(&mutexNodeForCPU);
 
        
        

        ((PCB *)arg)->state = 3;
        selectedPid = -1;
        pthread_cond_signal(&schedulerWakeUp);

        

        if ((*(PCB *)arg).remainingCPUBurst <= 0)

        {

          displayingArr[((PCB *)arg)->pid - 1][5]++;

          rndm = (float)rand() / (float)RAND_MAX;

       
          if (rndm < 1.0 - p2 - p1)
          {
            pthread_mutex_lock(&mutexThreadNumber);
            threadsInTheSystem--;

            pthread_mutex_unlock(&mutexThreadNumber);
            if (OUTMODE == 3)
            {
              printf("Process thread %d is being terminated\n", ((PCB *)arg)->pid);
            }
            if (threadsInTheSystem == 0)
            {
              pthread_cond_signal(&schedulerWakeUp);
            }
            gettimeofday(&currentTime, NULL);
            displayingArr[((PCB *)arg)->pid - 1][1] = (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000;
            displayingArr[((PCB *)arg)->pid - 1][4] = displayingArr[((PCB *)arg)->pid - 1][1] - displayingArr[((PCB *)arg)->pid - 1][0];
            displayingArr[((PCB *)arg)->pid - 1][3] = displayingArr[((PCB *)arg)->pid - 1][3] + rqWait;
            pthread_exit(NULL);
          }
          else if (rndm >= 1.0 - p2 - p1 && rndm < 1.0 - p2)
          {
            /* code */
            displayingArr[((PCB *)arg)->pid - 1][6]++;
            if (OUTMODE == 3)
            {
              printf("Process thread %d is going IO1\n", ((PCB *)arg)->pid);
            }
            pthread_mutex_lock(&mutexIO1);
            while (threadsSleepingOnIO1 > 0)
            {

              pthread_cond_wait(&device1Cond, &mutexIO1);
            }

            threadsSleepingOnIO1++;
            pthread_mutex_unlock(&mutexIO1);
       //     (*(PCB *)arg).state = 1; // waiting
            if (OUTMODE == 3)
            {
              printf("Process Thread  %d is using device 1\n", (*(PCB *)arg).pid);
            }
            gettimeofday(&currentTime, NULL);
            if (OUTMODE == 2)
            {
              printf("%d  %d USING DEVICE1 \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
            }
            usleep(T1 * 1000);

            pthread_mutex_lock(&mutexIO1);
            threadsSleepingOnIO1--;
            pthread_mutex_unlock(&mutexIO1);

            pthread_cond_signal(&device1Cond);
            if (OUTMODE == 3)
            {
              printf("Process thread %d is coming from IO1\n", ((PCB *)arg)->pid);
            }
            ((PCB *)arg)->remainingCPUBurst = ((PCB *)arg)->nextCPUBurst;
          }

          else if (1.0 > rndm && rndm >= 1.0 - p2)
          {
            displayingArr[((PCB *)arg)->pid - 1][7]++;
            if (OUTMODE == 3)
            {
              printf("Process thread %d is going IO2\n", ((PCB *)arg)->pid);
            }
            pthread_mutex_lock(&mutexIO2);

            while (threadsSleepingOnIO2 > 0)
            {

              pthread_cond_wait(&device2Cond, &mutexIO2);
            }

            threadsSleepingOnIO2++;
            pthread_mutex_unlock(&mutexIO2);
    
            if (OUTMODE == 3)
            {
              printf("Process Thread  %d is using device 2\n", (*(PCB *)arg).pid);
            }
            gettimeofday(&currentTime, NULL);
            if (OUTMODE == 2)
            {
              printf("%d  %d USING DEVICE2 \n", (int)(currentTime.tv_sec - startTime.tv_sec) * 1000 + (int)(currentTime.tv_usec - startTime.tv_usec) / 1000, (*(PCB *)arg).pid);
            }
            usleep(T2 * 1000);

            pthread_mutex_lock(&mutexIO2);
            threadsSleepingOnIO2--;
            pthread_mutex_unlock(&mutexIO2);

            pthread_cond_signal(&device2Cond);
            if (OUTMODE == 3)
            {
              printf("Process thread %d is coming from IO2\n", ((PCB *)arg)->pid);
            }
            ((PCB *)arg)->remainingCPUBurst = ((PCB *)arg)->nextCPUBurst;
          }
        }
        else
        {
          if (OUTMODE == 3)
          {
            printf("Process thread %d is coming from RR slicing\n", ((PCB *)arg)->pid);
          }
        }
      }


    displayingArr[((PCB *)arg)->pid - 1][3] = displayingArr[((PCB *)arg)->pid - 1][3] + rqWait;
    rqWait = 0;

  }

  
  pthread_exit(NULL);
}

void *cpuScheduler(void *arg)
{
 
  while (generatedThreadNumber < ALLP || threadsInTheSystem > 0)
  {

    pthread_mutex_unlock(&mutexThreadNumber);
    
    pthread_mutex_lock(&mutexNodeForCPU);
    
   
    if (cpu.pid == -1 && getHeadData(readyQueue).pid != -1 &&  getSleepingNodeNumber(readyQueue) == getNodeNumber(readyQueue))
    {

      pthread_mutex_lock(&mutexRQ);
 
      
      int index;
      if ((strcmp(ALG, "FCFS") == 0) || (strcmp(ALG, "RR") == 0))
      {
        selectedPid = getHeadData(readyQueue).pid; 
        
      
        cpu = getHeadData(readyQueue);

          
         setPCBState(&readyQueue,1,1);
      }

      else if ((strcmp(ALG, "SJF") == 0))
      {

        cpu = searchMin(readyQueue, &index);
        selectedPid = getData(readyQueue, index).pid;
       
      }

      pthread_mutex_unlock(&mutexRQ);
     
      
      if (OUTMODE == 3)
      {
        printf("Scheduler selected pid: %d\n", selectedPid);
      }
      
      pthread_cond_broadcast(&nodeSelectedForCPU);

    }

    pthread_mutex_unlock(&mutexNodeForCPU);

    

    if (generatedThreadNumber < ALLP || threadsInTheSystem > 0)
    {
   
      pthread_cond_wait(&schedulerWakeUp, &mutexAkin);
    }

    pthread_mutex_lock(&mutexThreadNumber);

  }
  pthread_mutex_lock(&mutexRQ);


  selectedPid = getHeadData(readyQueue).pid; 

  cpu = getHeadData(readyQueue);

  pthread_mutex_unlock(&mutexRQ);
  pthread_cond_broadcast(&nodeSelectedForCPU);
  printf("Scheduler Finished\n");
  pthread_exit(NULL);
}

void *processGenerator(void *arg_ptr)
{

  pthread_t pThreads[ALLP]; 
  PCB t_args[ALLP];         

  // Creating 10 processes
  int u;
  int x = 0;
  for (int i = 0; i < 10; i++)
  {

    if (strcmp(burst_dist, "fixed") == 0)
    {
      t_args[i].nextCPUBurst = fixedCPUBurst;
      t_args[i].remainingCPUBurst = fixedCPUBurst;
    }
    else if (strcmp(burst_dist, "exponential") == 0)
    {
      
    	u = ((double)rand()) / (double)((unsigned)RAND_MAX + 1);
        x = -((double) log(u) /(double) burstlen);
        x = min_burst + (max_burst-min_burst)*x;
        t_args[i].nextCPUBurst = round(x);
      t_args[i].remainingCPUBurst = round(x);
    }
    else if (strcmp(burst_dist, "uniform") == 0)
    {
      t_args[i].nextCPUBurst = min_burst + (max_burst - min_burst) * ((double)rand()) / (double)((unsigned)RAND_MAX + 1);
      t_args[i].remainingCPUBurst = min_burst + (max_burst - min_burst) * ((double)rand()) / (double)((unsigned)RAND_MAX + 1);
    }
    t_args[i].pid = i + 1;

    t_args[i].state = 3; // waiting

    t_args[i].threadId = pThreads[i];
    pthread_mutex_lock(&mutexThreadNumber);
    if (pthread_create(&pThreads[i], NULL, &processThreads, (void *)&(t_args[i])) != 0)
    {
      perror("Failed to create thread");
    }
    else
    {
    }
    if (OUTMODE == 3)
    {
      printf("Process Thread %d created with cpu burst %d\n", i + 1, t_args[i].nextCPUBurst);
    }
    generatedThreadNumber++;

    threadsInTheSystem++;

    pthread_mutex_unlock(&mutexThreadNumber);
  }

  pthread_t th;
  if (pthread_create(&th, NULL, &cpuScheduler, NULL) != 0)
  {
    perror("Failed to create thread");
  }

  while ((generatedThreadNumber >= 10 && generatedThreadNumber < ALLP)) 
  {
    usleep(5000);
  
    double rndm;
    if (threadsInTheSystem < MAXP) 
    {
      rndm = (float)rand() / (float)RAND_MAX;
      if (rndm < pg) 
      {

        if (strcmp(burst_dist, "fixed") == 0)
        {
          t_args[generatedThreadNumber].nextCPUBurst = fixedCPUBurst;
          t_args[generatedThreadNumber].remainingCPUBurst = fixedCPUBurst;
        }
        else if (strcmp(burst_dist, "exponential") == 0)
        {
          u = ((double)rand()) / (double)((unsigned)RAND_MAX + 1);
           x = -((double) log(u) /(double) burstlen);
            x = min_burst + (max_burst-min_burst)*x;
           t_args[generatedThreadNumber].nextCPUBurst = round(x);
           t_args[generatedThreadNumber].remainingCPUBurst = round(x);
        }
        else if (strcmp(burst_dist, "uniform") == 0)
        {
          t_args[generatedThreadNumber].nextCPUBurst = min_burst + (max_burst - min_burst) * ((double)rand()) / (double)((unsigned)RAND_MAX + 1);
          t_args[generatedThreadNumber].remainingCPUBurst = min_burst + (max_burst - min_burst) * ((double)rand()) / (double)((unsigned)RAND_MAX + 1);
        }
        t_args[generatedThreadNumber].pid = generatedThreadNumber + 1;

        t_args[generatedThreadNumber].state = 3;
        t_args[generatedThreadNumber].threadId = pThreads[generatedThreadNumber];
        pthread_mutex_lock(&mutexThreadNumber);
        if (pthread_create(&pThreads[generatedThreadNumber], NULL, &processThreads, (void *)&(t_args[generatedThreadNumber])) != 0)
        {
          perror("Failed to create thread");
        }
        else
        {
        }
        if (OUTMODE == 3)
        {
          printf("Process Thread %d created with cpu burst %d\n", generatedThreadNumber + 1, t_args[generatedThreadNumber].nextCPUBurst);
        }
        threadsInTheSystem++;
        generatedThreadNumber++;
        pthread_mutex_unlock(&mutexThreadNumber);
      }
    }
  }


  for (int i = 0; i < generatedThreadNumber; i++)
  {
    if (pthread_join(pThreads[i], NULL) != 0)
    {

      perror("Failed to join thread ");
    }
  }

  if (pthread_join(th, NULL) != 0)
  {

    perror("Failed to join thread");
  }

  if (OUTMODE == 3)
  {
    printf("Generator finished\n");
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{


  if (argc != 16)
  {
    printf("There must be 15 argument\n");
    exit(0);
  }

  srand(time(NULL));
  ALG = argv[1];
  Q = argv[2];

  Qint = atof(argv[2]);
  T1 = atoi(argv[3]);
  T2 = atoi(argv[4]);
  burst_dist = argv[5];
  burstlen = atoi(argv[6]);
  min_burst = atoi(argv[7]);
  max_burst = atoi(argv[8]);
  p0 = atof(argv[9]);
  p1 = atof(argv[10]);
  p2 = atof(argv[11]);
  pg = atof(argv[12]);
  MAXP = atoi(argv[13]);
  ALLP = atoi(argv[14]);
  OUTMODE = atoi(argv[15]);

  fixedCPUBurst = burstlen;

  for (int i = 0; i < ALLP; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if( j == 0)
      {
      displayingArr[i][j] = -1;
      }
      else
      displayingArr[i][j] = 0;
    }
  }

  if ((strcmp(ALG, "FCFS") == 0) || (strcmp(ALG, "SJF") == 0) || (strcmp(ALG, "RR") == 0))
  {

  }
  else
  {
    printf("Wrong Algorithm\n");
    exit(0);
  }

  gettimeofday(&startTime, NULL);
  readyQueue = createList();

  cpu = emptyPCB();
  pthread_t th[1];
  pthread_mutex_init(&mutexRQ, NULL);
  pthread_mutex_init(&mutexAkin, NULL);
  pthread_mutex_init(&mutexHakan, NULL);
  pthread_mutex_init(&mutexThreadNumber, NULL);
  pthread_mutex_init(&mutexNodeForCPU, NULL);
  pthread_mutex_init(&mutexUyan, NULL);
pthread_mutex_init(&mutexSelectedPid,NULL);
  pthread_mutex_init(&mutexIO1, NULL);

  pthread_mutex_init(&mutexIO2, NULL);


  pthread_cond_init (&rrCond,NULL);

  pthread_cond_init(&condQueue, NULL);
  pthread_cond_init(&schedulerWakeUp, NULL);
  pthread_cond_init(&nodeSelectedForCPU, NULL);

  pthread_cond_init(&device1Cond, NULL);
  pthread_cond_init(&device2Cond, NULL);

  if (pthread_create(&th[0], NULL, &processGenerator, NULL) != 0)
  {
    perror("Failed to create thread");
  }

  for (int i = 0; i < 1; i++)
  {
    if (pthread_join(th[i], NULL) != 0)
    {
      perror("Failed to join thread ");
    }
  }

  pthread_mutex_destroy(&mutexHakan);
  pthread_mutex_destroy(&mutexAkin);
  pthread_mutex_destroy(&mutexRQ);
  pthread_mutex_destroy(&mutexThreadNumber);
  pthread_mutex_destroy(&mutexNodeForCPU);
  pthread_mutex_destroy(&mutexUyan);
  pthread_mutex_destroy(&mutexIO1);
  pthread_mutex_destroy(&mutexIO2);
  pthread_mutex_destroy(&mutexSelectedPid);
  pthread_cond_destroy(&condQueue);
  pthread_cond_destroy(&schedulerWakeUp);
  pthread_cond_destroy(&nodeSelectedForCPU);
  pthread_cond_destroy(&device1Cond);
  pthread_cond_destroy(&device2Cond);

  pthread_cond_destroy (&rrCond);

  printf("pid   arv      dept     cpu      waitr    turna    nbursts  nd1  nd2 \n");
  for (int i = 0; i < ALLP; i++)
  {
    printf("%-5d %-8d %-8d %-8d %-8d %-8d %-8d %-4d %d \n", i + 1, displayingArr[i][0] , displayingArr[i][1], displayingArr[i][2], displayingArr[i][3], displayingArr[i][4], displayingArr[i][5], displayingArr[i][6], displayingArr[i][7]);
  }
  exit(0);
}

// main code end
