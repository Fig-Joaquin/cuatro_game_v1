# Makefile

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude
LDFLAGS =
EXEC = build/ConnectFourGame
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:src/%.cpp=build/%.o)

# Default target
all: $(EXEC)

# Link the program
$(EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compile the source files into object files
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure the build directory exists
build:
	mkdir -p build

# Clean up build artifacts
clean:
	rm -rf build

# Run the executable
run: $(EXEC)
	./$(EXEC)

.PHONY: all clean run
