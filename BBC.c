#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


sem_t sem_filled_slot;
sem_t sem_empty_slot;
sem_t fill_one_slot;
sem_t *sem_queue_check;
int *queue_length;
pthread_t *cooks,*cashiers,*customers;
int cnt_burger=0;
int cnt_cook,cnt_cashier,cnt_customer,rack_size;
int *rank_cashier,*rank_cook,*rank_customer;
#define COOK_TIME 10000

void *cook(void *p);
void *customer(void *p);
void *cashier(void *p);

int main(int argc,char **argv){
    srand(time(NULL));
    int i;
    if(argc!=5) {
        printf("Invalid variable number.\n");
        exit(-1);
    }
    queue_length=(int *)malloc(sizeof(int)*cnt_cashier);

    cnt_cook=atoi(argv[1]);
    cnt_cashier=atoi(argv[2]);
    cnt_customer=atoi(argv[3]);
    rack_size=atoi(argv[4]);
    queue_length=(int *)malloc(sizeof(int)*cnt_cashier);
    //scanf("%d%d%d%d",&cnt_cook,&cnt_cashier,&cnt_customer,&rack_size);
    sem_queue_check=(sem_t *)malloc(sizeof(sem_t)*cnt_cashier);

    rank_customer=(int *)malloc(sizeof(int)*cnt_customer);
    for(i=0;i<cnt_customer;i++) rank_customer[i]=i;

    rank_cook=(int *)malloc(sizeof(int)*cnt_cook);
    for(i=0;i<cnt_cook;i++) rank_cook[i]=i;

    rank_cashier=(int *)malloc(sizeof(int)*cnt_cashier);
    for(i=0;i<cnt_cashier;i++) rank_cashier[i]=i;

    for(i=0;i<cnt_cashier;i++) sem_init(sem_queue_check+i,0,1);
    sem_init(&sem_filled_slot,0,0);
    sem_init(&sem_empty_slot,0,rack_size);
    sem_init(&fill_one_slot,0,1);

    cooks=malloc(sizeof(pthread_t)*cnt_cook);
    cashiers=malloc(sizeof(pthread_t)*cnt_cashier);
    customers=malloc(sizeof(pthread_t)*cnt_customer);
    for(i=0;i<cnt_cook;i++) {
        pthread_create(cooks+i,NULL,cook,rank_cook+i);
        //pthread_join(cook[i],NULL);
    }
    for(i=0;i<cnt_customer;i++) {
        pthread_create(customers+i,NULL,customer,rank_customer+i);
    }
    for(i=0;i<cnt_customer;i++) pthread_join(customers[i],NULL);
    for(i=0;i<cnt_cashier;i++) pthread_join(cashiers[i],NULL);
    for(i=0;i<cnt_cook;i++) pthread_cancel(cooks[i]);
    free(sem_queue_check);
    free(cooks);
    free(cashiers);
    free(customers);
    free(rank_cook);
    free(rank_cashier);
    free(rank_customer);
    free(queue_length);
    return 0;
}

void *cook(void *p){
    int rank=*(int *)p;
    while(1){
        sem_wait(&sem_empty_slot);
        int i;
        for(i=0;i<COOK_TIME;i++);
        sem_wait(&fill_one_slot);
        cnt_burger++;
        printf("Cook [%d] makes a burger.\n",rank);
        sem_post(&fill_one_slot);
        sem_post(&sem_filled_slot);
    }
}

void *cashier(void *p){//assign his or her ID
    int rank=*(int *)p;
    while(1){
        printf("Cashier [%d] accepts an order.\n",rank);

        sem_wait(&sem_filled_slot);
        cnt_burger--;
        sem_post(&sem_empty_slot);

        printf("Cashier [%d] takes a burger to a customer.\n",rank);

        sem_wait(sem_queue_check+rank);
        if((--queue_length[rank])==0) {
            sem_post(sem_queue_check+rank);
            printf("Cashier [%d] goes to sleep.\n",rank);
            pthread_exit(0);//sleep
        }else{
            sem_post(sem_queue_check+rank);
        }

    }
}

void *customer(void *p){//assign his or her ID
    int rank=*(int *)p;
    printf("Customer [%d] comes.\n",rank);

    int rank_wakeup=rand()%cnt_cashier;//choose a queue at random and queue up

    sem_wait(sem_queue_check+rank_wakeup);
    if((queue_length[rank_wakeup]++)==0) {//wake up customer[rank]
        pthread_create(cashiers+rank_wakeup,NULL,cashier,rank_cashier+rank_wakeup);
        printf("cashier[%d] was woken up.\n",rank_cashier[rank_wakeup]);
    }
    sem_post(sem_queue_check+rank_wakeup);

}