

<div align="center">
<img width="689" height="639" alt="image" src="https://github.com/user-attachments/assets/3bfd95ad-6f61-4d04-99cf-8ca9754ca966" />

# 🚖 Autonomous Ride-Sharing Dispatch System


![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![POSIX Threads](https://img.shields.io/badge/Pthreads-Multithreading-green)
![OS Project](https://img.shields.io/badge/Operating%20Systems-Project-orange)
![Platform](https://img.shields.io/badge/Linux-Unix-red)
![Status](https://img.shields.io/badge/Status-Completed-success)

### Real-Time Multithreaded Ride Dispatch Simulation using POSIX Threads

*A production-inspired autonomous ride-sharing system demonstrating core Operating System concepts including multithreading, mutexes, semaphores, synchronization, producer-consumer architecture, and resource management.*

</div>

---

## 🎥 Project Demo


[▶️ Watch Demo Video](https://github.com/user-attachments/assets/7d15ccec-cd75-4e66-be9c-fe4e16622174)

---

## 📸 Screenshots

### 🚀 Startup Screen

<p align="center">
  <img width="664" height="330" alt="image" src="https://github.com/user-attachments/assets/95ffd867-3363-4c04-896e-b00d469a0dc9" />

</p>

---

### 📊 Live Dashboard

<p align="center">
 <img width="776" height="603" alt="image" src="https://github.com/user-attachments/assets/d951d1f0-3a8d-41a5-ad9f-abd6e078e8bd" />
</p>

---

### 📝 Thread-Safe Log Output

<p align="center">
  <img width="1254" height="773" alt="image" src="https://github.com/user-attachments/assets/54c5d257-13d4-4d19-9e72-178cc297299b" />
</p>

---

### 📈 Final Statistics

<p align="center">
 <img width="638" height="169" alt="image" src="https://github.com/user-attachments/assets/f46ffa75-bdbe-41b9-854b-5cc2e98a8d61" />
</p>

---

# 📖 Overview

The **Autonomous Ride-Sharing Dispatch System** is a multithreaded C++ simulation designed to model how modern ride-hailing platforms manage ride requests, driver allocation, dispatch operations, and fleet monitoring.

The system demonstrates practical implementation of Operating System concepts through a realistic ride dispatch workflow where:

* Customers generate ride requests concurrently
* Dispatchers process and assign rides
* Drivers act as finite shared resources
* Synchronization prevents race conditions
* Semaphores coordinate producer-consumer communication
* Live dashboards provide real-time monitoring

---

# ✨ Key Features

## 🧵 Concurrency & Synchronization

* Multiple customer threads generate requests simultaneously
* Multiple dispatcher threads process rides concurrently
* POSIX mutex protection for all critical sections
* Semaphore-based queue synchronization
* Deadlock prevention through consistent lock ordering

## 🚗 Smart Driver Management

* Shared driver pool
* Dynamic driver assignment
* Availability tracking
* Driver utilization monitoring
* Ride completion handling

## 💰 Dynamic Pricing Engine

Fare calculation based on:

* Base Fare
* Distance Charge
* Time Charge
* Peak Hour Multiplier

## 📊 Real-Time Monitoring

* Live Dashboard
* Fleet Status
* Revenue Tracking
* Queue Statistics
* Driver Performance Metrics

## 📝 Thread-Safe Logging

* Console Logging
* Persistent Log Storage
* Timestamped Events
* Colorized Output

---

# 🏗️ System Architecture

```text
+--------------------+
| Customer Threads   |
| (Producers)        |
+---------+----------+
          |
          v
+--------------------+
| Request Queue      |
| Mutex + Semaphore  |
+---------+----------+
          |
          v
+--------------------+
| Dispatcher Threads |
| (Consumers)        |
+---------+----------+
          |
          v
+--------------------+
| Driver Pool        |
| Shared Resource    |
+---------+----------+
          |
          v
+--------------------+
| Pricing Engine     |
+---------+----------+
          |
          v
+--------------------+
| Metrics & Logger   |
+--------------------+
```

---

# ⚙️ Operating System Concepts Demonstrated

| Concept               | Implementation                           |
| --------------------- | ---------------------------------------- |
| Multithreading        | Customer, Dispatcher & Dashboard Threads |
| Synchronization       | Shared Resource Coordination             |
| Mutexes               | Critical Section Protection              |
| Semaphores            | Queue Signaling Mechanism                |
| Producer-Consumer     | Request Processing Workflow              |
| Resource Management   | Driver Allocation System                 |
| Deadlock Prevention   | Consistent Lock Ordering                 |
| Race Condition Safety | Protected Shared State                   |

---

# 📂 Project Structure

```bash
.
├── main.cpp
├── dispatch.h
├── dispatch.cpp
├── request.h
├── request.cpp
├── queue.h
├── queue.cpp
├── driver.h
├── driver.cpp
├── pricing.h
├── pricing.cpp
├── logger.h
├── logger.cpp
├── Makefile
└── dispatch_system.log
```

---

# 🔧 Build Instructions

## Prerequisites

* Linux / Unix
* GCC with C++17
* POSIX Threads
* GNU Make

## Build

```bash
make clean
make
```

---

# ▶️ Run

### Default Configuration

```bash
./dispatch
```

### Custom Thread Configuration

```bash
./dispatch 2 4
```

Where:

```bash
Argument 1 → Dispatcher Threads
Argument 2 → Customer Threads
```

Example:

```bash
./dispatch 3 6
```

---

# 📊 Runtime Dashboard

The dashboard continuously displays:

* Total Requests
* Completed Rides
* Cancelled Rides
* Revenue Generated
* Driver Utilization
* Queue Depth
* Average Fare
* Driver Ratings

---

# 📈 Sample Workflow

```text
Customer Request
        ↓
Request Queue
        ↓
Dispatcher Picks Request
        ↓
Driver Assigned
        ↓
Ride Simulated
        ↓
Fare Calculated
        ↓
Driver Rated
        ↓
Metrics Updated
        ↓
Driver Released
```

---

# 🚀 Future Improvements

* Database Integration
* Persistent Ride History
* Driver Break Scheduling
* Customer Cancellation Support
* Analytics Dashboard
* Dynamic Dispatcher Scaling
* REST API Integration
* Unit Testing Suite

---

# 👨‍💻 Team Members

| Name                 | ID       |
| -------------------- | -------- |
| Umais Ahmed          | 24K-1003 |
| Abeer Siddiqui       | 24K-0538 |
| Musab Sheikh         | 24K-0862 |

---

# 🎓 Course Information

**Course:** Operating Systems

**Project Type:** Multithreaded System Simulation

**Language:** C++17

**Technology:** POSIX Threads (pthread)

**Platform:** Linux / Unix

---

<div align="center">

### ⭐ If you found this project interesting, consider giving it a star!

Built with ❤️ using C++, POSIX Threads, Mutexes, and Semaphores.

</div>
