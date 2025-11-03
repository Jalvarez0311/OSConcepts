#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int buf[8], in=0, out=0, count=0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;

//producer thread
void* producer(void* a){
  int n = *(int*)a;
  for (int x=1; x<=n; x++){
    pthread_mutex_lock(&mtx);//lock the mutex before modifying
    while (count==8) pthread_cond_wait(&cv, &mtx);//note that the pthread function is in the while loop
    buf[in] = x; in = (in+1)%8; count++;
    pthread_cond_signal(&cv); //signal that new item available
    pthread_mutex_unlock(&mtx);//unlocks mutex so other threads can access
  }
  return NULL;
}

void* consumer(void* a){
  int n = *(int*)a;
  for (int i=0; i<n; i++){
    pthread_mutex_lock(&mtx);//locks the mutex so other threads can't access
    while (count==0) pthread_cond_wait(&cv, &mtx);//same while loop as before
    int x = buf[out]; out = (out+1)%8; count--;
    pthread_cond_signal(&cv);//signal to producer there is space available
    pthread_mutex_unlock(&mtx);//unlock mutex so other threads can use
    printf("%d\n", x);
  }
  return NULL;
}

int main(int c, char** v){
  int N = atoi(v[1]);
  pthread_t p,t;
  pthread_create(&p,NULL,producer,&N);//create threads
  pthread_create(&t,NULL,consumer,&N);
  pthread_join(p,NULL);//wait for threads to finish
  pthread_join(t,NULL);
  return 0;
}
