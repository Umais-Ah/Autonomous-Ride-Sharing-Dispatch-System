#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std;

Logger* Logger::instance = NULL;

Logger::Logger() : logFileName("dispatch_system.log") {
    pthread_mutex_init(&logMutex, NULL);
    logFile.open(logFileName, ios::app);
}

Logger* Logger::getInstance() {
    // Simple singleton for the logger.
    if (instance == NULL) {
        instance = new Logger();
    }
    return instance;
}

void Logger::log(const string& message) {
    // Keep log lines tidy when many threads write at once.
    pthread_mutex_lock(&logMutex);
    
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    auto ms = chrono::duration_cast<chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    stringstream ss;
    ss << put_time(localtime(&time), "%H:%M:%S");
    ss << "." << setfill('0') << setw(3) << ms.count();
    
    string timestamp = ss.str();
    
    if (logFile.is_open()) {
        logFile << "[" << timestamp << "] " << message << endl;
        logFile.flush();
    }
    
    pthread_mutex_unlock(&logMutex);
}

void Logger::displayLog(const string& message) {
    // Console output gets the same protection.
    pthread_mutex_lock(&logMutex);
    
    cout << message << endl;
    
    pthread_mutex_unlock(&logMutex);
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
    pthread_mutex_destroy(&logMutex);
}
