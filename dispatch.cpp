#include "dispatch.h"
#include "logger.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

using namespace std;

DispatchSystem::DispatchSystem(int numDrivers, RequestQueue* queue)
        : requestQueue(queue), pricingEngine(new PricingEngine()), 
            totalRidesCompleted(0), totalRidesCancelled(0), totalEarnings(0.0),
            totalWaitTime(0.0), totalRequestsProcessed(0), nextDriverIndex(0), isRunning(true) {
    
    // Build the driver pool up front.
    for (int i = 0; i < numDrivers; i++) {
        string driverName = "Driver_" + to_string(i + 1);
        drivers.push_back(new Driver(i + 1, driverName));
    }
    
    // One mutex keeps driver state changes tidy.
    pthread_mutex_init(&driverPoolMutex, NULL);
}

DispatchSystem::~DispatchSystem() {
    // Free the drivers first.
    for (auto driver : drivers) {
        delete driver;
    }
    drivers.clear();
    
    // Then release the pricing helper.
    if (pricingEngine) {
        delete pricingEngine;
    }
    
    pthread_mutex_destroy(&driverPoolMutex);
}

int DispatchSystem::findAvailableDriver() {
    // Check the pool while nobody else is changing it.
    pthread_mutex_lock(&driverPoolMutex);
    int availableDriverId = -1;

    // Round-robin scan starting from nextDriverIndex to balance load.
    int n = drivers.size();
    for (int offset = 0; offset < n; ++offset) {
        int i = (nextDriverIndex + offset) % n;
        if (drivers[i]->getAvailability()) {
            availableDriverId = drivers[i]->getDriverId();
            // Next search should start after this driver.
            nextDriverIndex = (i + 1) % n;
            break;
        }
    }
    
    pthread_mutex_unlock(&driverPoolMutex);
    return availableDriverId;
}

bool DispatchSystem::assignDriverToRequest(int driverId, RideRequest* request) {
    // Assign the driver and customer in one locked step.
    pthread_mutex_lock(&driverPoolMutex);
    
    bool success = false;
    for (size_t i = 0; i < drivers.size(); i++) {
        if (drivers[i]->getDriverId() == driverId) {
            drivers[i]->setAvailable(false);
            drivers[i]->setCurrentCustomerId(request->getCustomerId());
            success = true;
            break;
        }
    }
    
    pthread_mutex_unlock(&driverPoolMutex);
    if (success) {
        // Save the assignment on the request too.
        request->setAssignedDriverId(driverId);
        request->setAssignmentTime();
        
        // Keep the log readable.
        string logMsg = "Driver " + to_string(driverId) + 
                            " ASSIGNED to Customer " + to_string(request->getCustomerId());
        Logger::getInstance()->log(logMsg);
    }
    
    return success;
}

void DispatchSystem::releaseDriver(int driverId) {
    // Put the driver back in the pool.
    pthread_mutex_lock(&driverPoolMutex);
    
    for (size_t i = 0; i < drivers.size(); i++) {
        if (drivers[i]->getDriverId() == driverId) {
            drivers[i]->setAvailable(true);
            drivers[i]->setCurrentCustomerId(-1);
            drivers[i]->incrementRides();
            break;
        }
    }
    
    pthread_mutex_unlock(&driverPoolMutex);
}

void DispatchSystem::dispatchLoop() {
    // This thread keeps the queue moving.
    Logger::getInstance()->log("[DISPATCHER] Dispatch system started");
    
    while (isRunning) {
        // Wait here until a customer request shows up.
        RideRequest* request = requestQueue->dequeueRequest();
        
        if (!isRunning) break;
        
        // Find a free driver and start the ride.
        int availableDriver = findAvailableDriver();
        
        if (availableDriver != -1) {
            assignDriverToRequest(availableDriver, request);
            
            // Simulate a short trip with a random distance.
            int rideDuration = 2 + (rand() % 4);
            double distanceKm = 2.0 + ((rand() % 8) * 1.0);
            
            double fare = pricingEngine->calculateFare(rideDuration, distanceKm, pricingEngine->isPeakHour());
            request->setFare(fare);
            request->setDistance(distanceKm);
            request->setRideDuration(rideDuration);
            
            sleep(rideDuration);
            
            // Give the driver a simple random rating.
            int driverRating = 1 + (rand() % 5);
            request->setDriverRating(driverRating);
            
            // Update the driver profile with that rating.
            pthread_mutex_lock(&driverPoolMutex);
            for (auto driver : drivers) {
                if (driver->getDriverId() == availableDriver) {
                    driver->addRating(driverRating);
                    break;
                }
            }
            pthread_mutex_unlock(&driverPoolMutex);
            
            releaseDriver(availableDriver);
            request->setCompleted(true);
            request->setCompletionTime();
            
            totalRidesCompleted++;
            totalEarnings += fare;

            // Use the request's measured waiting time (assignment - request)
            // rather than the ride duration when computing average wait.
            double waitSeconds = request->getWaitingTime();
            totalWaitTime += waitSeconds;

            // Mark the queue-level processed counter (thread-safe).
            requestQueue->incrementProcessed();
            
            // Log the ride with the useful bits.
            string logMsg = "Ride COMPLETED: Customer " + to_string(request->getCustomerId()) + 
                           " | Duration: " + to_string(rideDuration) + "s | " +
                           "Distance: " + to_string(distanceKm) + " km | " +
                           "Fare: Rs." + to_string((int)fare) + " | " +
                           "Rating: " + to_string(driverRating) + "/5";
            Logger::getInstance()->log(logMsg);
        } else {
            // If nobody is free, leave a clear note in the log.
            Logger::getInstance()->log("[WARNING] No driver available for Customer " + to_string(request->getCustomerId()));
        }
    }
}

int DispatchSystem::getTotalCompleted() const {
    return totalRidesCompleted;
}

double DispatchSystem::getTotalEarnings() const {
    return totalEarnings;
}

int DispatchSystem::getTotalDrivers() const {
    return drivers.size();
}

int DispatchSystem::getAvailableDriversCount() {
    int count = 0;
    pthread_mutex_lock(&driverPoolMutex);
    
    for (size_t i = 0; i < drivers.size(); i++) {
        if (drivers[i]->getAvailability()) {
            count++;
        }
    }
    
    pthread_mutex_unlock(&driverPoolMutex);
    return count;
}

vector<Driver*> DispatchSystem::getDrivers() const {
    return drivers;
}

bool DispatchSystem::getIsRunning() const {
    return isRunning;
}

void DispatchSystem::setIsRunning(bool status) {
    isRunning = status;
}

void DispatchSystem::cleanup() {
    setIsRunning(false);
}

SystemMetrics DispatchSystem::getSystemMetrics() {
    SystemMetrics metrics;
    // Use the queue's processed counter as the authoritative source.
    metrics.totalRequests = requestQueue->getTotalProcessed();
    metrics.completedRides = totalRidesCompleted;
    metrics.cancelledRides = totalRidesCancelled;
    metrics.totalRevenue = totalEarnings;
    metrics.averageWaitTime = (totalRidesCompleted > 0) ? (totalWaitTime / totalRidesCompleted) : 0.0;
    metrics.driverUtilizationRate = getDriverUtilizationRate();
    return metrics;
}

int DispatchSystem::getTotalCancelled() const {
    return totalRidesCancelled;
}

double DispatchSystem::getAverageWaitTime() const {
    return (totalRidesCompleted > 0) ? (totalWaitTime / totalRidesCompleted) : 0.0;
}

double DispatchSystem::getDriverUtilizationRate() {
    pthread_mutex_lock(&driverPoolMutex);
    int busyCount = 0;
    for (auto driver : drivers) {
        if (!driver->getAvailability()) {
            busyCount++;
        }
    }
    pthread_mutex_unlock(&driverPoolMutex);
    
    return (drivers.size() > 0) ? ((double)busyCount / drivers.size()) * 100.0 : 0.0;
}

void DispatchSystem::cancelRide(int requestId, CancellationReason reason) {
    Logger* logger = Logger::getInstance();
    logger->log("CANCELLATION: Ride request #" + to_string(requestId) + " cancelled");
    totalRidesCancelled++;
}
