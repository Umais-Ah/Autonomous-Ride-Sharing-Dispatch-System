#include "request.h"
#include <sstream>

using namespace std;

RideRequest::RideRequest(int reqId, int custId, string pickup, string destination)
    : requestId(reqId), customerId(custId), pickupLocation(pickup), 
      destinationLocation(destination), isCompleted(false), assignedDriverId(-1),
      isCancelled(false), cancellationReason(CancellationReason::NONE),
      fare(0.0), distance(0.0), rideDuration(0), driverRating(0) {
    requestTime = chrono::system_clock::now();
}

RideRequest::~RideRequest() {}

int RideRequest::getRequestId() const {
    return requestId;
}

int RideRequest::getCustomerId() const {
    return customerId;
}

string RideRequest::getPickupLocation() const {
    return pickupLocation;
}

string RideRequest::getDestinationLocation() const {
    return destinationLocation;
}

bool RideRequest::getIsCompleted() const {
    return isCompleted;
}

int RideRequest::getAssignedDriverId() const {
    return assignedDriverId;
}

chrono::system_clock::time_point RideRequest::getRequestTime() const {
    return requestTime;
}

chrono::system_clock::time_point RideRequest::getAssignmentTime() const {
    return assignmentTime;
}

double RideRequest::getWaitingTime() const {
    if (assignedDriverId == -1) {
        // Still waiting, so measure from now.
        auto now = chrono::system_clock::now();
        chrono::duration<double> elapsed = now - requestTime;
        return elapsed.count();
    } else {
        // Already assigned, so measure up to the assignment time.
        chrono::duration<double> elapsed = assignmentTime - requestTime;
        return elapsed.count();
    }
}

void RideRequest::setAssignmentTime() {
    assignmentTime = chrono::system_clock::now();
}

void RideRequest::setCompletionTime() {
    completionTime = chrono::system_clock::now();
}

void RideRequest::setCompleted(bool status) {
    isCompleted = status;
}

void RideRequest::setAssignedDriverId(int driverId) {
    assignedDriverId = driverId;
}

string RideRequest::getStatusString() const {
    stringstream ss;
    ss << "Customer " << customerId << " (Req#" << requestId << "): ";
    ss << pickupLocation << " -> " << destinationLocation << " | ";
    
    if (isCancelled) {
        ss << "CANCELLED (" << getCancellationReasonString() << ")";
    } else if (assignedDriverId == -1) {
        ss << "WAITING (" << getWaitingTime() << "s)";
    } else {
        ss << "Driver " << assignedDriverId;
        if (fare > 0) {
            ss << " | Rs." << fare;
        }
    }
    
    return ss.str();
}

bool RideRequest::getIsCancelled() const {
    return isCancelled;
}

CancellationReason RideRequest::getCancellationReason() const {
    return cancellationReason;
}

string RideRequest::getCancellationReasonString() const {
    switch (cancellationReason) {
        case CancellationReason::NONE: return "None";
        case CancellationReason::CUSTOMER_REQUESTED: return "Customer Cancelled";
        case CancellationReason::DRIVER_REJECTED: return "Driver Rejected";
        case CancellationReason::NO_DRIVER_AVAILABLE: return "No Driver Available";
        case CancellationReason::TIMEOUT: return "Request Timeout";
        default: return "Unknown";
    }
}

void RideRequest::cancelRide(CancellationReason reason) {
    isCancelled = true;
    cancellationReason = reason;
}

double RideRequest::getFare() const {
    return fare;
}

double RideRequest::getDistance() const {
    return distance;
}

int RideRequest::getRideDuration() const {
    return rideDuration;
}

void RideRequest::setFare(double fareAmount) {
    fare = fareAmount;
}

void RideRequest::setDistance(double distanceKm) {
    distance = distanceKm;
}

void RideRequest::setRideDuration(int minutes) {
    rideDuration = minutes;
}

int RideRequest::getDriverRating() const {
    return driverRating;
}

void RideRequest::setDriverRating(int rating) {
    if (rating >= 1 && rating <= 5) {
        driverRating = rating;
    }
}
