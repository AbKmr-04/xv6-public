#include "types.h"
#include "user.h"
#include "thread.h"

#define NUM_THREADS 2
#define NUM_INCREMENTS 100000

volatile int shared_counter = 0;
ticket_lock_t counter_lock;

void incrementer(void *arg1, void *arg2) {
    int thread_num = *(int*)arg1;
    printf(1, "Thread %d: Starting...\n", thread_num);
    
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        lock_acquire(&counter_lock);
        shared_counter++;
        lock_release(&counter_lock);
    }
    
    printf(1, "Thread %d: Finished (%d increments).\n", thread_num, NUM_INCREMENTS);
    exit();
}

int main(int argc, char *argv[]) {
    int pid;
    int args[NUM_THREADS];
    
    printf(1, "Main: Starting test with %d threads, %d increments each...\n",
           NUM_THREADS, NUM_INCREMENTS);
    
    lock_init(&counter_lock);
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i] = i + 1;
        pid = thread_create(incrementer, &args[i], 0);
        printf(1, "Main: Created thread %d\n", pid);
    }
    
    // Wait for threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_join();
    }
    
    printf(1, "Main: All threads completed.\n");
    printf(1, "Main: Final counter value: %d\n", shared_counter);
    printf(1, "Main: Expected counter value: %d\n", NUM_THREADS * NUM_INCREMENTS);
    
    exit();
}