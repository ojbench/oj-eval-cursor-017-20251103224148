CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall

TARGET = code
SOURCES = main.cpp
HEADERS = TicketSystem.hpp BPlusTree.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) *.dat *.log

.PHONY: all clean
