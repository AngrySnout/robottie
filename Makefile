PLATFORM:= $(shell ./config.guess)

CC:=g++
CFLAGS:=-c -g -std=c++11 -ILuaBridge
LDFLAGS:=-lpthread -L/usr/lib -llua -ldl -lcurl
SOURCE_FILES:=$(wildcard src/*.cpp)
OBJECTS:=$(SOURCE_FILES:.cpp=.o)
BUILD_DIR:=bin

all: $(SOURCE_FILES) ircclient
	
ircclient: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf src/*.o ircclient

src/client.o: src/sock.h src/client.h
src/main.o: src/client.h src/tracker.h
src/sock.o: src/sock.h
