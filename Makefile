# Makefile for Autonomous Ride-Sharing Dispatch System
# Compiles all source files with pthreads support

CXX = g++
CXXFLAGS = -Wall -std=c++11 -pthread
LDFLAGS = -pthread

# Target executable
TARGET = dispatch

# Source files
SOURCES = main.cpp driver.cpp request.cpp queue.cpp dispatch.cpp logger.cpp pricing.cpp

# Object files (derived from source files)
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete! Run with: ./dispatch"

# Compile source files to object files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executable
clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(TARGET) dispatch_system.log
	@echo "Clean complete!"

# Run the program
run: $(TARGET)
	@echo "Starting Autonomous Dispatch System..."
	./$(TARGET)

# Help information
help:
	@echo "Available targets:"
	@echo "  make        - Build the project"
	@echo "  make run    - Build and run the project"
	@echo "  make clean  - Remove compiled files"
	@echo "  make help   - Show this help message"

.PHONY: all clean run help
