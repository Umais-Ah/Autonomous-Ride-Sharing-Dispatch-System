#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <pthread.h>
#include <queue>

/*
 * LOGGER CLASS - Demonstrates thread-safe logging using Singleton pattern
 * 
 * OS Concepts:
 * - Mutex protection for log output
 * - Multiple threads safely writing to same log file
 * - Demonstrates critical sections for I/O operations
 */

class Logger {
private:
    static Logger* instance;
    pthread_mutex_t logMutex;
    std::ofstream logFile;
    std::string logFileName;
    
    // Private constructor for singleton
    Logger();

public:
    // Singleton instance getter
    static Logger* getInstance();
    
    // Thread-safe logging
    // Each log call is a critical section protected by mutex
    void log(const std::string& message);
    
    // Display log to console
    void displayLog(const std::string& message);
    
    ~Logger();
};

#endif
