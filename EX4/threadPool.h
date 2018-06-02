// Or Paz
// 311250708
#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include "osqueue.h"
#include <pthread.h>
#define ALLOCATION_ERR "Error: Cannot allocate memory."
#define INIT_MUTEX_ERR "Error: Could not initialize cond or mutex."
#define SYSCALL "Error in system call"



/**
 * Thread Pool.
 */
typedef struct thread_pool
{
 //The field x is here because a struct without fields
 //doesn't compile. Remove it once you add fields of your own
    int thread_count;
    pthread_mutex_t lock;
    pthread_t* threads;
    OSQueue* tasksQueue;
    pthread_cond_t notify;
    int isActive;
    int shouldWaitForTasks;


 //TODO - FILL THIS WITH YOUR FIELDS

} ThreadPool;
/**
 * task.
 */
typedef struct task_t{
    void (*computeFunc)(void *);
    void *argument;
} Task;




/**
 * this method create ThreadPool
 * @param numOfThreads - number of thread
 * @return ThreadPoll
 */
ThreadPool* tpCreate(int numOfThreads);
/**is method destroy thread pool and free all resources.
 * @param threadPool - pointer to ThreadPool
 * @param shouldWaitForTasks symbolize if we need to wait all task or not.
 */
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);
/**
 *
 * @param threadPool - pointer to ThreadPool
 * @param computeFunc - pointer to function
 * @param param - argument ,if there is, to function
 * @return return 0 if success or -1 if not.
 */
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

/**
 * write to STDERR.
 */
void error(const char* msg);
/**
 * this method start when thread is created. pointer to function.
 * it took task from queue and run it.
 * @param args - arguments
 * @return - void*
 */
void* execute(void *args);

#endif
