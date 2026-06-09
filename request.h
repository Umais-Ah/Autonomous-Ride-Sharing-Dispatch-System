#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <chrono>

using namespace std;

// Cancellation reasons
enum class CancellationReason {
    NONE,
    CUSTOMER_REQUESTED,
    DRIVER_REJECTED,
    NO_DRIVER_AVAILABLE,
    TIMEOUT
};

/*
 * REQUEST CLASS - Represents a customer ride request
 * Demonstrates: Producer behavior, request lifecycle, timestamps
 * Each request runs in its own thread and enters the shared queue
 * 
 * IMPROVEMENTS:
 * - Ride cancellation support
 * - Pricing tracking
 * - Driver ratings
 * - Distance tracking
 */

class RideRequest {
private:
    int requestId;
    int customerId;
    string pickupLocation;
    string destinationLocation;
    chrono::system_clock::time_point requestTime;
    chrono::system_clock::time_point assignmentTime;
    chrono::system_clock::time_point completionTime;
    bool isCompleted;
    int assignedDriverId;
    
    // NEW: Cancellation support
    bool isCancelled;
    CancellationReason cancellationReason;
    
    // NEW: Pricing
    double fare;
    double distance;      // in km
    int rideDuration;     // in minutes
    
    // NEW: Ratings
    int driverRating;     // 1-5 stars

public:
    RideRequest(int reqId, int custId, string pickup, string destination);
    ~RideRequest();

    // Getters
    int getRequestId() const;
    int getCustomerId() const;
    string getPickupLocation() const;
    string getDestinationLocation() const;
    bool getIsCompleted() const;
    int getAssignedDriverId() const;
    
    // NEW: Cancellation getters
    bool getIsCancelled() const;
    CancellationReason getCancellationReason() const;
    string getCancellationReasonString() const;
    
    // NEW: Pricing getters
    double getFare() const;
    double getDistance() const;
    int getRideDuration() const;
    
    // NEW: Rating getters
    int getDriverRating() const;
    
    // Time tracking
    chrono::system_clock::time_point getRequestTime() const;
    chrono::system_clock::time_point getAssignmentTime() const;
    double getWaitingTime() const;

    // Setters
    void setAssignmentTime();
    void setCompletionTime();
    void setCompleted(bool status);
    void setAssignedDriverId(int driverId);
    
    // NEW: Cancellation setters
    void cancelRide(CancellationReason reason);
    
    // NEW: Pricing setters
    void setFare(double fareAmount);
    void setDistance(double distanceKm);
    void setRideDuration(int minutes);
    
    // NEW: Rating setters
    void setDriverRating(int rating);

    // Display request status
    string getStatusString() const;
};

#endif
