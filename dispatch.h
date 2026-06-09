#ifndef DISPATCH_H
#define DISPATCH_H

#include "driver.h"
#include "request.h"
#include "queue.h"
#include "pricing.h"
#include <vector>
#include <pthread.h>
#include <stdexcept>

using namespace std;

// NEW: System metrics structure
struct SystemMetrics {
    int totalRequests;
    int completedRides;
    int cancelledRides;
    double averageWaitTime;
    double driverUtilizationRate;
    double totalRevenue;
};

/*
 * DISPATCH SYSTEM - Core orchestrator for ride assignment
 * 
 * OS Concepts Demonstrated:
 * 1. SHARED RESOURCE MANAGEMENT: Drivers are shared resources (limited pool)
 * 2. MUTEX PROTECTION: Driver pool is protected from concurrent modification
 * 3. CRITICAL SECTIONS: Driver allocation is atomic operation
 * 4. RACE CONDITION PREVENTION: Only one thread can assign a driver at a time
 * 5. DEADLOCK AVOIDANCE: Simple linear lock ordering (only one mutex)
 * 
 * IMPROVEMENTS:
 * - Exception handling for error cases
 * - Pricing system integration
 * - System metrics tracking
 * - Ride cancellation support
 * - Driver ratings integration
 */

class DispatchSystem {
private:
    vector<Driver*> drivers;
    RequestQueue* requestQueue;
    PricingEngine* pricingEngine;
    
    // MUTEX: Protects driver pool from concurrent access
    pthread_mutex_t driverPoolMutex;
    
    // NEW: Statistics tracking
    int totalRidesCompleted;
    int totalRidesCancelled;
    double totalEarnings;
    double totalWaitTime;
    int totalRequestsProcessed;
    // For round-robin driver selection
    int nextDriverIndex;
    bool isRunning;

public:
    DispatchSystem(int numDrivers, RequestQueue* queue);
    ~DispatchSystem();

    // Core functions
    int findAvailableDriver();
    bool assignDriverToRequest(int driverId, RideRequest* request);
    void releaseDriver(int driverId);
    
    // NEW: Cancellation support
    void cancelRide(int requestId, CancellationReason reason);
    
    // Main dispatch loop
    void dispatchLoop();

    // Statistics getters
    int getTotalCompleted() const;
    int getTotalCancelled() const;
    double getTotalEarnings() const;
    int getTotalDrivers() const;
    int getAvailableDriversCount();
    
    // NEW: Metrics getter
    SystemMetrics getSystemMetrics();
    double getAverageWaitTime() const;
    double getDriverUtilizationRate();

    // Get driver list for display
    vector<Driver*> getDrivers() const;

    // Check system status
    bool getIsRunning() const;
    void setIsRunning(bool status);

    // Clean up resources
    void cleanup();
};

#endif
