#include "driver.h"
#include <sstream>

using namespace std;

Driver::Driver(int id, string name)
    : driverId(id), isAvailable(true), currentCustomerId(-1), 
      driverName(name), ridesCompleted(0), totalEarnings(0.0),
      totalRatings(0), ratingCount(0), averageRating(0.0) {}

Driver::~Driver() {}

int Driver::getDriverId() const {
    return driverId;
}

bool Driver::getAvailability() const {
    return isAvailable;
}

int Driver::getCurrentCustomerId() const {
    return currentCustomerId;
}

string Driver::getDriverName() const {
    return driverName;
}

int Driver::getRidesCompleted() const {
    return ridesCompleted;
}

double Driver::getTotalEarnings() const {
    return totalEarnings;
}

void Driver::setAvailable(bool status) {
    isAvailable = status;
}

void Driver::setCurrentCustomerId(int customerId) {
    currentCustomerId = customerId;
}

void Driver::incrementRides() {
    ridesCompleted++;
}

void Driver::addEarnings(double amount) {
    totalEarnings += amount;
}

double Driver::getAverageRating() const {
    return averageRating;
}

int Driver::getRatingCount() const {
    return ratingCount;
}

void Driver::addRating(int rating) {
    if (rating >= 1 && rating <= 5) {
        totalRatings += rating;
        ratingCount++;
        averageRating = (double)totalRatings / ratingCount;
    }
}

string Driver::getStatusString() const {
    stringstream ss;
    ss << "Driver " << driverId << " (" << driverName << ") -> ";
    
    if (isAvailable) {
        ss << "Available";
    } else {
        ss << "Busy (Customer " << currentCustomerId << ")";
    }
    
    // Show the rating beside the status.
    ss << " | Rating: ";
    if (ratingCount > 0) {
        ss << averageRating << "/5.0 (" << ratingCount << " reviews)";
    } else {
        ss << "No ratings";
    }
    
    return ss.str();
}
