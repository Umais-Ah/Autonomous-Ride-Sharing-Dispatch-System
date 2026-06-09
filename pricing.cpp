#include "pricing.h"
#include <ctime>
#include <sstream>

using namespace std;

PricingEngine::PricingEngine() {}

PricingEngine::~PricingEngine() {}

double PricingEngine::calculateFare(int durationMinutes, double distanceKm, bool isPeakHour) {
    // Build the base fare from time and distance.
    double baseFare = BASE_FARE;
    double timeFare = durationMinutes * PER_MINUTE_RATE;
    double distanceFare = distanceKm * PER_KM_RATE;
    
    double totalFare = baseFare + timeFare + distanceFare;
    
    // Keep the fare above the minimum.
    if (totalFare < MINIMUM_FARE) {
        totalFare = MINIMUM_FARE;
    }
    
    // Add surge if the ride is in a busy window.
    totalFare = applySurgeMultiplier(totalFare, isPeakHour);
    
    return totalFare;
}

bool PricingEngine::isPeakHour() const {
    int hour = getCurrentHour();
    // Morning and evening rush hours.
    return (hour >= 8 && hour <= 10) || (hour >= 17 && hour <= 19);
}

int PricingEngine::getCurrentHour() const {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    return localtime(&time)->tm_hour;
}

double PricingEngine::applySurgeMultiplier(double baseFare, bool isPeakHour) const {
    if (isPeakHour) {
        return baseFare * PEAK_HOUR_MULTIPLIER;
    }
    return baseFare;
}

string PricingEngine::getPricingInfo() const {
    stringstream ss;
    ss << "Pricing: Base=" << BASE_FARE << " Rs. | ";
    ss << "Time=" << PER_MINUTE_RATE << " Rs/min | ";
    ss << "Distance=" << PER_KM_RATE << " Rs/km | ";
    ss << "Surge=" << (PEAK_HOUR_MULTIPLIER * 100.0 - 100.0) << "% during peak";
    return ss.str();
}
