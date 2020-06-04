#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;// Initializing the mutex
pthread_cond_t readersQ = PTHREAD_COND_INITIALIZER; // Inititalizing the readers Condition Variable Queue
pthread_cond_t writersQ = PTHREAD_COND_INITIALIZER; // Inititalizing the writers Condition Variable Queue

int readers = 0; // no. of acvtive readers
int writers = 0; // no. of writers ready to write
int active_writers = 0; // no. of active writers

int global_data_RW = 8; // every thread will try to read and write on this shared variable

void read_lock() {
    pthread_mutex_lock(&m); // Lock the mutex
    while (!(writers == 0))
        pthread_cond_wait(&readersQ, &m);
    readers++;
    pthread_mutex_unlock(&m); // Unlock the mutex
}

void read_unlock() {
    pthread_mutex_lock(&m); // Lock the mutex
    if (--readers == 0)
        pthread_cond_signal(&writersQ); // if readers == 0 then signal on witers CV Queue
    pthread_mutex_unlock(&m); // Unlock the mutex
}

void writer_lock() {
    pthread_mutex_lock(&m); // Lock the mutex
    writers++; // Incrementing the number of writer threads ready to write
    while (!((readers == 0) && (active_writers == 0))) {
        pthread_cond_wait(&writersQ, &m); // wait on wirters CV queue
    }
    active_writers++;
    pthread_mutex_unlock(&m); // Unlock the mutex
}

void writer_unlock( ) {
    pthread_mutex_lock(&m); // lock the mutex
    writers--; // decrease the writers ready to write by 1
    active_writers--; // decrease the active_writers by 1
    if (writers > 0)
        pthread_cond_signal(&writersQ); // If  there are still writers to write (ready to write) signal on writers CV Queue
    else
        pthread_cond_broadcast(&readersQ); // If no writers to write (ready to write) wake up all the threads on the readers cv queue
    pthread_mutex_unlock(&m); // unlock the mutex
}

void read_sync_function(int thr_no){
    read_lock();
    printf("thread number %d read the shared value and it was %d.\n",thr_no,global_data_RW);
    read_unlock();
} // synchronization function for reader threads

void write_sync_function(int increment_val){
    writer_lock();
    global_data_RW += increment_val;
    printf("The writer thread increments the value by %d and the new global data value is %d.\n",increment_val,global_data_RW);
    writer_unlock();
} // synchronization function for writer threads

void *read_first_procedure(void *a){
    int c = (int) a;
    
    int upper = 180;
    int lower = 120;
    int rand_wait_time = (rand() % (upper - lower + 1)) + lower; // this is to exhibit the actual behaviour. Not all read threads will take
    //same time to read every thread will be different say (executes different code)
    
    usleep(rand_wait_time); // this is added to create
    read_sync_function(c);
    return(0);
}// first procedure of reader thread

void *write_first_procedure(void *a){
    int increment_val  =(int) a;
    write_sync_function(increment_val);
    return(0);
}// first procedure of writer thread

int main(){
    pthread_t thr[50]; // an array of 50 thread identifiers for reader
    // I will make 2 writer threads and the 1st will try to increase the gloabl data value by 4 and the 2nd would try to increase it by 7
    pthread_t writer_T1;
    pthread_t writer_T2;
    
    for(int i=0;i<50;i++){
        pthread_create(&thr[i],0,read_first_procedure,(void *)(size_t)i);
    }// the order in which threads will be created is not from1 to 50, it can be in any order (like 3,5,1,2..)
    
    // Creating 2 writer threads
    pthread_create(&writer_T1,0,write_first_procedure,(void *) 4);
    pthread_create(&writer_T2,0,write_first_procedure,(void *) 7);
    
    for(int i=0;i<50;i++){
        pthread_join(thr[i],0);
    } // joining with all the reader threads
    
    pthread_join(writer_T1,0);
    pthread_join(writer_T2,0); // Joining with both writer threads
    
    return(0);
}
