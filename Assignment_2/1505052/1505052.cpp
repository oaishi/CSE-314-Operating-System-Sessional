#include<unistd.h>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
using namespace std;


//semaphore to control sleep and wake up
sem_t empty;
sem_t full;
sem_t emptyvan;
sem_t fullvan;
sem_t emptychoc;
sem_t fullchoc;
queue<int> production;
queue<int> vanilla;
queue<int> chocolate;
pthread_mutex_t lock;
pthread_mutex_t lockprint;
pthread_mutex_t lockvan;
pthread_mutex_t lockchoc;


void init_semaphore()
{
    sem_init(&empty,0,5);
    sem_init(&full,0,0);

    //sem_init(&emptyvan,0,5);
    sem_init(&fullvan,0,0);

    //sem_init(&emptychoc,0,5);
    sem_init(&fullchoc,0,0);

    pthread_mutex_init(&lock,0);
    pthread_mutex_init(&lockprint,0);
    pthread_mutex_init(&lockvan,0);
    pthread_mutex_init(&lockchoc,0);
}

void * ChefX(void * arg)
{
    /*pthread_mutex_lock(&lockprint);
    printf("%s X\n",(char*)arg);
    pthread_mutex_unlock(&lockprint);*/
    int j;
    while(1)
    {

        sem_getvalue( &empty,&j);
        if(j==0){
            pthread_mutex_lock(&lockprint);
            printf("Chef X on tea break\n");
            pthread_mutex_unlock(&lockprint);
        }        

        sem_wait(&empty);
        pthread_mutex_lock(&lock);
        sleep(1);
        production.push(1);
        pthread_mutex_unlock(&lock);
        sem_post(&full);

        pthread_mutex_lock(&lockprint);
        printf("Chef X produced vanilla cake \n");
        pthread_mutex_unlock(&lockprint);
    }
}

void * ChefY(void * arg)
{
    /*pthread_mutex_lock(&lockprint);
    printf("%s Y\n",(char*)arg);
    pthread_mutex_unlock(&lockprint);*/
    int j;
    while(1)
    {

        sem_getvalue( &empty,&j);
        if(j==0){
            pthread_mutex_lock(&lockprint);
            printf("Chef Y on tea break\n");
            pthread_mutex_unlock(&lockprint);
        }        

        sem_wait(&empty);
        pthread_mutex_lock(&lock);
        sleep(1);
        production.push(2);
        pthread_mutex_unlock(&lock);
        sem_post(&full);

        pthread_mutex_lock(&lockprint);
        printf("Chef Y produced chocolate cake \n");
        pthread_mutex_unlock(&lockprint);
    }
}

void * ChefZ(void * arg)
{
    /*pthread_mutex_lock(&lockprint);
    printf("%s\n",(char*)arg);
    pthread_mutex_unlock(&lockprint);*/
    int j;
    while(1)
    {

        sem_getvalue( &full,&j);
        if(j==0){
            pthread_mutex_lock(&lockprint);
            printf("Chef Z on tea break\n");
            pthread_mutex_unlock(&lockprint);
        }

        sem_wait(&full);
        pthread_mutex_lock(&lock);

        sleep(1);
        
        int item = production.front();
        production.pop();
        if(item==1){
            pthread_mutex_unlock(&lock);
            sem_post(&empty);

            pthread_mutex_lock(&lockprint);
            printf("Chef Z decorating vanilla cake\n");
            pthread_mutex_unlock(&lockprint);

            //sem_wait(&emptyvan);
            pthread_mutex_lock(&lockvan);
            vanilla.push(1);
            pthread_mutex_unlock(&lockvan);
            sem_post(&fullvan);

            pthread_mutex_lock(&lockprint);
            printf("Chef Z pushed vanilla cake\n");
            pthread_mutex_unlock(&lockprint);

        }
        else{
            pthread_mutex_unlock(&lock);
            sem_post(&empty);

            pthread_mutex_lock(&lockprint);
            printf("Chef Z decorating chocolate cake\n");
            pthread_mutex_unlock(&lockprint);
            
            //sem_wait(&emptychoc);
            pthread_mutex_lock(&lockchoc);
            chocolate.push(2);
            pthread_mutex_unlock(&lockchoc);
            sem_post(&fullchoc);

            pthread_mutex_lock(&lockprint);
            printf("Chef Z pushed chocolate cake\n");
            pthread_mutex_unlock(&lockprint);
        }

    }
}

void * Waiter1(void * arg)
{
    /*pthread_mutex_lock(&lockprint);
    printf("%s 1\n",(char*)arg);
    pthread_mutex_unlock(&lockprint);*/
    int j;
    while(1)
    {

        sem_getvalue( &fullvan,&j);
        if(j==0){
            pthread_mutex_lock(&lockprint);
            printf("Waiter 1 on tea break\n");
            pthread_mutex_unlock(&lockprint);
        }

        sem_wait(&fullvan);
        pthread_mutex_lock(&lockvan);

        sleep(1);
        
        vanilla.pop();
        pthread_mutex_unlock(&lockvan);
        //sem_post(&emptyvan);

        pthread_mutex_lock(&lockprint);
        printf("Waiter 1 serving vanilla cake\n");
        pthread_mutex_unlock(&lockprint);

    }
}


void * Waiter2(void * arg)
{
    /*pthread_mutex_lock(&lockprint);
    printf("%s 2\n",(char*)arg);
    pthread_mutex_unlock(&lockprint);*/
    int j;
    while(1)
    {

        sem_getvalue( &fullchoc,&j);
        if(j==0){
            pthread_mutex_lock(&lockprint);
            printf("Waiter 2 on tea break\n");
            pthread_mutex_unlock(&lockprint);
        }        

        sem_wait(&fullchoc);
        pthread_mutex_lock(&lockchoc);

        sleep(1);
        
        chocolate.pop();
        pthread_mutex_unlock(&lockchoc);
        //sem_post(&emptychoc);

        pthread_mutex_lock(&lockprint);
        printf("Waiter 2 serving chocolate cake\n");
        pthread_mutex_unlock(&lockprint);
        
    }
}


int main(void)
{
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    pthread_t thread5;

    init_semaphore();

    char * message1 = (char*)"I am producer";
    char * message2 = (char*)"I am decorator";
    char * message3 = (char*)"I am waiter";

    pthread_create(&thread1,NULL,ChefX,(void*)message1 );
    pthread_create(&thread2,NULL,ChefY,(void*)message1 );
    pthread_create(&thread3,NULL,ChefZ,(void*)message2 );
    pthread_create(&thread4,NULL,Waiter1,(void*)message3 );
    pthread_create(&thread5,NULL,Waiter2,(void*)message3 );


    while(1);
    return 0;
}
