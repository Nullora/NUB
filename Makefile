CXX = g++
CXXFLAGS = -std=c++20
TARGET = nub
all: build
build:
	$(CXX) $(CXXFLAGS) nub.cpp -o $(TARGET)
clean:
	rm -rf nub
.PHONY: all build clean