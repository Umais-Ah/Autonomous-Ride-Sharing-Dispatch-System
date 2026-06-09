#include "queue.h"
#include <iostream>

using namespace std;

RequestQueue::RequestQueue() : totalRequestsProcessed(0) {
    // Keep the queue protected from the start.
    pthread_mutex_init(&queueMutex, NULL);
    pthread_mutex_init(&processedMutex, NULL);
    
    // Start empty so the dispatcher waits for work.
    sem_init(&requestSemaphore, 0, 0);
}

RequestQueue::~RequestQueue() {
    // Release queue resources cleanly.
    pthread_mutex_destroy(&queueMutex);
    sem_destroy(&requestSemaphore);
    pthread_mutex_destroy(&processedMutex);
}

void RequestQueue::enqueueRequest(RideRequest* request) {
    // Lock the queue, push the request, and notify the dispatcher.
    pthread_mutex_lock(&queueMutex);
    
    requestQueue.push(request);
    
    pthread_mutex_unlock(&queueMutex);
    
    sem_post(&requestSemaphore);
}

RideRequest* RequestQueue::dequeueRequest() {
    // Sleep until a request shows up.
    sem_wait(&requestSemaphore);
    
    // Pull the next request while holding the lock.
    pthread_mutex_lock(&queueMutex);
    
    RideRequest* request = requestQueue.front();
    requestQueue.pop();
    
    pthread_mutex_unlock(&queueMutex);
    
    return request;
}

int RequestQueue::getQueueSize() const {
    // Good enough for display purposes.
    return requestQueue.size();
}

bool RequestQueue::isEmpty() const {
    return requestQueue.empty();
}

int RequestQueue::getTotalProcessed() const {
    // Lock the processed counter to return a stable snapshot.
    pthread_mutex_lock((pthread_mutex_t*)&processedMutex);
    int val = totalRequestsProcessed;
    pthread_mutex_unlock((pthread_mutex_t*)&processedMutex);
    return val;
}

void RequestQueue::incrementProcessed() {
    pthread_mutex_lock(&processedMutex);
    totalRequestsProcessed++;
    pthread_mutex_unlock(&processedMutex);
}
