#ifndef _THREAD_H_
#define _THREAD_H_

// Thread creation and management
int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2);
int thread_join(void); 

// Ticket lock structure and functions
typedef struct ticket_lock {
    volatile uint ticket;     // Next ticket to be given
    volatile uint serving;    // Currently serving ticket
} ticket_lock_t;

void lock_init(ticket_lock_t *lock);
void lock_acquire(ticket_lock_t *lock);
void lock_release(ticket_lock_t *lock);

#endif