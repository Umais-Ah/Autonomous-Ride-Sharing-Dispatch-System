#include "driver.h"
#include "request.h"
#include "queue.h"
#include "dispatch.h"
#include "logger.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>
#include <chrono>
#include <fstream>
#include <deque>

using namespace std;

// Main simulation file for the dispatch system.

// Shared objects used by the simulation.
DispatchSystem* dispatchSystem = NULL;
RequestQueue* requestQueue = NULL;
bool systemRunning = true;
int requestIdCounter = 1;
pthread_mutex_t requestIdMutex = PTHREAD_MUTEX_INITIALIZER;

// Keep customer thread handles so we can join them later.
vector<pthread_t> customerThreads;
vector<pthread_t> dispatcherThreads;
int configuredDispatchers = 2;
int configuredCustomers = 4;
chrono::steady_clock::time_point runtimeStart;

// Small helpers for the premium dashboard output.
const string UI_RESET = "\033[0m";
const string UI_BOLD = "\033[1m";
const string UI_DIM = "\033[2m";
const string UI_CYAN = "\033[36m";
const string UI_GREEN = "\033[32m";
const string UI_YELLOW = "\033[33m";
const string UI_RED = "\033[31m";
const string UI_BLUE = "\033[34m";
const string UI_MAGENTA = "\033[35m";

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

string centerText(const string& text, int width) {
    if ((int)text.size() >= width) return text;
    int leftPadding = (width - (int)text.size()) / 2;
    int rightPadding = width - leftPadding - (int)text.size();
    return string(leftPadding, ' ') + text + string(rightPadding, ' ');
}

string repeatChar(char ch, int count) {
    return string(count, ch);
}

string formatMoney(double amount) {
    stringstream ss;
    ss << fixed << setprecision(0) << amount;
    return ss.str();
}

string currentTimestamp() {
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    tm* localTime = localtime(&currentTime);

    stringstream ss;
    ss << put_time(localTime, "%H:%M:%S");
    return ss.str();
}

string formatRuntime() {
    long elapsed = chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - runtimeStart).count();
    int mins = (int)(elapsed / 60);
    int secs = (int)(elapsed % 60);
    stringstream ss;
    ss << setfill('0') << setw(2) << mins << ":" << setw(2) << secs;
    return ss.str();
}

string fitText(const string& text, size_t width) {
    if (text.size() <= width) return text + string(width - text.size(), ' ');
    if (width <= 3) return text.substr(0, width);
    return text.substr(0, width - 3) + "...";
}

string utilizationBar(double percent, int width) {
    if (width < 4) width = 4;
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    int filled = (int)((percent / 100.0) * width + 0.5);
    if (filled > width) filled = width;
    return string(filled, '=') + string(width - filled, '.');
}

vector<string> tailLogLines(const string& path, int count) {
    ifstream in(path.c_str());
    deque<string> window;
    string line;
    while (getline(in, line)) {
        if ((int)window.size() == count) window.pop_front();
        window.push_back(line);
    }
    return vector<string>(window.begin(), window.end());
}

string colorizeEvent(const string& line) {
    if (line.find("WARNING") != string::npos || line.find("cancelled") != string::npos) {
        return UI_RED + line + UI_RESET;
    }
    if (line.find("ASSIGNED") != string::npos) {
        return UI_CYAN + line + UI_RESET;
    }
    if (line.find("COMPLETED") != string::npos) {
        return UI_GREEN + line + UI_RESET;
    }
    return UI_DIM + line + UI_RESET;
}

// Sample locations for the ride requests.
const string pickupLocations[] = {
    "Downtown",
    "Mall",
    "Airport",
    "Station",
    "Park",
    "Hospital",
    "School",
    "Market"
};

const string destinationLocations[] = {
    "Home",
    "Office",
    "Hotel",
    "Theater",
    "Restaurant",
    "Beach",
    "Museum",
    "Library"
};

void* customerThreadFunction(void* arg) {
    int customerId = *(int*)arg;
    delete (int*)arg;
    
    while (systemRunning) {
        // Grab a unique request id.
        pthread_mutex_lock(&requestIdMutex);
        int requestId = requestIdCounter++;
        pthread_mutex_unlock(&requestIdMutex);
        
        // Pick a random trip.
        string pickup = pickupLocations[rand() % 8];
        string destination = destinationLocations[rand() % 8];
        
        // Build the request object.
        RideRequest* request = new RideRequest(requestId, customerId, pickup, destination);
        
        // Log it before it goes into the queue.
        Logger::getInstance()->log("Customer " + to_string(customerId) + 
                                 " requested ride: " + pickup + " -> " + destination);
        
        // Push the request for the dispatcher.
        requestQueue->enqueueRequest(request);
        
        // Customers do not all show up at the same pace.
        sleep(3 + (rand() % 5));
    }
    
    return NULL;
}

// Dispatcher thread: keeps pulling requests and assigning drivers.
void* dispatcherThreadFunction(void* arg) {
    DispatchSystem* dispatch = (DispatchSystem*)arg;
    dispatch->dispatchLoop();
    return NULL;
}

// Dashboard thread: refreshes the live screen.
void* dashboardThreadFunction(void* arg) {
    DispatchSystem* dispatch = (DispatchSystem*)arg;
    while (systemRunning) {
        sleep(2);
        if (!systemRunning) {
            break;
        }
        clearScreen();

        SystemMetrics metrics = dispatch->getSystemMetrics();
        vector<Driver*> drivers = dispatch->getDrivers();
        int queueSize = requestQueue->getQueueSize();
        int available = dispatch->getAvailableDriversCount();
        int totalDrivers = dispatch->getTotalDrivers();
        int busy = totalDrivers - available;

        double avgRating = 0.0;
        int ratedDrivers = 0;
        for (size_t i = 0; i < drivers.size(); ++i) {
            if (drivers[i]->getRatingCount() > 0) {
                avgRating += drivers[i]->getAverageRating();
                ratedDrivers++;
            }
        }
        if (ratedDrivers > 0) avgRating /= ratedDrivers;

        vector<string> events = tailLogLines("dispatch_system.log", 7);

        cout << "\n";
        cout << UI_CYAN << UI_BOLD << "+------------------------------------------------------------------------------+" << UI_RESET << "\n";
        cout << UI_CYAN << UI_BOLD << "|" << centerText("AUTONOMOUS RIDE-SHARING DISPATCH SYSTEM", 78) << "|" << UI_RESET << "\n";
        cout << UI_CYAN << UI_BOLD << "|" << centerText("Premium Live Operations Dashboard", 78) << "|" << UI_RESET << "\n";
        cout << UI_CYAN << "| " << UI_DIM << "Live @ " << currentTimestamp() << " | Runtime: " << formatRuntime()
             << " | Threads: " << (configuredDispatchers + configuredCustomers + 1)
             << " | pthreads + mutexes + semaphores" << UI_RESET
             << fitText("", 78 - (52 + (int)to_string(configuredDispatchers + configuredCustomers + 1).size() + (int)formatRuntime().size() + (int)currentTimestamp().size()))
             << UI_CYAN << "|" << UI_RESET << "\n";
        cout << UI_CYAN << UI_BOLD << "+------------------------------------------------------------------------------+" << UI_RESET << "\n\n";

        cout << UI_BLUE << "+------------------+ +------------------+ +------------------+ +------------------+" << UI_RESET << "\n";
        cout << UI_BLUE << "|" << centerText("Total Rides", 18) << "| |" << centerText("Completed", 18)
             << "| |" << centerText("Cancelled", 18) << "| |" << centerText("Revenue", 18) << "|" << UI_RESET << "\n";
        cout << "|" << UI_BOLD << centerText(to_string(metrics.totalRequests), 18) << UI_RESET
             << "| |" << UI_GREEN << UI_BOLD << centerText(to_string(metrics.completedRides), 18) << UI_RESET
             << "| |" << UI_RED << UI_BOLD << centerText(to_string(metrics.cancelledRides), 18) << UI_RESET
             << "| |" << UI_YELLOW << UI_BOLD << centerText("Rs." + formatMoney(metrics.totalRevenue), 18) << UI_RESET << "|\n";
        cout << UI_BLUE << "+------------------+ +------------------+ +------------------+ +------------------+" << UI_RESET << "\n\n";

        cout << UI_MAGENTA << UI_BOLD << "+-- DRIVER FLEET --------------------------------------------------------------+" << UI_RESET << "\n";
        cout << "| Total: " << setw(2) << totalDrivers
             << "   Available: " << setw(2) << available
             << "   Busy: " << setw(2) << busy
             << "   Utilization: " << setw(5) << fixed << setprecision(1) << metrics.driverUtilizationRate << "%  "
             << UI_CYAN << utilizationBar(metrics.driverUtilizationRate, 24) << UI_RESET
             << " |\n";
        cout << "|------------------------------------------------------------------------------|\n";

        for (size_t i = 0; i < drivers.size(); i++) {
            string left = string("Driver ") + to_string(drivers[i]->getDriverId()) + " (" + drivers[i]->getDriverName() + ")";
            string state;
            if (drivers[i]->getAvailability()) {
                state = UI_GREEN + string("FREE") + UI_RESET;
            } else {
                state = UI_RED + string("BUSY") + UI_RESET + " (Customer " + to_string(drivers[i]->getCurrentCustomerId()) + ")";
            }

            string rating = "* ";
            if (drivers[i]->getRatingCount() > 0) {
                stringstream rs;
                rs << fixed << setprecision(1) << drivers[i]->getAverageRating();
                rating += rs.str() + "/5.0";
            } else {
                rating += "N/A";
            }
            rating += "  " + to_string(drivers[i]->getRidesCompleted()) + " rides";

            cout << "| " << fitText(left, 22) << " " << state
                 << fitText("", (drivers[i]->getAvailability() ? 17 : 6))
                 << fitText(rating, 24) << " |\n";
        }
        cout << UI_MAGENTA << UI_BOLD << "+------------------------------------------------------------------------------+" << UI_RESET << "\n\n";

        cout << UI_CYAN << UI_BOLD << "+-- SYNC EVENTS (live) --------------------------------------------------------+" << UI_RESET << "\n";
        if (events.empty()) {
            cout << "| " << UI_DIM << "No events yet..." << UI_RESET << fitText("", 61) << "|\n";
        } else {
            for (size_t i = 0; i < events.size(); ++i) {
                string line = events[i];
                if (line.size() > 76) line = line.substr(line.size() - 76);
                cout << "| " << colorizeEvent(fitText(line, 76)) << " |\n";
            }
        }
        cout << UI_CYAN << UI_BOLD << "+------------------------------------------------------------------------------+" << UI_RESET << "\n\n";

        cout << UI_BLUE << UI_BOLD << "+-- QUEUE + PERFORMANCE --------------------------------------------------------+" << UI_RESET << "\n";
        cout << "| Waiting: " << setw(3) << queueSize
             << "   Avg Wait: " << setw(5) << fixed << setprecision(1) << metrics.averageWaitTime << "s"
             << "   Revenue: " << UI_YELLOW << "Rs." << formatMoney(metrics.totalRevenue) << UI_RESET
             << "   Avg Rating: " << UI_CYAN << fixed << setprecision(1) << avgRating << "/5.0" << UI_RESET
             << " |\n";
        cout << "| Processed: " << setw(3) << metrics.totalRequests
             << "   Completed: " << setw(3) << metrics.completedRides
             << "   Cancelled: " << setw(3) << metrics.cancelledRides
             << "   Driver Util: " << setw(4) << fixed << setprecision(1) << metrics.driverUtilizationRate << "% |\n";
        cout << UI_BLUE << UI_BOLD << "+------------------------------------------------------------------------------+" << UI_RESET << "\n\n";
    }

    return NULL;
}

void printSystemInfo() {
    cout << "\n";
    cout << UI_CYAN << UI_BOLD << "╔" << repeatChar('=', 72) << "╗" << UI_RESET << "\n";
    cout << UI_CYAN << UI_BOLD << "║" << centerText("AUTONOMOUS RIDE-SHARING DISPATCH SYSTEM", 72) << "║" << UI_RESET << "\n";
    cout << UI_CYAN << UI_BOLD << "║" << centerText("OS Project | Multithreading + Synchronization", 72) << "║" << UI_RESET << "\n";
    cout << UI_CYAN << UI_BOLD << "╚" << repeatChar('=', 72) << "╝" << UI_RESET << "\n\n";

    cout << UI_BOLD << "SYSTEM SNAPSHOT" << UI_RESET << "\n";
    cout << "  " << UI_GREEN << "Drivers:" << UI_RESET << " 5   " << UI_GREEN << "Customers:" << UI_RESET << " 4   " << UI_GREEN << "Threads:" << UI_RESET << " dispatcher(s) + dashboard + 4 customers\n";
    cout << "  " << UI_GREEN << "Sync:" << UI_RESET << " pthreads, mutexes, semaphores   " << UI_GREEN << "Pattern:" << UI_RESET << " producer-consumer\n\n";

    cout << UI_BOLD << "WHAT THIS DEMONSTRATES" << UI_RESET << "\n";
    cout << "  " << UI_BLUE << "•" << UI_RESET << " Multithreading and shared resource coordination\n";
    cout << "  " << UI_BLUE << "•" << UI_RESET << " Mutex-protected critical sections\n";
    cout << "  " << UI_BLUE << "•" << UI_RESET << " Semaphore-driven request queueing\n";
    cout << "  " << UI_BLUE << "•" << UI_RESET << " Driver ratings, fares, and live metrics\n";
    cout << "  " << UI_BLUE << "•" << UI_RESET << " Thread-safe logging and race-condition prevention\n\n";

    cout << UI_DIM << "Press any key to launch the simulation..." << UI_RESET << "\n";
    cin.get();
}

void cleanup() {
    cout << "\n\n" << UI_BOLD << UI_YELLOW << "Shutting down system..." << UI_RESET << "\n";
    
    // Tell the threads to wrap up.
    systemRunning = false;
    
    // Wait for the dispatchers first.
    for (size_t i = 0; i < dispatcherThreads.size(); ++i) {
        pthread_join(dispatcherThreads[i], NULL);
    }
    
    // Then wait for the customer threads.
    for (size_t i = 0; i < customerThreads.size(); i++) {
        pthread_join(customerThreads[i], NULL);
    }
    
    // Tear down the core objects.
    if (dispatchSystem) {
        dispatchSystem->cleanup();
        delete dispatchSystem;
    }
    
    if (requestQueue) {
        delete requestQueue;
    }
    
    cout << UI_GREEN << "System shutdown complete." << UI_RESET << "\n";
    cout << UI_DIM << "Detailed logs are available in dispatch_system.log" << UI_RESET << "\n\n";
}

int main(int argc, char** argv) {
    srand(time(NULL));
    runtimeStart = chrono::steady_clock::now();
    
    // Show the intro screen.
    printSystemInfo();
    
    // Build the core system objects.
    requestQueue = new RequestQueue();
    dispatchSystem = new DispatchSystem(5, requestQueue);
    
    Logger::getInstance()->log("========== SYSTEM STARTED ==========");
    Logger::getInstance()->log("Initial drivers: 5");
    Logger::getInstance()->log("Initial customers: 4 (concurrent threads)");

    // Number of dispatcher threads (default 2) can be overridden by first arg.
    int numDispatchers = 2;
    if (argc > 1) {
        int val = atoi(argv[1]);
        if (val > 0) numDispatchers = val;
    }

    // Number of customer generator threads (default 4) can be overridden by second arg.
    int numCustomers = 4;
    if (argc > 2) {
        int c = atoi(argv[2]);
        if (c > 0) numCustomers = c;
    }
    configuredDispatchers = numDispatchers;
    configuredCustomers = numCustomers;
    for (int i = 0; i < numDispatchers; ++i) {
        pthread_t dThread;
        pthread_create(&dThread, NULL, dispatcherThreadFunction, dispatchSystem);
        dispatcherThreads.push_back(dThread);
    }

    // Start the customer generators.
    for (int i = 1; i <= numCustomers; i++) {
        int* customerId = new int(i);
        pthread_t thread;
        pthread_create(&thread, NULL, customerThreadFunction, customerId);
        customerThreads.push_back(thread);
    }

    // Start the live dashboard.
    pthread_t dashboardThread;
    pthread_create(&dashboardThread, NULL, dashboardThreadFunction, dispatchSystem);

    Logger::getInstance()->log("All threads created successfully");
    Logger::getInstance()->log("Dispatchers: " + to_string(numDispatchers) + ", Customers: " + to_string(numCustomers));

    // Let the simulation run for 30 seconds.
    sleep(30);

    // Stop the live dashboard before printing the final summary so it stays visible.
    systemRunning = false;
    pthread_join(dashboardThread, NULL);
    
    cout << "\n\n\n";
    cout << UI_CYAN << UI_BOLD << "╔" << repeatChar('=', 72) << "╗" << UI_RESET << "\n";
    cout << UI_CYAN << UI_BOLD << "║" << centerText("SIMULATION TIME LIMIT REACHED", 72) << "║" << UI_RESET << "\n";
    cout << UI_CYAN << UI_BOLD << "║" << centerText("Final Statistics", 72) << "║" << UI_RESET << "\n";
    cout << UI_CYAN << UI_BOLD << "╠" << repeatChar('=', 72) << "╣" << UI_RESET << "\n";
    cout << "║  Completed Rides : " << setw(3) << dispatchSystem->getTotalCompleted() << "                                        ║\n";
    cout << "║  Available Drivers: " << setw(3) << dispatchSystem->getAvailableDriversCount() << "                                       ║\n";
    cout << "║  Pending Requests : " << setw(3) << requestQueue->getQueueSize() << "                                        ║\n";
    cout << "╚" << repeatChar('=', 72) << "╝\n\n";
    
    // Shut everything down.
    cleanup();
    
    return 0;
}
