#ifndef DRIVER_H
#define DRIVER_H

#include <string>
#include <pthread.h>

using namespace std;

/*
 * DRIVER CLASS - Represents a shared resource (limited driver availability)
 * Demonstrates: Shared resource management, state transitions, thread-safe access
 * 
 * IMPROVEMENTS:
 * - Driver ratings (1-5 stars)
 * - Total earnings tracking
 * - Average rating calculation
 */

class Driver {
private:
    int driverId;
    bool isAvailable;
    int currentCustomerId;
    string driverName;
    int ridesCompleted;
    double totalEarnings;
    
    // NEW: Ratings
    int totalRatings;        // Sum of all ratings
    int ratingCount;         // Number of ratings received
    double averageRating;    // Average rating

public:
    Driver(int id, string name);
    ~Driver();

    // Getters
    int getDriverId() const;
    bool getAvailability() const;
    int getCurrentCustomerId() const;
    string getDriverName() const;
    int getRidesCompleted() const;
    double getTotalEarnings() const;
    
    // NEW: Rating getters
    double getAverageRating() const;
    int getRatingCount() const;

    // Setters - These will be protected by mutex in dispatch system
    void setAvailable(bool status);
    void setCurrentCustomerId(int customerId);
    void incrementRides();
    void addEarnings(double amount);
    
    // NEW: Rating setters
    void addRating(int rating);

    // Display driver status
    string getStatusString() const;
};

#endif
