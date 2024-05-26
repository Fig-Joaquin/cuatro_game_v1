# Nombre del compilador
CXX = g++

# Opciones de compilación
CXXFLAGS = -Wall -std=c++11 -pthread

# Archivos fuente
SRCS = client.cpp server.cpp fourmodel.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Ejecutables
TARGETS = client server

# Reglas de compilación
all: $(TARGETS)

client: client.o fourmodel.o
	$(CXX) $(CXXFLAGS) -o client client.o fourmodel.o

server: server.o fourmodel.o
	$(CXX) $(CXXFLAGS) -o server server.o fourmodel.o

%.o: %.cpp fourmodel.h
	$(CXX) $(CXXFLAGS) -c $<

# Regla para limpiar archivos objeto y ejecutables
clean:
	rm -f $(OBJS) $(TARGETS)
