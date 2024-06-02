# Makefile

CXX = g++
CXXFLAGS = -Wall -std=c++11 -pthread

all: client server

client: client.o fourmodel.o
	$(CXX) $(CXXFLAGS) -o client client.o fourmodel.o

server: server.o fourmodel.o
	$(CXX) $(CXXFLAGS) -o server server.o fourmodel.o

client.o: client.cpp fourmodel.h
	$(CXX) $(CXXFLAGS) -c client.cpp

server.o: server.cpp fourmodel.h
	$(CXX) $(CXXFLAGS) -c server.cpp

fourmodel.o: fourmodel.cpp fourmodel.h
	$(CXX) $(CXXFLAGS) -c fourmodel.cpp

clean:
	rm -f *.o client server
