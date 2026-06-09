#ifndef PRICING_H
#define PRICING_H

#include <chrono>
#include <cmath>
#include <string>
#include <sstream>

using namespace std;

/*
 * PRICING SYSTEM - Calculates ride fares with surge pricing
 * 
 * Features:
 * - Base fare
 * - Per-minute charges
 * - Surge pricing during peak hours
 * - Distance-based pricing
 */

class PricingEngine {
private:
    // Pricing constants (Pakistani Rupees)
    const double BASE_FARE = 50.0;           // Rs.50 initial charge
    const double PER_MINUTE_RATE = 5.0;      // Rs.5 per minute
    const double PER_KM_RATE = 15.0;         // Rs.15 per km
    const double MINIMUM_FARE = 100.0;       // Rs.100 minimum
    const double PEAK_HOUR_MULTIPLIER = 1.5; // 50% surge during peak
    
public:
    PricingEngine();
    ~PricingEngine();
    
    // Calculate fare based on ride parameters
    double calculateFare(int durationMinutes, double distanceKm, bool isPeakHour = false);
    
    // Check if current time is peak hour (8-10am, 5-7pm)
    bool isPeakHour() const;
    
    // Get current hour (0-23)
    int getCurrentHour() const;
    
    // Apply surge multiplier
    double applySurgeMultiplier(double baseFare, bool isPeakHour) const;
    
    // Get pricing info string
    string getPricingInfo() const;
};

#endif
