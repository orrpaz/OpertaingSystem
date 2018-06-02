// Or Paz
// 311250708

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "threadPool.h"
#include "osqueue.h"

void error(const char* msg) {
   // char msg[] = "Error in system call\n";
    write(2, msg, sizeof(msg));
    exit(EXIT_FAILURE);
}


ThreadPool* tpCreate(int numOfThreads) {
    int i,j;
    ThreadPool* threadPool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (threadPool == NULL) {
        exit(EXIT_FAILURE);
    }
    threadPool->thread_count = numOfThreads;

    threadPool->shouldWaitForTasks = 1;

    threadPool->isActive = 1;
    threadPool->tasksQueue = osCreateQueue();
    // allocate thread.
    threadPool->threads = (pthread_t*)malloc(sizeof(pthread_t) * threadPool->thread_count);
    if(threadPool->threads == NULL) {
        error(ALLOCATION_ERR);
        free(threadPool);
        exit(EXIT_FAILURE);
    }
    // initializes mutex.

    if(pthread_mutex_init(&threadPool->lock, NULL) !=0){
        error(INIT_MUTEX_ERR);
    }
    if(pthread_cond_init(&threadPool->notify, NULL) !=0){
        error(INIT_MUTEX_ERR);
    }
    // create pthread.
    for(i = 0; i < threadPool->thread_count ; i++) {
        if(pthread_create(&(threadPool->threads[i]), NULL,execute,(void*)threadPool)!=0) {
            for (j = 0; j < i; j++) {
                pthread_cancel(threadPool->threads[j]);
                pthread_join(threadPool->threads[j], NULL);
            }
//            tpDestroy(threadPool,0);
            free(threadPool->threads);
            free(threadPool);
            error(ALLOCATION_ERR);
            }
    }

    return threadPool;
}

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks) {
    int i;

    if (threadPool == NULL) {
        error(ALLOCATION_ERR);
        exit(EXIT_FAILURE);
    }
    threadPool->isActive = 0;
    threadPool->shouldWaitForTasks = shouldWaitForTasks;

    if (pthread_cond_broadcast(&threadPool->notify) != 0 || pthread_mutex_unlock
                                                                  (&threadPool->lock) != 0) {
//        printf("here2\n");

        error(INIT_MUTEX_ERR);
        exit(EXIT_FAILURE);
    }

    // wait for threads to finish.
    for (i = 0; i < threadPool->thread_count; i++) {
        if (pthread_join(threadPool->threads[i], NULL) != 0) {
            error(SYSCALL);
        }
    }


    while (!osIsQueueEmpty(threadPool->tasksQueue)) {
        Task* task = osDequeue(threadPool->tasksQueue);
        free(task);
    }


    free(threadPool->threads);
    osDestroyQueue(threadPool->tasksQueue);

    pthread_mutex_destroy(&(threadPool->lock));
    pthread_cond_destroy(&threadPool->notify);
    free(threadPool);

}


int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param){
    if(threadPool == NULL) {
        //free???
        error(ALLOCATION_ERR);
        exit(EXIT_FAILURE);
    }


    if (!threadPool->isActive) {
        // return -1 if thread pool is being destroyed.
        return -1;
    }

    Task* task = (Task*)malloc(sizeof(Task));
    if (task == NULL) {
        error(ALLOCATION_ERR);
        tpDestroy(threadPool,0);
        exit(EXIT_FAILURE);
    }

    task->computeFunc = computeFunc;
    task->argument = param;
    pthread_mutex_lock(&(threadPool->lock));

    osEnqueue(threadPool->tasksQueue, (void *)task);
    if (pthread_cond_signal(&(threadPool->notify)) != 0) {
        error(INIT_MUTEX_ERR);
    }
    pthread_mutex_unlock(&(threadPool->lock));

    return 0;
}


void* execute(void *args) {
    ThreadPool *pool = (ThreadPool *)args;
    while(pool->shouldWaitForTasks) {
        pthread_mutex_lock(&pool->lock);
        if (osIsQueueEmpty(pool->tasksQueue)) {
            if (!pool->isActive) {
                pthread_mutex_unlock(&pool->lock);
                break;
            }
            pthread_cond_wait(&pool->notify, &pool->lock);
            pthread_mutex_unlock(&pool->lock);
            continue;
        }
        Task* task = osDequeue(pool->tasksQueue);
        pthread_mutex_unlock(&pool->lock);
        task->computeFunc(task->argument);
        free(task);
    }
}





