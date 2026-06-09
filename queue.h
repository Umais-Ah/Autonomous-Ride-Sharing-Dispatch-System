#ifndef QUEUE_H
#define QUEUE_H

#include "request.h"
#include <queue>
#include <pthread.h>
#include <semaphore.h>

/*
 * THREAD-SAFE QUEUE CLASS - Implements producer-consumer pattern
 * 
 * OS Synchronization Concepts Demonstrated:
 * 1. MUTEX (pthread_mutex_t): Protects shared queue from race conditions
 *    - Ensures only one thread can access queue at a time
 *    - Critical section: actual queue manipulation
 * 
 * 2. SEMAPHORE (sem_t): Controls queue availability
 *    - sem_requests: Counts number of pending requests
 *    - Used to signal when driver can dequeue a request
 * 
 * Race Conditions Prevented:
 * - Multiple threads trying to enqueue simultaneously
 * - Multiple threads trying to dequeue simultaneously
 * - Dequeue on empty queue
 * 
 * Producer-Consumer Pattern:
 * - Producers: Customer threads that enqueue requests
 * - Consumers: Dispatcher thread that dequeues requests
 */

class RequestQueue {
private:
    std::queue<RideRequest*> requestQueue;
    
    // MUTEX: Protects access to requestQueue data structure
    // Prevents race conditions when multiple threads access queue simultaneously
    pthread_mutex_t queueMutex;
    
    // SEMAPHORE: Signals availability of requests in queue
    // Dispatcher thread waits on this semaphore
    // When request is enqueued, semaphore is posted (incremented)
    sem_t requestSemaphore;
    
    int totalRequestsProcessed;
    // Protects the processed counter for thread-safe increment/read
    pthread_mutex_t processedMutex;

public:
    RequestQueue();
    ~RequestQueue();

    /*
     * ENQUEUE: Adds request to queue (Producer - called from customer threads)
     * 
     * Critical Section:
     * 1. Lock mutex (pthread_mutex_lock)
     * 2. Push request to queue
     * 3. Unlock mutex (pthread_mutex_unlock)
     * 4. Post semaphore (sem_post) - signal dispatcher thread
     * 
     * This ensures no two producers modify queue simultaneously
     */
    void enqueueRequest(RideRequest* request);

    /*
     * DEQUEUE: Removes request from queue (Consumer - called from dispatcher)
     * 
     * Critical Section:
     * 1. Wait on semaphore (sem_wait) - blocks if queue is empty
     * 2. Lock mutex
     * 3. Pop request from queue
     * 4. Unlock mutex
     * 
     * Semaphore ensures we don't dequeue on empty queue
     * Mutex prevents data corruption during dequeue
     */
    RideRequest* dequeueRequest();

    // Non-blocking peek at queue size (for dashboard display)
    int getQueueSize() const;

    // Check if queue is empty
    bool isEmpty() const;

    // Get statistics
    int getTotalProcessed() const;
    void incrementProcessed();
};

#endif
